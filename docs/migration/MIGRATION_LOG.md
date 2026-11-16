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
