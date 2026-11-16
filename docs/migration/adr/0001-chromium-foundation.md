<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# ADR 0001 — Chromium foundation for Secant Browser

- **Status:** Proposed
- **Date:** 2026-11-16
- **Decision:** Adopt a pinned Chromium fork with a `//secant` product layer, a
  native Views window shell and a WebUI browser-chrome surface.
- **Supersedes:** None
- **Related:** [Architecture analysis](../phase-0/architecture-analysis.md),
  [Chromium integration evaluation](../phase-0/chromium-integration-evaluation.md),
  [Feature inventory](../phase-0/feature-inventory.csv)

## Context

Zen Browser is a product overlay on Mozilla Firefox. This repository contains
Zen's product code, 283 Firefox patches, preferences, packaging and tests, but
not the Firefox engine. `surfer` downloads Firefox, applies the patches, imports
`src/zen` and builds with `mach`.

Secant must move to Chromium/Blink/V8 while preserving as much Zen behavior and
visual design as possible. The existing overlay cannot be moved by changing
imports: it depends on Firefox's XUL chrome, `gBrowser`, `SessionStore`,
`PlacesUtils`, XPCOM, contextual identities, WebExtensions and Gecko layout.
The coupling is structural.

The migration also must produce a real desktop browser, not an embedded web
view. It must support Windows, macOS and Linux, preserve Chromium's security
model and remain maintainable across upstream releases.

## Decision

1. **Foundation:** use a pinned Chromium release branch as the browser
   foundation. Blink and V8 render and execute all web content. Chromium's
   sandbox, site isolation, network service, certificate verification and
   permission model remain enabled.
2. **Product layer:** add a `//secant` layer that owns the browser model,
   workspaces, tabs, sessions, themes, boosts, mods, settings, migration and
   platform integration.
3. **Window shell:** use native Views for the top-level window, native menus,
   dialogs, platform effects and OS integration.
4. **Browser chrome:** implement the vertical tab strip, sidebar, workspace
   switcher, library, panels, settings and other dense product UI as privileged
   WebUI hosted in a `views::WebView`.
5. **Service reuse:** reuse Chromium browser services (`Profile`,
   `HistoryService`, `BookmarkModel`, `DownloadManager`, `PermissionManager`,
   `SessionService`, `ExtensionSystem`, DevTools, Safe Browsing) rather than
   rebuilding them.
6. **Patch policy:** keep changes to `//chrome` minimal and tracked in
   `patches/chromium/`. Product code lives in `//secant`. Firefox patches are
   behavior references and are never applied to Chromium.
7. **Repository model:** keep the Chromium source uncommitted. A sync tool
   checks out the pinned source, applies the reviewed Chromium patch set, copies
   `secant/` into `src/secant`, and builds with GN/Ninja.

## Consequences

### Positive

- Secant is a real Chromium browser with the full browser service stack.
- Zen's visual language can be recreated with WebUI instead of stock Views
  controls.
- Product code has a clear ownership boundary and a smaller patch surface.
- Security protections remain those of Chromium, subject to review.
- The product repository remains reviewable without vendoring Chromium.

### Negative

- A Chromium fork requires continuous rebasing and release engineering.
- WebUI browser chrome must prove input latency, focus, IME, accessibility,
  startup and memory budgets.
- `//chrome/browser` is not a clean library; some integration points require
  patches or subclassing.
- Workspace/container isolation, extension compatibility, mods/boosts, sync,
  codecs/DRM and data migration remain hard product problems.
- The team must maintain GN/Ninja, Chromium test infrastructure and platform
  packaging expertise.

### Neutral

- Some Zen code is reusable (tokens, assets, localization, CSS, behavior
  patterns), but no Firefox patch or XPCOM component is portable unchanged.
- The migration is multi-release and gated by evidence, not a single rewrite.

## Alternatives considered

| Alternative | Outcome |
| --- | --- |
| Chromium fork + native Views chrome | Viable fallback for surfaces where WebUI fails performance or accessibility. Lower visual parity and slower UI iteration. |
| Chromium `//content` + custom shell | Rejected: rebuilds browser services, extensions and platform integration. |
| CEF | Rejected: embedding framework, not a full browser; release cadence and service gaps. |
| Electron | Rejected: application runtime, not a browser; security and service gaps. |
| Qt WebEngine | Rejected: embedded web view; extension, service and licensing constraints. |
| WebView2 | Rejected: Windows-only and runtime-dependent. |
| Firefox/Gecko with a Chromium-looking shell | Rejected: does not migrate the engine. |

## Compliance and constraints

- Do not disable Chromium sandboxing, site isolation, certificate validation,
  permission enforcement or Safe Browsing.
- Do not port `external-patches/firefox/override_cert_checks_temp.patch`.
- WebUI must use a dedicated privileged scheme, strict CSP, no remote scripts
  and a narrow Mojo API. It must not expose raw profile, filesystem or extension
  privileges to web content.
- Workspace isolation must be implemented without weakening site isolation.
- Any security-relevant Chromium patch needs a named reviewer and threat-model
  note.
- No parity claim without a passing acceptance test or visual comparison.

## Validation

This decision is accepted only after the Phase 1 spike proves:

1. reproducible Chromium builds on Windows, macOS and Linux;
2. a native Secant window with a WebUI vertical tab strip and sidebar;
3. basic tabs, navigation and session restore through `SecantTabModel`;
4. a workspace isolation prototype accepted by security review;
5. measured startup, input latency, memory and accessibility results.

If WebUI chrome fails the performance or accessibility gate, the affected
surfaces fall back to native Views rather than weakening requirements.

## Review triggers

Revisit this ADR if:

- Chromium's release or embedding model changes materially;
- the WebUI chrome spike fails its budgets;
- workspace isolation cannot be implemented securely;
- extension compatibility proves unacceptable to the product;
- codec/DRM or store licensing blocks distribution;
- the maintenance cost of the fork exceeds the approved budget.
