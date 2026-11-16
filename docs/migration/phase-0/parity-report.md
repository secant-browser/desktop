<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Parity report

**Report date:** 2026-11-16
**Build:** None
**Status:** Phase 0 baseline

## 1. Current scores

| Score | Result | Denominator | Evidence |
| --- | --- | --- | --- |
| Behavioral parity | **0.0% (0 passing)** | 182 features | No Secant build; no acceptance run |
| Visual parity | **0.0% (0 passing)** | 18 baseline cases | No Secant build; no screenshots |
| Inventory coverage | **100.0% (182/182)** | Features in scope | [feature-inventory.csv](feature-inventory.csv) |
| Secant test coverage | **0 tests** | Acceptance suite | No harness or build exists |

These are honest baselines. They do not measure the quality of the plan. They
measure the absence of a Secant build and evidence.

## 2. Behavioral parity by area

| Area | Features | Passing | Score |
| --- | ---: | ---: | ---: |
| Core | 40 | 0 | 0.0% |
| Browser | 30 | 0 | 0.0% |
| Zen UI | 80 | 0 | 0.0% |
| Platform | 20 | 0 | 0.0% |
| Security | 12 | 0 | 0.0% |
| **Total** | **182** | **0** | **0.0%** |

## 3. Inventory status

These are inventory states, not test results.

| Status | Count | Meaning |
| --- | ---: | --- |
| `inventoried` | 88 | Behavior and mapping recorded; not implemented |
| `port-pending` | 74 | Chromium equivalent identified or plausible; no implementation |
| `blocked` | 13 | Requires a decision, spike or product trade-off |
| `asset-ready` | 7 | Assets likely reusable, subject to licensing and conversion |
| `passing` | 0 | Requires a passing Secant acceptance test |
| **Total** | **182** | |

Blocked features are listed in [feature-inventory.md](feature-inventory.md).
They are not counted as passing, and they are not silently removed from the
denominator.

## 4. Visual parity by case

| Case | Surface | Result |
| --- | --- | --- |
| V-01 | Default light | Not captured |
| V-02 | Default dark | Not captured |
| V-03 | System theme | Not captured |
| V-04 | Vertical tabs | Not captured |
| V-05 | Collapsed sidebar | Not captured |
| V-06 | Compact mode | Not captured |
| V-07 | Workspace switcher | Not captured |
| V-08 | Workspace theme | Not captured |
| V-09 | Pinned tabs and essentials | Not captured |
| V-10 | Tab folders | Not captured |
| V-11 | Split view | Not captured |
| V-12 | Glance | Not captured |
| V-13 | URL bar | Not captured |
| V-14 | Site identity and data panel | Not captured |
| V-15 | Library | Not captured |
| V-16 | Settings | Not captured |
| V-17 | Mods and Boosts | Not captured |
| V-18 | Welcome/onboarding | Not captured |
| **Total** | | **0/18** |

See [visual-baseline-plan.md](visual-baseline-plan.md) for variants, tolerances
and capture rules.

## 5. What counts as passing

### Behavioral

A feature passes only when:

1. it has an inventory ID;
2. a Secant acceptance test exists;
3. the test passes on the declared platform(s);
4. the result records the build commit, test ID, date and evidence link;
5. known differences are documented and approved.

A screenshot, a manual impression or a merged pull request is not behavioral
evidence.

### Visual

A case passes only when:

1. the reference and Secant screenshots are captured with the same fixture,
   viewport, theme, DPI and font set;
2. all required variants pass;
3. the comparison is within the declared tolerance;
4. design and QA approve the result;
5. no P0/P1 visual defect is open for that surface.

A behavioral test is not visual evidence.

## 6. Score history

| Date | Build | Behavioral | Visual | Inventory | Notes |
| --- | --- | --- | --- | --- | --- |
| 2026-11-16 | None | 0/182 | 0/18 | 182/182 | Phase 0 baseline |

Future entries must include the commit, platform matrix and a link to evidence.
Do not overwrite earlier rows.

## 7. Blockers to the first non-zero score

1. A reproducible Chromium build.
2. A Secant browser window with at least one navigable tab.
3. A test harness that can run behavioral assertions against the build.
4. A capture harness that can produce deterministic screenshots.
5. A pinned reference build for visual comparison.
6. Priority classification (P0/P1/P2) for the 182 features.

Until these exist, any non-zero score would be fabricated.

## 8. Priority classification

The current denominator weights all 182 features equally because priority has
not been assigned. Phase 1 must classify each feature:

| Priority | Meaning |
| --- | --- |
| P0 | Browser cannot ship without it (navigation, tabs, security, data integrity) |
| P1 | Core product promise (workspaces, vertical tabs, essentials, sessions) |
| P2 | Important but deferrable (secondary integrations, polish) |
| P3 | Optional or explicitly out of scope |

The release gate uses P0/P1 for mandatory pass rates and the full inventory for
the overall score. Until classification is complete, no P0/P1 claim is valid.

## 9. Known differences

No difference is approved yet. The inventory records 21 rows explicitly marked
`Not yet assessed`; the remaining rows have an initial technical note. An
initial note is not an approved exception.

Every approved difference must record:

- the feature ID;
- the technical reason;
- user-visible impact;
- mitigation or accepted limitation;
- test that verifies the chosen behavior;
- approver and date.

## 10. Reporting cadence

- **Per phase gate:** full behavioral and visual report.
- **Per release candidate:** full report plus platform matrix.
- **Nightly:** dashboard from CI, not a manually edited score.
- **On regression:** a new entry in the history; never erase the prior result.

Scores are computed from test results, not from a sum of self-reported statuses.
Inventory status and parity score are related but not interchangeable.

## 11. Integrity rules

- Never change the denominator to improve the score.
- Never mark a feature passing without a test ID and evidence.
- Never average behavioral and visual scores into one number.
- Never compare screenshots across operating systems.
- Never widen a visual tolerance to hide a regression.
- Never count an unimplemented feature because its UI shell exists.
- Never claim "100% parity"; the target is maximum verified parity with
  documented differences.

## 12. Release targets

These are targets, not current results:

| Gate | Target |
| --- | --- |
| P0/P1 behavior | 100% pass |
| Overall behavior | ≥95% pass; every exception approved |
| P0/P1 visual | 100% within tolerance |
| Overall visual | ≥95% within tolerance |
| Security | No open critical/high findings |
| Accessibility | No critical blockers |
| Data integrity | No silent loss in migration/restore fixtures |
