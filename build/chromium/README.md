<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Chromium build and sync

## Pin

`pin.json` pins Chromium Stable **157.0.8103.78** at commit
`4b3d0dabb8250b6f152d2d85d79e7706e309d087` (release tag, not the moving
branch head). The pin is validated against Chromium Dash.

## Toolchain

- `depot_tools`: `/data/workspace/depot_tools`
- `gn`: `/usr/bin/gn` (`generate-ninja`)
- `ninja`: `/usr/bin/ninja`
- `gclient`, `fetch`, `autoninja`: `depot_tools`

The sandbox has **7.2 GiB free on `/data`**, so it cannot hold a Chromium
checkout or build. A full checkout plus build needs roughly **150 GiB** free,
plus a toolchain download and build artifacts. Run `sync.py --check` before
fetching.

## Commands

```sh
python3 build/chromium/sync.py --check
python3 build/chromium/sync.py --fetch
python3 build/chromium/sync.py --build
```

`--fetch` creates `/data/workspace/chromium` unless `SECANT_CHROMIUM_DIR` or
`--dest` is set, fetches a shallow Chromium checkout, checks out the pinned
release tag, syncs dependencies, applies `patches/chromium/*.patch`, and copies
`secant/` to `src/secant`. Tag fetches and dependency sync retry transient
Googlesource `429` responses, and `gclient sync` runs with four SCM jobs to
avoid rate limits.

`--build` runs `gn gen out/secant-dev` with `args/linux-dev.gn` and builds
`chrome` and `secant_product` by default. Override the targets with repeatable
`--target` flags:

```sh
python3 build/chromium/sync.py --build --target chrome --target secant_product
```

The `chrome` target proves the Chromium browser build. The `secant_product`
target builds the product-layer skeleton through `//secant`. CI builds
`secant_product` first so product-layer compile errors fail before the full
browser build. A Secant-branded executable still requires the app bootstrap and
the browser wiring patch.

## Validation without Chromium

The tab model is dependency-free and can be checked with the system compiler:

```sh
build/chromium/check_model.sh
```

## CI

`.github/workflows/chromium-foundation.yml` runs the pin check, tab model tests,
Chromium fetch, build dependency install, `secant_product` compile, `chrome`
build and `chrome --version` smoke test. It uses
`blacksmith-32vcpu-ubuntu-2404` (32 vCPU, 128 GiB RAM, 1.5 TiB disk), which
satisfies the 150 GiB requirement.

The Linux workflow passes in about 1h30m wall time, including a 1h08m `chrome`
build; the smoke test reports `Chromium 157.0.8103.78`. This sandbox is suitable
for source review, model tests and script validation, not for a full Chromium
build.
