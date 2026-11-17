#!/usr/bin/env python3

import argparse
import json
import os
import shlex
import shutil
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
PIN_PATH = REPO_ROOT / "build" / "chromium" / "pin.json"
ARGS_PATH = REPO_ROOT / "build" / "chromium" / "args" / "linux-dev.gn"
PATCH_DIR = REPO_ROOT / "patches" / "chromium"
SECANT_DIR = REPO_ROOT / "secant"
DEFAULT_DEST = Path(os.environ.get("SECANT_CHROMIUM_DIR", "/data/workspace/chromium"))
MIN_FREE_GIB = 150
DEPOT_TOOLS = Path("/data/workspace/depot_tools")


def load_pin():
    with PIN_PATH.open(encoding="utf-8") as stream:
        return json.load(stream)


def free_gib(path):
    candidate = path
    while not candidate.exists() and candidate != candidate.parent:
        candidate = candidate.parent
    usage = shutil.disk_usage(candidate)
    return usage.free / (1024**3)


def require_tool(name):
    if shutil.which(name):
        return
    if DEPOT_TOOLS.is_dir():
        os.environ["PATH"] = f"{DEPOT_TOOLS}:{os.environ['PATH']}"
    if not shutil.which(name):
        raise SystemExit(f"required tool not found: {name}")


def run(command, cwd):
    print(shlex.join(str(part) for part in command), flush=True)
    subprocess.run([str(part) for part in command], cwd=cwd, check=True)


def check(dest, skip_disk_check):
    pin = load_pin()
    available = free_gib(dest)
    print(f"Chromium pin: {pin['version']} ({pin['commit']})")
    print(f"Destination: {dest}")
    print(f"Free disk: {available:.1f} GiB; required: {MIN_FREE_GIB} GiB")
    if available < MIN_FREE_GIB and not skip_disk_check:
        print("Insufficient disk for a Chromium checkout and build.", file=sys.stderr)
        return 1
    return 0


def apply_patches(src):
    patches = sorted(PATCH_DIR.glob("*.patch"))
    for patch in patches:
        run(["git", "apply", "--3way", patch], src)
    print(f"Applied {len(patches)} Chromium patch(es).")


def copy_secant(src):
    target = src / "secant"
    if target.exists():
        shutil.rmtree(target)
    shutil.copytree(SECANT_DIR, target)
    print(f"Copied {SECANT_DIR} to {target}.")


def fetch(dest, force, skip_disk_check):
    if check(dest, skip_disk_check) != 0:
        raise SystemExit(1)
    require_tool("fetch")
    require_tool("gclient")
    require_tool("git")

    pin = load_pin()
    if (dest / ".gclient").exists() and not force:
        raise SystemExit(f"{dest} already has a checkout; pass --force to replace it")
    if (dest / ".gclient").exists() and force:
        shutil.rmtree(dest)

    dest.mkdir(parents=True, exist_ok=True)
    run(["fetch", "--nohooks", "--no-history", "chromium"], dest)

    src = dest / "src"
    run(["git", "fetch", "--depth", "1", "origin", f"refs/tags/{pin['version']}"], src)
    run(["git", "checkout", "--detach", pin["commit"]], src)
    run(
        [
            "gclient",
            "sync",
            "--no-history",
            "--shallow",
            "--delete_unversioned_trees",
            "--revision",
            f"src@{pin['commit']}",
            "-D",
        ],
        dest,
    )
    run(["gclient", "runhooks"], dest)

    apply_patches(src)
    copy_secant(src)


def build(dest, targets):
    require_tool("gn")
    require_tool("autoninja")
    src = dest / "src"
    if not (src / "BUILD.gn").exists():
        raise SystemExit(f"no Chromium checkout at {src}")
    args = ARGS_PATH.read_text(encoding="utf-8")
    run(["gn", "gen", "out/secant-dev", f"--args={args}"], src)
    run(["autoninja", "-C", "out/secant-dev", *targets], src)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dest", type=Path, default=DEFAULT_DEST)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--fetch", action="store_true")
    parser.add_argument("--build", action="store_true")
    parser.add_argument("--target", action="append", dest="targets")
    parser.add_argument("--force", action="store_true")
    parser.add_argument("--skip-disk-check", action="store_true")
    args = parser.parse_args()

    if not args.check and not args.fetch and not args.build:
        args.check = True
    if args.check and check(args.dest, args.skip_disk_check) != 0:
        return 1
    if args.fetch:
        fetch(args.dest, args.force, args.skip_disk_check)
    if args.build:
        build(args.dest, args.targets or ["chrome", "secant_product"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
