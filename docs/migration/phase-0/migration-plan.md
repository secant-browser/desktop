<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Migration plan

**Status:** Phase 0 proposal, awaiting architecture review
**Integration branch:** `feat/chromium-engine-migration`
**Base:** `dev` @ `dcf5b046974098186a317d01a4bacb070955829d`

This plan is gated. No phase starts until the previous phase's exit criteria are
met and the review gate is recorded in [MIGRATION_LOG.md](../MIGRATION_LOG.md).

Durations are planning ranges, not commitments. They must be re-estimated after
the Phase 1 spike.

## 1. Principles

1. **Evidence over intent.** A feature is done only when its acceptance test or
   visual comparison passes.
2. **Security is a gate.** Sandbox, site isolation, certificate validation,
   permissions and Safe Browsing are never traded for parity.
3. **Product code in `//secant`.** Changes to `//chrome` are minimal, reviewed
   and tracked.
4. **No Firefox patches.** Zen patches are behavior references only.
5. **One migration at a time.** Do not combine engine, UI, data and packaging
   rewrites in a single change.
6. **Every difference is documented.** Unavoidable differences need a reason,
   impact, mitigation and test.
7. **No big-bang release.** Internal builds, dogfood, beta, then stable.

## 2. Phase overview

| Phase | Name | Primary outcome | Planning range |
| --- | --- | --- | --- |
| 0 | Assessment | Architecture, inventory, dependency map, plan | Complete pending review |
| 1 | Foundation spike | Reproducible Chromium build, shell prototype, tab model, isolation spike | 6–10 weeks |
| 2 | Shell, tabs and chrome | Native window + WebUI chrome + core tab experience | 4–6 months |
| 3 | Workspaces, sessions and profiles | Workspace isolation, session restore, profiles, private spaces, migration | 3–5 months |
| 4 | Data, extensions and sync | History/bookmarks/downloads/library, extensions, mods, boosts, sync | 4–6 months |
| 5 | Platform and release engineering | Native platform integration, installers, signing, updates | 3–5 months |
| 6 | Parity and hardening | Performance, accessibility, security, visual and behavioral parity, beta | 3+ months before beta |

Phases 2–5 overlap by workstream, but their exit gates are sequential.

## 3. Phase 0 — Assessment (current)

### Deliverables

- Architecture analysis and dependency map.
- 182-feature inventory and 283-patch inventory.
- Chromium integration evaluation and ADR 0001.
- Migration, visual baseline, licensing, parity and risk documents.
- Repository snapshot and migration log.

### Exit criteria

- Architecture review approves the foundation and the hybrid shell.
- Workspace isolation, extension policy and mods policy are accepted as
  **open Phase 1/3 questions**, not silently assumed.
- The Phase 1 spike scope and budgets are approved.
- The working tree is committed on the migration branch.

## 4. Phase 1 — Foundation spike

### Goal

Prove the foundation is viable before committing to a product-wide port.

### Scope

- Pin a Chromium stable release branch and record the exact commit.
- Build on Windows, macOS and Linux with GN/Ninja.
- Add `//secant/app` bootstrap and `SecantBrowserMainParts`.
- Create a native Secant window with a `views::WebView` chrome surface.
- Prototype the vertical tab strip and sidebar in WebUI with a narrow Mojo API.
- Prototype `SecantTabModel` over `TabStripModel`.
- Create, close, reorder and navigate basic tabs.
- Prototype session restore with a metadata sidecar.
- Prototype workspace isolation models; security review of the leading model.
- Add performance, accessibility and visual test harnesses.
- Add CI for sync, build, lint, unit tests and a smoke browser test.

### Exit criteria

| Criterion | Evidence |
| --- | --- |
| Clean build on all three platforms | CI logs and pinned revision |
| Browser launches and navigates | Smoke test recording |
| Tab create/close/reorder works | Browser test |
| WebUI chrome input and focus work | Measured latency and manual test |
| Session restore prototype | Fixture test |
| Workspace isolation prototype | Security review note |
| No critical accessibility blocker | Accessibility smoke report |
| Performance baseline recorded | Startup, tab switch, memory, input latency |
| Patch budget recorded | `patches/chromium/` inventory |

If the WebUI performance or accessibility gate fails, the affected surfaces
fall back to native Views. If isolation cannot be made secure, the product must
choose a documented isolation model before Phase 3.

## 5. Phase 2 — Shell, tabs and browser chrome

### Scope

- Native window frame, window controls, menus and dialogs.
- WebUI vertical tab strip, sidebar, toolbar, URL bar surface and panels.
- Tab model: create, close, switch, reorder, pin, duplicate, unload, multi-select.
- Folders, pinned tabs, essentials, split view and glance.
- Tab context menus, drag/drop, loading indicators and tab search.
- Design tokens, themes, accent colors and wallpapers.
- Core browser chrome: history/bookmark entry points, downloads entry point,
  site identity, find bar, zoom, print, full screen and PiP controls.
- Port Phase 2 acceptance tests from Zen reference tests.

### Exit criteria

- Core tab workflows pass browser tests on all platforms.
- Vertical tab strip and sidebar meet input, focus, IME and accessibility
  budgets.
- Visual baseline for the shell is captured and reviewed.
- No P0/P1 defects in tab lifecycle or navigation.
- Phase 2 features in the inventory have tests or documented gaps.

## 6. Phase 3 — Workspaces, sessions and profiles

### Scope

- Choose and implement the workspace isolation model.
- Workspace creation, editing, switching, icons, themes, gradients and routing.
- Workspace containers/identity and default profile/container behavior.
- Session restore, crash recovery, closed tabs, backups and window sync.
- Profiles, profile picker, private browsing and private workspaces.
- Little window and multi-window behavior.
- Migration from Firefox, Arc and Dia profiles.
- Workspace bookmarks and space-scoped metadata.

### Exit criteria

- Workspace isolation passes security review and data-isolation tests.
- Session restore fixtures pass without silent data loss.
- Profile and private-browsing tests pass.
- Migration fixtures cover Firefox/Arc/Dia data with documented gaps.
- Workspace-scoped permissions and data clearing are proven or explicitly
  limited.

## 7. Phase 4 — Data, extensions and sync

### Scope

- History, bookmarks, favicons and downloads on Chromium services.
- Library: history, downloads, media, spaces, boosts, search, routing.
- Live folders (GitHub/RSS) with explicit network permissions.
- Extension platform, compatibility shims and a compatibility matrix.
- Extension store/signing policy.
- Zen Mods package system and theme/mod management.
- Boosts: per-site styling, editor, sharing and persistence.
- Preferences and settings WebUI; 193 Zen prefs mapped.
- Autofill, passwords, search engines and site data.
- Sync decision: Chromium Sync, a Secant sync backend or documented removal.
- Spaces sync and device sync if approved.

### Exit criteria

- Data services pass import/export and migration tests.
- Extension compatibility matrix is published with pass/fail per API.
- Mods and boosts pass functional and visual acceptance tests.
- Settings parity review is complete; unsupported prefs are documented.
- Sync scope is approved and tested, or explicitly deferred with product sign-off.

## 8. Phase 5 — Platform and release engineering

### Scope

- Windows, macOS and Linux native integration.
- Installers: Windows, macOS DMG, Linux AppImage/Flatpak packages.
- Signing and notarization.
- Update service and update UI.
- Global shortcuts, share, haptics, notifications and file associations.
- Multi-monitor, DPI, touchpad, gestures and Wayland behavior.
- Crash reporting and telemetry policy.
- Platform accessibility validation.
- Reproducible builds and release automation.

### Exit criteria

- Signed builds install, launch and update on each supported platform.
- Platform integration tests pass on the supported matrix.
- Update rollback and failure paths are tested.
- No open critical/high security findings.
- Release checklist and rollback plan are approved.

## 9. Phase 6 — Parity and hardening

### Scope

- Port remaining acceptance tests and close parity gaps.
- Performance: startup, tab switch, input latency, memory and rendering.
- Accessibility: keyboard, screen reader, focus and high contrast.
- Security: sandbox, site isolation, permissions, certificate handling and
  extension permissions.
- Visual regression across the baseline matrix.
- Documentation, onboarding and support tooling.
- Internal dogfood, beta and stable release gates.

### Release gates

| Gate | Requirement |
| --- | --- |
| P0/P1 behavior | 100% of P0/P1 acceptance tests pass |
| Overall behavior | At least 95% of inventoried features pass; every exception approved |
| Visual | 100% of P0/P1 visual cases within tolerance; at least 95% overall |
| Security | No open critical/high findings; isolation and permission tests pass |
| Accessibility | No critical blockers; keyboard-only core flows pass |
| Performance | No more than 20% regression vs pinned Chromium baseline on approved metrics |
| Data | Migration and restore fixtures pass without silent loss |
| Documentation | Every known difference has a reason, impact, mitigation and test |

These are release targets, not current claims. The current parity score remains
0.0% until a Secant build exists and tests pass.

## 10. Workstreams

| ID | Workstream | Responsibilities |
| --- | --- | --- |
| W1 | Engine, fork and build | Chromium pin, sync tool, GN/Ninja, patch budget, rebase cadence |
| W2 | Browser model and tabs | `SecantBrowser`, `SecantTabModel`, tab lifecycle, folders, split, glance |
| W3 | WebUI chrome and design system | Vertical tabs, sidebar, panels, library, settings, tokens, CSS |
| W4 | Native shell and platform | Views window, menus, dialogs, platform effects, drag/drop |
| W5 | Workspaces and isolation | Workspace service, routing, containers, permissions, data isolation |
| W6 | Sessions, profiles and migration | Session sidecar, restore, profiles, private mode, importers |
| W7 | Data services | History, bookmarks, downloads, favicons, library, live folders |
| W8 | Extensions, mods and boosts | Extension platform, shims, mods, boosts, store policy |
| W9 | Preferences and settings | Pref mapping, settings WebUI, defaults, migration |
| W10 | Security and privacy | Threat models, isolation review, permission model, security tests |
| W11 | QA, parity and visual | Acceptance tests, visual baseline, performance, accessibility |
| W12 | Release engineering | Installers, signing, updates, CI, reproducible builds |
| W13 | Documentation and developer experience | Build docs, contribution guide, migration log, release notes |

Workstreams can overlap. Each has a named owner before Phase 1 starts.

## 11. Branch and commit policy

- Integration branch: `feat/chromium-engine-migration`.
- Child branches: `migration/<phase>-<workstream>-<slug>`.
- Never commit directly to `dev`, `stable`, `twilight` or `main`.
- Never force-push or rewrite published history.
- Commit in logical increments; keep generated Chromium source out of git.
- Do not commit secrets, signing keys, API keys or profile data.
- Every PR states: feature IDs affected, tests run, parity impact, security
  impact and rollback plan.
- Documentation changes and code changes may be separate commits.
- Record phase gates and notable decisions in `MIGRATION_LOG.md`.

## 12. CI and quality gates

### Required checks

- Patch application and Chromium sync reproducibility.
- Build matrix: Windows, macOS, Linux.
- `unit_tests`, `browser_tests`, WebUI Playwright tests.
- Lint: clang-format, ESLint, Stylelint, presubmit.
- License and third-party notices check.
- Patch budget check: no unreviewed `//chrome` patch.
- Security checks: dependency audit, secret scan, sandbox/site-isolation smoke.
- Visual regression on the baseline matrix.
- Performance smoke on reference hardware.
- Migration fixtures.

### Nightly

- Sync the pinned Chromium branch and report upstream drift.
- Rebase the integration branch in a scratch worktree.
- Run the full parity and visual suites.
- Publish a health report.

## 13. Test strategy

| Layer | Tooling | Purpose |
| --- | --- | --- |
| Unit | Chromium gtest | Services, tab model, session metadata, prefs |
| Browser integration | Chromium `browser_tests` | Tabs, navigation, profiles, permissions, downloads |
| WebUI | Playwright + Chromium harness | Vertical tabs, sidebar, settings, panels |
| Visual | Screenshot comparison | 18-case baseline matrix and component states |
| Performance | Startup/input/memory harness | Regression against pinned baseline |
| Accessibility | AT-SPI/IA2/UIA + axe-style checks | Keyboard, screen reader, focus, contrast |
| Security | Sandbox/site-isolation/permission tests | No weakened protections |
| Migration | Profile/session fixtures | Firefox/Arc/Dia import and restore |
| Manual exploratory | Dogfood builds | Real-world workflows and platform quirks |

Zen's 232 test files and 529 imported Firefox tests are **reference
specifications**. Each relevant assertion must be ported to a Secant test; they
cannot run against Chromium directly.

## 14. Parity method

- Behavioral parity is computed from feature acceptance results in
  [parity-report.md](parity-report.md), using the 182-row inventory.
- Visual parity is computed from the 18-case matrix in
  [visual-baseline-plan.md](visual-baseline-plan.md).
- Inventory coverage tracks whether every feature has an owner, mapping and test
  plan.
- A status changes only with evidence. No self-reported completion.
- Differences are approved explicitly and recorded in the risk register.

## 15. Roles and staffing assumptions

Minimum viable team:

- 1 architecture/technical lead;
- 1 Chromium build/release engineer;
- 2 browser-model engineers;
- 2 WebUI/frontend engineers with design support;
- 1–2 platform engineers (Windows/macOS/Linux);
- 1 security engineer (part-time or shared);
- 1 QA/parity engineer;
- product/design support for workspace, mods, boosts and migration decisions.

A smaller team can run Phase 1, but Phases 2–5 require the full set or a longer
schedule. This assumption must be validated before Phase 1.

## 16. Dependencies and decisions

| Dependency | Needed by | Owner |
| --- | --- | --- |
| Chromium milestone pin | Phase 1 | W1 |
| Workspace isolation model | Phase 3 | W5 + W10 |
| Extension compatibility policy | Phase 4 | Product + W8 |
| Mods/boosts policy | Phase 4 | Product + W8 |
| Sync scope | Phase 4 | Product + W7 |
| Codec/DRM distribution | Phase 5 | Legal + W12 |
| Store/signing policy | Phase 4/5 | Product + W12 |
| Update infrastructure | Phase 5 | W12 |
| Reference hardware for performance | Phase 1 | W11 |
| Visual baseline assets and licenses | Phase 1 | Design + legal |

## 17. Definition of done

A feature is done when:

1. it has an inventory ID and an owner;
2. its Chromium mapping is documented;
3. its acceptance test exists and passes;
4. its visual state is covered if user-visible;
5. its security and privacy impact is reviewed;
6. its known differences are documented and approved;
7. it is included in a signed build on the supported platform matrix;
8. its docs and release note are updated.

## 18. Immediate next actions after approval

1. Pin the Chromium milestone and record the commit.
2. Stand up the sync/build skeleton and CI.
3. Create `//secant/app`, `//secant/browser` and `//secant/webui` stubs.
4. Build the native window and WebUI tab-strip prototype.
5. Prototype `SecantTabModel` and basic navigation.
6. Run the workspace isolation spike and security review.
7. Capture the first performance and accessibility baselines.
8. Record the Phase 1 gate in `MIGRATION_LOG.md`.
