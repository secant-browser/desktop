<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Phase 1 — Foundation spike

**Status:** In progress
**Started:** 2026-11-16
**Gate owner:** Architecture review
**Integration branch:** `feat/chromium-engine-migration`

## Goal

Prove the Chromium foundation and the `//secant` boundary before product-wide
porting. This phase is successful only when the build, shell, tab model,
isolation prototype and measurement harnesses produce evidence.

## Chromium pin

| Item | Value |
| --- | --- |
| Channel | Stable |
| Version | `157.0.8103.78` |
| Milestone | 157 |
| Release tag commit | `4b3d0dabb8250b6f152d2d85d79e7706e309d087` |
| Branch | `refs/branch-heads/8103` |
| Branch head at pin time | `b73459700a57da23a8ae9f624e6093daa04e9a71` |
| Main branch position | `1717349` |
| Source | Chromium Dash Stable/Linux |

The release tag is pinned, not the moving branch head. `build/chromium/pin.json`
is the machine-readable source of truth.

## Toolchain

| Tool | Location | Version |
| --- | --- | --- |
| depot_tools | `/data/workspace/depot_tools` | Current `main` |
| gclient / fetch / autoninja | depot_tools | Installed |
| GN | `/usr/bin/gn` | `1000 (487f835)` |
| Ninja | `/usr/bin/ninja` | `1.12.1` |
| C++ compiler | `/usr/bin/c++` | GCC 14.2.0 |

`depot_tools` is added to `PATH` by `~/.bashrc` and `~/.profile`.

## Implemented scaffold

| Path | Purpose |
| --- | --- |
| `build/chromium/pin.json` | Pinned Chromium release and commit |
| `build/chromium/sync.py` | Check, fetch, patch, copy and build driver |
| `build/chromium/args/linux-dev.gn` | Linux development GN args |
| `build/chromium/check_model.sh` | Dependency-free model test |
| `.github/workflows/chromium-foundation.yml` | CI pin check, tests, build and smoke test |
| `patches/chromium/0001-build-expose-secant-target.patch` | Adds the `secant_product` root group |
| `patches/chromium/` | Reviewed Chromium patch staging area |
| `secant/BUILD.gn` | Product-layer GN entry point |
| `secant/app/` | Browser bootstrap skeleton |
| `secant/browser/tab_model.*` | Initial `SecantTabModel` |
| `secant/browser/tab_model_unittest.cc` | Model tests |
| `secant/webui/` | Privileged WebUI skeleton |

## Validation

| Check | Result |
| --- | --- |
| `python3 -m py_compile build/chromium/sync.py` | Pass |
| `bash -n build/chromium/check_model.sh` | Pass |
| `build/chromium/check_model.sh` | Pass |
| `gn format` on `secant/**/BUILD.gn` | Pass |
| `python3 build/chromium/sync.py --check` | Expected fail: 7.2 GiB free, 150 GiB required |

The tab model currently covers create, close, move, activate, pin, essential
kind, folder metadata, workspace filtering and empty-title fallback. It has no
Chromium dependency yet; the adapter over `TabStripModel` is the next step.

## CI build runner

A Chromium checkout plus build needs roughly **150 GiB** free. This sandbox has
**7.2 GiB** on `/data` and **9.5 GiB** on `/`. It can run source review, model
tests, pin validation and script checks, but it cannot fetch or build Chromium.

`.github/workflows/chromium-foundation.yml` moves the build gate to GitHub
Actions on `blacksmith-32vcpu-ubuntu-2404` (32 vCPU, 128 GiB RAM, 1.5 TiB disk).
It installs `depot_tools`, verifies the pin, runs the tab model tests, fetches
Chromium, installs build dependencies, builds `chrome` and `secant_product`,
and runs `chrome --version`. The first workflow run is pending.

## Next steps

1. Wire `secant_product` into the browser executable and add
   `SecantBrowserMainParts`.
2. Implement `SecantTabModel` as an adapter over `TabStripModel` and keep the
   dependency-free model tests as the behavior specification.
3. Create the native window plus `views::WebView` chrome host.
4. Add the `secant://` WebUI scheme, CSP and Mojo interface.
5. Complete the first Chromium CI build and smoke test, then capture the run
   URL in the migration log.
6. Capture startup, input latency, memory and accessibility baselines.
