<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Migration log

Append-only record of Project Scala decisions, repository operations and gate
results. Do not rewrite earlier entries; correct them with a new entry.

## 2026-11-16 — Phase 0 start: repository assessment

**Repository:** `/data/home/desktop`

### Initial Git state

| Item | Observed |
| --- | --- |
| Current state | Detached `HEAD` at `upstream/dev` |
| `HEAD` | `09431ff0655f4ecce390cb6d20a0677d67abc996` |
| `HEAD` description | `1.24.2b-13-g09431ff06` |
| Local branches | `dev` |
| Remote branches | `origin/dev`, `origin/blacksmith-migration-09431ff`, `upstream/dev` |
| `origin` | `https://github.com/secant-browser/desktop.git` |
| `upstream` | `https://github.com/zen-browser/desktop.git` |
| Uncommitted changes | None |
| Stashes | None |
| Local `dev` | `dcf5b046974098186a317d01a4bacb070955829d` |
| `origin/dev` | `dcf5b046974098186a317d01a4bacb070955829d` |
| Difference `HEAD..dev` | 12 CI workflow files (Blacksmith migration only) |

The working tree was safe to modify: no tracked or untracked changes and no
stashes.

### Branch operation

Created `feat/chromium-engine-migration` from the project default branch
`dev` (`dcf5b0469`), not from detached `upstream/dev` (`09431ff06`). The only
difference is the Blacksmith CI workflow merge, which belongs in the project
baseline. The original `upstream/dev` commit remains reachable and recorded
above. No history was rewritten.

```text
git switch -c feat/chromium-engine-migration dev
```

The branch did not exist before this operation.

### Assessment findings

- This repository is a **Zen product overlay**, not a browser engine checkout.
- `engine/` is absent. Firefox is downloaded by `surfer` at build time.
- Product code: 1,371 files under `src/zen`, of which 1,019 are tests and 352
  are non-test files.
- Non-test source: ~89,949 lines across JS/MJS/CSS/XHTML/C++/headers/Objective-C++.
- Firefox patches: 283 under `src/`.
- Preferences: 193 Zen-specific prefs in 28 YAML files.
- Tests: 232 Zen-specific test files in 32 suites, plus 529 imported upstream
  Firefox tests (761 JS test files total).
- Native XPCOM components: 10 Zen C++/Objective-C++ components plus 4
  Boost-related C++ units and 4 gtest units.
- No Chromium checkout, `depot_tools`, GN, Ninja or `gclient` is installed in
  this environment.

### Decisions

1. Phase 0 is assessment only. No browser code is migrated in this increment.
2. The selected foundation is a pinned Chromium fork with a `//secant` product
   layer; see ADR 0001.
3. The existing Zen code is a **reference and asset source**, not a source of
   Firefox patches to reapply.
4. Parity scores are recorded as 0.0% behavioural and 0.0% visual because no
   Secant build or acceptance run exists. Inventory coverage is 100%.
5. The workspace/container isolation model is the highest-risk parity area and
   requires a dedicated Phase 3 spike before UI work depends on it.

### Phase 0 outputs

- `docs/migration/README.md`
- `docs/migration/phase-0/architecture-analysis.md`
- `docs/migration/phase-0/dependency-map.md`
- `docs/migration/phase-0/feature-inventory.md` and `.csv`
- `docs/migration/phase-0/patch-inventory.csv`
- `docs/migration/phase-0/chromium-integration-evaluation.md`
- `docs/migration/phase-0/migration-plan.md`
- `docs/migration/phase-0/visual-baseline-plan.md`
- `docs/migration/phase-0/licensing.md`
- `docs/migration/phase-0/parity-report.md`
- `docs/migration/phase-0/risk-register.md`
- `docs/migration/adr/0001-chromium-foundation.md`

### Phase 0 document set completed

All documents listed above now exist. Validation on 2026-11-16:

- feature inventory: **182 rows** (40 Core, 30 Browser, 80 Zen UI, 20 Platform,
  12 Security); no duplicate IDs or feature names;
- patch inventory: **283 rows**; dispositions **252 replace, 21 drop, 9 review,
  1 do-not-port**;
- visual baseline: **18 cases** defined, **0 captured**;
- risk register: **25 risks**, including 1 Critical, 20 High and 4 Medium;
- relative Markdown links: **0 missing**;
- no browser build, test run or screenshot capture was performed.

The document set was committed on `feat/chromium-engine-migration` as
`60333386c` (`docs: add Phase 0 Chromium migration assessment`).

### Gate

Phase 0 is **complete pending architecture review**. Phase 1 is not started.

## 2026-11-16 — Phase 1 start: toolchain and foundation scaffold

### Approval

Architecture was approved on 2026-11-16 and depot_tools installation was
authorized. Phase 1 is now **in progress**.

### Chromium pin

| Item | Value |
| --- | --- |
| Channel | Stable |
| Version | `157.0.8103.78` |
| Milestone | 157 |
| Release tag commit | `4b3d0dabb8250b6f152d2d85d79e7706e309d087` |
| Branch | `refs/branch-heads/8103` |
| Branch head at pin time | `b73459700a57da23a8ae9f624e6093daa04e9a71` |
| Main branch position | `1717349` |

`build/chromium/pin.json` is the source of truth. The exact release tag is
pinned, not the moving branch head.

### Toolchain

- `depot_tools` cloned to `/data/workspace/depot_tools`.
- `gclient`, `fetch` and `autoninja` are available from depot_tools.
- `gn` installed as `/usr/bin/gn` (`1000 (487f835)`).
- `ninja` installed as `/usr/bin/ninja` (`1.12.1`).
- `depot_tools` added to `PATH` in `~/.bashrc` and `~/.profile`.

### Scaffold

- `build/chromium/sync.py`: check, fetch, patch, copy and build driver.
- `build/chromium/args/linux-dev.gn`: Linux development GN args.
- `build/chromium/check_model.sh`: dependency-free model test.
- `patches/chromium/`: reviewed patch staging area.
- `secant/`: product-layer GN skeleton.
- `secant/browser/tab_model.*`: initial `SecantTabModel` and tests.
- `docs/migration/phase-1/foundation-spike.md`: Phase 1 status and blockers.

### Validation

- `python3 -m py_compile build/chromium/sync.py`: pass.
- `bash -n build/chromium/check_model.sh`: pass.
- `build/chromium/check_model.sh`: pass.
- `gn format` on `secant/**/BUILD.gn`: pass.
- `python3 build/chromium/sync.py --check`: expected fail, 7.2 GiB free
  versus 150 GiB required.
- relative Markdown links: 0 missing.

### Blocker

This sandbox has 7.2 GiB free on `/data` and 9.5 GiB on `/`. A Chromium
checkout plus build needs roughly 150 GiB. The pin, sync driver and model can
be validated here, but the Chromium fetch and build gate needs a larger runner
with a warm build cache and the platform toolchains.

### Commit

Phase 1 scaffold committed as `d96a49e67`
(`feat: scaffold Chromium Phase 1 foundation`).

### Gate

Phase 1 is **in progress**. It is not complete until the build, shell, tab,
session, isolation, performance and accessibility evidence in
[phase-1/foundation-spike.md](../phase-1/foundation-spike.md) is recorded.
