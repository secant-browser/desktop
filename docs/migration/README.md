<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Project Scala — Secant Browser Chromium Migration

Internal codename: **Project Scala**
Public product: **Secant Browser**
Source product: **Zen Browser**
Target engine: **Chromium** (Blink + V8)

This directory is the migration control plane. It records the assessment, the
architecture decision, the dependency and feature maps, the parity methodology,
and the phase gates. Phase 0 was approved on 2026-11-16; Phase 1 is in progress.

## Status

| Item | Value |
| --- | --- |
| Phase | **1 — Foundation spike** |
| Phase 0 state | Approved 2026-11-16 |
| Implementation state | Phase 1 scaffold and Linux build green |
| Integration branch | `feat/chromium-engine-migration` |
| Branch base | `dev` @ `dcf5b046974098186a317d01a4bacb070955829d` |
| Working tree at assessment | Clean |
| Browser engine in checkout | **Absent** (`engine/` is not vendored) |
| Chromium pin | **157.0.8103.78** @ `4b3d0dabb8250b6f152d2d85d79e7706e309d087` |
| Chromium build | **Linux green** in CI (`44186036431`); Windows/macOS pending |

The repository is a Zen *overlay*: it contains Zen's product code, Firefox
patches, preferences, packaging and tests, but not the Firefox engine. `surfer`
downloads Mozilla source into `engine/`, applies `src/**/*.patch`, imports
`src/zen`, and builds with `mach`. A Chromium migration therefore cannot be a
patch-replacement exercise; it requires a Chromium fork and a new product layer.

## Phase 0 deliverables

| Document | Purpose |
| --- | --- |
| [architecture-analysis.md](phase-0/architecture-analysis.md) | What the current system is and how it is built |
| [dependency-map.md](phase-0/dependency-map.md) | Firefox/Gecko/SpiderMonkey dependencies by subsystem |
| [feature-inventory.md](phase-0/feature-inventory.md) | Feature inventory schema, status and risk summary |
| [feature-inventory.csv](phase-0/feature-inventory.csv) | Canonical machine-readable feature inventory |
| [patch-inventory.csv](phase-0/patch-inventory.csv) | All Firefox patches with provisional disposition |
| [chromium-integration-evaluation.md](phase-0/chromium-integration-evaluation.md) | Integration options, scoring and selected architecture |
| [migration-plan.md](phase-0/migration-plan.md) | Phases, workstreams, acceptance criteria and branch policy |
| [visual-baseline-plan.md](phase-0/visual-baseline-plan.md) | Screenshot matrix, tolerances and regression method |
| [licensing.md](phase-0/licensing.md) | MPL/Chromium/third-party/trademark obligations |
| [parity-report.md](phase-0/parity-report.md) | Explicit parity scores and evidence |
| [risk-register.md](phase-0/risk-register.md) | Top risks, mitigations and owners |
| [ADR 0001](../migration/adr/0001-chromium-foundation.md) | The Chromium foundation decision record |
| [MIGRATION_LOG.md](MIGRATION_LOG.md) | Append-only migration log |
| [phase-1/foundation-spike.md](phase-1/foundation-spike.md) | Phase 1 pin, toolchain, scaffold and blockers |

## Selected architecture (summary)

**A pinned Chromium fork with a `//secant` product layer, a native Views window
shell, and a WebUI browser-chrome surface.**

- Browser process: Chromium `//chrome/browser` services and Secant services.
- Rendering and JavaScript: Chromium `//content` (Blink, V8, site isolation,
  sandbox).
- Zen experience: `//secant/webui` (HTML/CSS/JS using Zen design tokens and
  assets) hosted in a `views::WebView`; native `//secant/ui` for the window
  frame, OS menus, dialogs and platform effects.
- Product services: `SecantTabModel`, `WorkspaceService`, `SessionService`,
  `ThemeService`, `BoostService`, `ModService`, `ExtensionBridge`.
- Persistence: Chromium `Profile`/`PrefService` plus Secant JSON/SQLite stores;
  history and bookmarks use Chromium `HistoryService` and `BookmarkModel`.

Full reasoning, rejected options and limitations are in
[chromium-integration-evaluation.md](phase-0/chromium-integration-evaluation.md)
and [ADR 0001](adr/0001-chromium-foundation.md).

## Parity scores (explicit, evidence-based)

Scores are computed only from acceptance results, never from intent.

| Score | Result | Denominator | Evidence |
| --- | --- | --- | --- |
| Behavioural parity | **0.0%** (0 passing) | 182 inventoried features | Linux Chromium build exists; no Secant acceptance run |
| Visual parity | **0.0%** (0 passing) | 18 baseline cases | Linux Chromium build exists; no Secant shell or screenshots |
| Inventory coverage | **100%** (182/182) | Feature areas in scope | This Phase 0 inventory |

These are honest baselines, not failure claims about the plan. They become
non-zero only when a Secant build exists and a test or screenshot passes its
declared acceptance criterion. See [parity-report.md](phase-0/parity-report.md).

## Non-negotiable constraints

1. All work happens on `feat/chromium-engine-migration` or scoped child
   branches. Never on `dev`, `stable`, `twilight`, or `main`.
2. Never force-push, rewrite published history, or discard user changes.
3. Chromium must be the real rendering and JavaScript foundation. A Firefox
   process with a Chromium-looking shell is not acceptable.
4. Do not disable Chromium sandboxing, site isolation, certificate validation,
   permission enforcement, or safe-download protections to make a feature work.
5. No feature is "done" because its UI exists. It is done when its acceptance
   test passes.
6. Every unavoidable difference is documented with its technical reason and
   remaining work.
7. No 100% parity claim without test evidence.

## Next gate

Phase 1 exit criteria in
[foundation-spike.md](phase-1/foundation-spike.md):

1. reproducible Chromium build on Windows, macOS and Linux;
2. native window with a WebUI chrome surface;
3. basic tab lifecycle through `SecantTabModel`;
4. session restore and workspace isolation prototypes;
5. measured performance, accessibility and visual baselines.

The Linux foundation build is green in CI. The immediate work is Windows and
macOS build coverage plus the remaining Phase 1 evidence: native shell, tab
adapter, session/workspace prototypes and measured baselines.
