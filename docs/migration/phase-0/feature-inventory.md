<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Feature inventory

**Snapshot:** 2026-11-16
**Canonical data:** [feature-inventory.csv](feature-inventory.csv)
**Scope:** Every user-visible behavior, product service and platform
requirement that Secant must either preserve or explicitly replace.

This is an inventory, not a port plan. No row is marked as passing because no
Secant build exists. The behavioral parity denominator is **182 features**.

## Method

The inventory was derived from:

- `src/zen/` modules, actors, native components, styles and tests;
- `src/**/*.patch` (283 Firefox patches);
- `prefs/zen/*.yaml` (193 Zen preferences in 28 files);
- `src/zen/tests/` (232 Zen test files in 32 suites);
- imported upstream Firefox tests (529 files);
- build, packaging, migration and updater configuration.

Each row records what Zen does, which Firefox/Gecko surfaces it uses, the
candidate Chromium equivalent, current status, reference tests, Secant test
coverage and known differences.

## Schema

| Column | Meaning |
| --- | --- |
| `id` | Stable identifier. Never renumber. New work gets a new ID. |
| `area` | `Core`, `Browser`, `Zen UI`, `Platform` or `Security`. |
| `feature` | User-visible behavior or required product service. |
| `zen_implementation` | Current Zen modules, patches or native code. |
| `firefox_dependencies` | Firefox/Gecko APIs, data stores and subsystems used. |
| `chromium_equivalent` | Candidate Chromium service or an explicit gap. |
| `status` | Inventory state, not implementation state. See below. |
| `reference_tests` | Existing Zen or upstream tests that describe behavior. |
| `secant_test_coverage` | Secant test evidence. All rows are `none` today. |
| `known_differences` | Known technical mismatch or `Not yet assessed`. |

## Status definitions

| Status | Meaning |
| --- | --- |
| `inventoried` | Behavior, dependencies and a candidate mapping are recorded. No port exists. |
| `port-pending` | A Chromium equivalent exists or is plausible, but no implementation or acceptance run exists. |
| `blocked` | No acceptable equivalent or policy decision exists yet. A spike or product decision is required. |
| `asset-ready` | Assets, tokens or localization data can likely be reused, subject to licensing and format conversion. |
| `passing` | Reserved. Requires a passing Secant acceptance test and evidence link. |

`inventoried` does **not** mean implemented. `asset-ready` does **not** mean
approved for redistribution.

## Summary

| Area | Total | Inventoried | Port pending | Blocked | Asset ready |
| --- | ---: | ---: | ---: | ---: | ---: |
| Core | 40 | 36 | 2 | 1 | 1 |
| Browser | 30 | 20 | 4 | 6 | 0 |
| Zen UI | 80 | 19 | 50 | 5 | 6 |
| Platform | 20 | 3 | 17 | 0 | 0 |
| Security | 12 | 10 | 1 | 1 | 0 |
| **Total** | **182** | **88** | **74** | **13** | **7** |

No feature is counted as behaviorally passing. Behavioral parity is
**0.0% (0/182)**. Inventory coverage is **100% (182/182)**.

## Blocked features

These require a decision, a spike or a product trade-off before implementation:

| ID | Feature | Why blocked |
| --- | --- | --- |
| `CORE-034` | Media playback / codecs / DRM | Codec and Widevine redistribution/licensing is unresolved. |
| `BRW-009` | Extension compatibility | Firefox WebExtension APIs have no complete Chromium shim. |
| `BRW-012` | Sync | Firefox Sync cannot be reused; a Secant sync backend is required. |
| `BRW-014` | Extension store and policy | Store terms, signing and review policy are undecided. |
| `BRW-017` | Cookie and storage partitioning | Chromium has no lightweight contextual-identity equivalent. |
| `BRW-025` | Spaces sync service | Depends on the sync backend and workspace identity model. |
| `BRW-028` | Policy for unsigned add-ons | Chromium signing policy differs from Zen's configuration. |
| `ZEN-014` | Workspace containers and identity | Core isolation model is unresolved. |
| `ZEN-039` | Zen Mods | No equivalent for arbitrary browser-chrome CSS/JS mods. |
| `ZEN-040` | Boosts | Per-site style computation and injection differ materially. |
| `ZEN-056` | Spaces sync UI | Depends on the sync backend and workspace model. |
| `ZEN-063` | Workspace default profile/container | Depends on the isolation decision. |
| `SEC-012` | Container isolation | No direct equivalent; security and parity risk. |

## Highest-risk areas

1. **Workspace and container isolation.** Zen routes tabs through Firefox
   contextual identities and per-space profiles. Chromium storage partitions,
   profiles and `OffTheRecord` contexts are not a drop-in replacement. This
   affects cookies, storage, permissions, history, bookmarks, private spaces,
   session restore and the meaning of a workspace.
2. **Extension compatibility.** Zen relies on Firefox WebExtensions and
   Firefox-specific APIs. Chromium extension APIs, manifest versions and
   signing rules differ. A compatibility layer can cover common cases, not all.
3. **Zen Mods and Boosts.** Mods can alter browser-chrome CSS/JS; Boosts perform
   per-site style manipulation. Chromium's WebUI and Blink style pipeline do not
   expose equivalent extension points without new privileged APIs.
4. **Sync and data migration.** Firefox Sync, `places.sqlite`, `key4.db`,
   session JSON and container metadata all need migration or replacement.
5. **Codecs and DRM.** Proprietary codec and Widevine distribution require
   separate legal and packaging review.
6. **WebUI browser chrome.** A WebUI tab strip must meet native input,
   accessibility, IME, animation and startup-performance expectations.

## Cross-cutting dependencies

- **Tab model:** 49 files reference `gBrowser`; tabs, folders, split view,
  glance, pinned tabs, essentials and workspaces all depend on it.
- **Session state:** 25 files reference `SessionStore`; session schema is a
  prerequisite for restore, sync and migration.
- **Places:** history, bookmarks, favicons and workspace bookmarks depend on
  `PlacesUtils` in 13 files.
- **XUL and XBL:** 28 files use `MozXULElement`; 22 use `createXULElement`.
  These surfaces must be recreated in WebUI or native Views.
- **Preferences:** 238 `Services.prefs` references across 50 files; 193 Zen
  prefs must be mapped to Chromium `PrefService` or Secant stores.
- **Native components:** 10 Zen XPCOM components and Boost C++ units depend on
  Gecko internals and cannot be ported unchanged.

## Known differences policy

`known_differences` records a known technical mismatch at inventory time. It is
not an accepted parity exception. Every non-empty difference must later have:

1. a technical reason;
2. a user-visible impact statement;
3. a mitigation or a documented decision to accept the difference;
4. a test that verifies the chosen behavior.

Rows marked `Not yet assessed` must be assessed before their Phase 1 or Phase 2
exit gate.

## Change control

- The CSV is canonical; this document is a summary and must be regenerated when
  counts change.
- Add features with new IDs; do not renumber existing IDs.
- Change `status` only with evidence: a design, spike result, implementation or
  test.
- `passing` requires a Secant test ID and a recorded result in
  [parity-report.md](parity-report.md).
- A feature removed from scope must be marked `not-reproducible` with a product
  decision, not silently deleted.
