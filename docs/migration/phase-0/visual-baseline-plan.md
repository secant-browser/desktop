<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Visual baseline plan

**Status:** Phase 0 proposal
**Current visual parity:** **0.0% (0/18)** — no Secant build and no captured
baseline exist.
**Canonical result:** [parity-report.md](parity-report.md)

## 1. Purpose

Visual parity is a separate gate from behavioral parity. A feature can work and
still look wrong. This plan defines:

- the 18 baseline cases;
- the states and platforms captured;
- deterministic capture rules;
- tolerances and approval rules;
- how visual parity is scored and reported.

The baseline is **browser chrome**, not arbitrary web content. Content is a
controlled fixture so that engine rendering differences do not masquerade as
product regressions.

## 2. Reference set

The reference is a pinned Zen build on each supported platform, captured with
the same viewport, theme, font set and fixture content as the Secant build.

Until that reference set is captured:

- visual parity remains 0/18;
- no screenshot comparison may be reported as passing;
- no baseline may be replaced to make a failing comparison pass.

## 3. Baseline cases

Each case passes only if **all required variants** for that case pass. The
denominator is 18 cases, not the number of screenshots.

| ID | Surface | Required state | Variants | Pass condition |
| --- | --- | --- | --- | --- |
| V-01 | Default window | One new tab, sidebar expanded, light theme | 1x, 2x; focused and unfocused window | Layout, spacing, colors and typography match |
| V-02 | Default window | One new tab, sidebar expanded, dark theme | 1x, 2x; focused and unfocused | Same, with dark tokens |
| V-03 | System theme | Theme follows OS light/dark | Light-to-dark transition; reduced motion | Correct theme applied without flash or stale colors |
| V-04 | Vertical tabs | 10 tabs with mixed titles, favicons and loading states | Active, hover, unread, muted, dragging | Row geometry, truncation, indicators and drag state match |
| V-05 | Collapsed sidebar | Sidebar collapsed, tab strip icons visible | Hover reveal; pinned tab active | Collapse/reveal geometry and icon alignment match |
| V-06 | Compact mode | Sidebar hidden, edge-hover reveal | Mouse enter/leave; keyboard focus | Reveal behavior and layout shift match |
| V-07 | Workspace switcher | Switcher open with four workspaces | Light/dark; active/hover; overflow | Panel placement, gradient thumbnails and labels match |
| V-08 | Workspace theme | Non-default workspace with gradient and icon | Light/dark; selected/unselected | Gradient, accent and icon composition match |
| V-09 | Pinned tabs and essentials | Pinned tabs plus essentials strip | Loaded, unloaded, unread, hover | Icon size, spacing, unread dot and tooltip anchor match |
| V-10 | Tab folders | Nested folders with tabs | Expanded, collapsed, drag-over, empty | Indentation, disclosure, counts and drop target match |
| V-11 | Split view | Two tabs side by side with divider | Horizontal/vertical if supported; focused pane | Pane sizing, divider, focus ring and controls match |
| V-12 | Glance | Glance overlay over a page | Open, loading, error, close | Overlay position, scrim, animation end state match |
| V-13 | URL bar | Focused omnibox with results | Empty, typed query, search mode, overflow | Result list geometry, icons, highlight and focus match |
| V-14 | Site identity and data panel | Identity popup with permissions and site data | Secure/insecure; granted/blocked | Panel layout, icon states and permission rows match |
| V-15 | Library sidebar | History/downloads/media sections | Empty, populated, loading, error | Section layout, rows, empty states and scrollbars match |
| V-16 | Settings | Preferences/settings home and a detail page | Light/dark; narrow window | Navigation, controls, spacing and text match |
| V-17 | Mods and Boosts | Mods list and Boost editor | Installed/none; enabled/disabled; editor open | Cards, toggles, editor layout and preview match |
| V-18 | Welcome/onboarding | First-run welcome | Light/dark; reduced motion; small window | Layout, video poster, copy and CTA placement match |

## 4. Capture matrix

| Dimension | Values |
| --- | --- |
| Platform | Windows, macOS, Linux (separate baselines) |
| Theme | Light, dark, system |
| DPI | 1x and 2x required; 1.25x and 1.5x smoke |
| Window size | 1440×900 reference; 1024×700 minimum; 1920×1080 smoke |
| Input state | Pointer, keyboard focus, active, hover, drag |
| Motion | Normal and `prefers-reduced-motion: reduce` |
| Locale | `en-US` reference; one RTL locale and one long-string locale smoke |
| Accessibility | Default, high contrast, 200% text zoom |

Cross-platform pixel comparison is invalid. Each platform has its own approved
baseline because font rasterization, native controls and window decorations
differ.

## 5. Determinism rules

1. Use a fixed fixture profile with no extensions except test fixtures.
2. Disable telemetry, updates, sync and network access during capture.
3. Use local fixture pages with fixed images, fonts and content.
4. Pin the font set and disable font fallback where possible.
5. Disable animations for static captures; capture motion separately.
6. Wait for `document.fonts.ready`, images, layout and two animation frames.
7. Hide the caret and disable blinking cursors.
8. Use a fixed timezone, locale and device scale factor.
9. Capture the whole window and the chrome-only region.
10. Record the Chromium commit, Secant commit, OS version, GPU and font list.

## 6. Tooling

| Surface | Capture method |
| --- | --- |
| WebUI chrome | Playwright/CDP screenshot of the `secant://` surface |
| Native window | Platform screenshot of the real window, plus compositor capture where available |
| Animations | Screen recording at a fixed frame rate |
| Accessibility | Platform accessibility tree snapshot plus screenshot |
| Diff | Pixel diff, layout bounds diff and color diff |

The visual harness must run in CI and produce artifacts even when the comparison
fails.

## 7. Tolerances

| Dimension | Tolerance |
| --- | --- |
| Layout bounds | No unintended movement greater than 1 CSS px at reference DPI |
| Flat color | ΔE2000 ≤ 2.0 |
| Gradients | ΔE2000 ≤ 3.0 at sampled stops |
| Text | No clipping or overflow; line breaks match; baseline within 1 CSS px |
| Icons | Correct asset, size and alignment; no missing glyphs |
| Static pixel diff | ≤ 0.5% of chrome pixels, excluding declared dynamic regions |
| Motion | Duration within ±10%; no dropped frames in the recorded run |
| Focus ring | Present, visible and not clipped in every required state |
| Z-order | No overlapping or occluded chrome elements |
| Scrollbars | Correct visibility, size and theme |

Tolerances are per case. A case with a stricter requirement overrides the
default. Dynamic regions (video, spinners, live timestamps) must be masked
explicitly and listed in the case definition.

## 8. Scoring

Visual parity is:

```text
visual parity = passing cases / 18
```

A case passes only when:

1. every required variant passes;
2. no P0/P1 visual defect is open for that surface;
3. the comparison was captured from a signed or CI build of the same commit;
4. the result is recorded in `parity-report.md`.

A screenshot that is merely "close" is not a pass. A baseline update is a
product change, not a test fix.

## 9. Approval and change control

- Baseline changes require design and QA approval.
- Every change states why the old baseline was wrong or why the product changed.
- A failing case cannot be fixed by widening tolerance without approval.
- New cases require an inventory ID or a documented product decision.
- Removed cases must be marked `not-reproducible` with a reason.
- Visual differences caused by platform fonts or native controls are documented
  per platform, not hidden by a global tolerance.

## 10. Artifacts

```text
artifacts/visual/<case-id>/<platform>/<theme>/<dpi>/<commit>/
  screenshot.png
  reference.png
  diff.png
  bounds.json
  colors.json
  metadata.json
  result.json
```

Artifacts are retained for the life of the release line. Approved reference
images are stored in the visual baseline store, not mixed with build outputs.

## 11. Phase gates

| Phase | Visual requirement |
| --- | --- |
| Phase 1 | Capture harness works; one shell case captured; no parity claim |
| Phase 2 | V-01 to V-10, V-13 and V-16 captured; shell visual review |
| Phase 3 | V-07, V-08, V-09, V-10 and V-18 captured with workspace states |
| Phase 4 | V-15 and V-17 captured; mods/boosts visual review |
| Phase 5 | All 18 cases captured on all platforms |
| Phase 6 | 100% of P0/P1 cases pass; at least 95% overall; differences approved |

## 12. Known limitations

- Native window decorations and font rasterization differ by OS and cannot be
  pixel-identical across platforms.
- Video posters, spinners and live content need masking or motion capture.
- High-DPI fractional scaling may produce subpixel differences.
- WebUI and native surfaces may composite differently; compare the final window,
  not only the WebUI DOM.
- Visual parity does not prove behavioral parity, accessibility or performance.
