<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Chromium integration evaluation

**Status:** Phase 0 proposal, awaiting architecture review
**Decision record:** [ADR 0001](../adr/0001-chromium-foundation.md)
**Scope:** Choose the foundation and browser-shell strategy for Secant Browser.

## 1. Decision summary

Secant will use a **pinned Chromium fork with a `//secant` product layer**, a
**native Views window shell**, and a **WebUI browser-chrome surface** hosted in a
`views::WebView`.

The browser process reuses Chromium browser services where they are complete
and stable (`Profile`, navigation, permissions, history, bookmarks, downloads,
extensions, DevTools, Safe Browsing). Secant owns the product-specific browser
model and UI: workspaces, vertical tabs, folders, essentials, split view,
glance, boosts, mods, Zen-style settings and session metadata.

This is the only option that satisfies all four hard requirements:

1. Chromium/Blink/V8 is the real rendering and JavaScript engine.
2. Secant remains a full desktop browser, not an embedded web view.
3. The Zen experience can be recreated with high visual and behavioral fidelity.
4. Chromium security boundaries are preserved rather than replaced by a custom
   security model.

## 2. Requirements

### 2.1 Hard requirements

| ID | Requirement |
| --- | --- |
| R1 | Blink and V8 render and execute all web content. |
| R2 | Full browser services: profiles, navigation, history, bookmarks, downloads, permissions, extensions, DevTools, updates. |
| R3 | Windows, macOS and Linux desktop support with native window behavior. |
| R4 | Sandbox, site isolation, certificate validation and permission enforcement remain enabled. |
| R5 | Zen's visual language and core interactions can be reproduced. |
| R6 | User data can be migrated without silent loss. |
| R7 | The product can be maintained against upstream Chromium releases. |

### 2.2 Product requirements

- Vertical tabs, sidebar, workspaces, folders, essentials, split view, glance,
  pinned tabs, compact mode, boosts, mods, library, site data panel, share,
  migration, sync and keyboard shortcuts.
- A credible extension story, even if Firefox-only extension APIs cannot be
  preserved.
- Packaging, signing and updates for all supported platforms.
- No dependency on a proprietary runtime that excludes a platform.

### 2.3 Non-goals for Phase 0

- Selecting a Chromium milestone.
- Implementing any browser feature.
- Finalizing workspace isolation, extension compatibility, codecs/DRM or sync.
- Claiming parity without a build and acceptance results.

## 3. Options considered

| Option | Description |
| --- | --- |
| **A. Chromium fork + `//secant` + hybrid Views/WebUI** | Fork a pinned Chromium release branch; add a product layer; native window shell; WebUI browser chrome. |
| **B. Chromium fork + native Views chrome** | Same foundation, but all chrome is native Views; no WebUI browser-chrome surface. |
| **C. Chromium `//content` + custom browser shell** | Embed Chromium content only; build every browser service and shell ourselves. |
| **D. Chromium Embedded Framework (CEF)** | Embed Chromium through CEF; build the browser around the CEF API. |
| **E. Electron** | Chromium content plus Node.js in an application shell. |
| **F. Qt WebEngine** | Chromium content behind Qt WebEngine; Qt application shell. |
| **G. WebView2** | Microsoft Edge WebView2 runtime hosted in a Windows application. |
| **H. Firefox/Gecko with a Chromium-looking shell** | Keep Gecko and imitate Chromium UI. |

Option H is listed only to reject it explicitly. It does not migrate the engine
and therefore fails R1 by definition.

## 4. Scoring

Scores are 1 (poor) to 5 (excellent). Weighted score is out of 100.

| Criterion | Weight | Why it matters |
| --- | ---: | --- |
| Engine/runtime correctness | 15 | Blink/V8 must be the real foundation. |
| Browser service completeness | 15 | A browser needs profiles, navigation, history, downloads, permissions, extensions, DevTools. |
| Zen behavioral parity feasibility | 20 | The product differentiator must survive. |
| Visual parity feasibility | 10 | Zen's identity is visual and spatial. |
| Extension compatibility | 10 | Users and the product depend on add-ons. |
| Platform integration | 10 | Native windows, menus, shortcuts, share, packaging. |
| Security model | 10 | Sandbox, site isolation and permissions must not be weakened. |
| Maintainability and upstream velocity | 5 | A fork must be rebased for years. |
| Licensing and distribution | 5 | Codecs, DRM, store terms and third-party licenses. |

### 4.1 Scores

| Option | Engine | Services | Behavior | Visual | Extensions | Platform | Security | Maintain | License | **Weighted** |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| **A. Fork + `//secant` + hybrid** | 5 | 5 | 4 | 5 | 3 | 4 | 5 | 4 | 5 | **89** |
| B. Fork + native Views chrome | 5 | 5 | 3 | 3 | 3 | 5 | 5 | 3 | 5 | **82** |
| C. `//content` + custom shell | 5 | 2 | 4 | 5 | 1 | 2 | 4 | 2 | 4 | **67** |
| D. CEF | 5 | 2 | 3 | 4 | 2 | 3 | 4 | 3 | 4 | **66** |
| E. Electron | 5 | 1 | 2 | 3 | 2 | 2 | 3 | 2 | 4 | **52** |
| F. Qt WebEngine | 5 | 2 | 2 | 3 | 1 | 3 | 4 | 2 | 3 | **56** |
| G. WebView2 | 5 | 1 | 2 | 3 | 2 | 1 | 3 | 2 | 3 | **49** |
| H. Firefox + Chromium shell | 1 | 5 | 1 | 1 | 4 | 4 | 3 | 3 | 4 | **53 (disqualified)** |

The scores are planning judgments, not benchmark results. They must be revisited
after the Phase 1 spike, especially for WebUI performance, workspace isolation
and extension compatibility.

## 5. Option analysis

### A. Chromium fork + `//secant` + hybrid Views/WebUI (selected)

**Why it wins**

- Reuses the complete Chromium browser stack, not just the renderer.
- Keeps Blink/V8, sandbox, site isolation, network service, certificate
  verification and permission enforcement intact.
- Gives the product layer a stable ownership boundary (`//secant`) instead of
  scattering changes through `//chrome`.
- A WebUI chrome surface is the most credible path to Zen's visual and spatial
  design, which is difficult to reproduce with stock Views controls.
- Native Views remains available for the window frame, menus, dialogs and
  platform effects where native behavior matters.

**Costs and risks**

- A Chromium fork is a long-term maintenance commitment. Every release rebase
  must be staffed.
- WebUI browser chrome is non-standard. Input latency, focus, IME,
  accessibility, startup time and memory must be proven in Phase 1.
- `//chrome/browser` is not a clean library. Some integration requires patches
  or subclassing of Chromium browser classes.
- Workspace isolation, extensions, mods/boosts and sync remain unsolved product
  problems regardless of the foundation.

**Verdict:** selected, subject to Phase 1 spike exit criteria.

### B. Chromium fork + native Views chrome

**Strengths**

- Best native input, accessibility and platform behavior.
- No WebUI startup cost or renderer for browser chrome.
- Easier to reason about security boundaries for chrome UI.

**Why not selected**

- Zen's UI is not a standard toolbar/tabstrip layout. Recreating a vertical,
  animated, workspace-aware, drag-heavy chrome in Views is a large custom UI
  project with slower iteration than CSS/HTML.
- Visual parity for themes, gradients, mods and dense sidebar layouts would be
  worse.
- It does not remove the need for a product layer or Chromium patches.

Native Views remains part of the hybrid for surfaces where it is superior.

### C. Chromium `//content` + custom browser shell

**Strengths**

- Maximum control over the product.
- No dependency on `//chrome` UI.

**Why not selected**

- `//content` is a rendering/multi-process substrate, not a browser. Profiles,
  downloads, permissions, history, bookmarks, extensions, DevTools, Safe
  Browsing, updater and platform integration must be rebuilt.
- This is effectively building a browser from scratch around Blink.
- Security review and maintenance burden are larger than a controlled fork.

### D. Chromium Embedded Framework (CEF)

**Strengths**

- Mature embedding API.
- Chromium rendering and multi-process model.
- Used by commercial desktop applications.

**Why not selected**

- CEF is an embedding framework, not a full browser. The application still owns
  most browser services and the entire UI.
- Extension support, profile model, updater and platform integration are not
  equivalent to a browser.
- CEF release cadence trails Chromium; security updates depend on CEF releases.
- Multi-platform packaging and codec/DRM distribution remain product work.

### E. Electron

**Strengths**

- Fast UI development with web technologies.
- Large ecosystem.

**Why not selected**

- Electron is an application runtime, not a browser. It does not provide a
  browser-grade extension platform, profile model, updater or site isolation
  policy out of the box.
- Shipping a browser on Electron would require rebuilding browser services and
  would carry a large memory and security surface.
- It sends the wrong product signal: Secant would be an Electron app with a
  browser UI, not a Chromium browser.

### F. Qt WebEngine

**Strengths**

- Chromium content with a Qt shell.
- Good fit for a Qt application.

**Why not selected**

- Qt WebEngine is an embedded web view with a narrower API than a browser.
- Extension support, browser services, updater and platform integration are
  incomplete for a browser product.
- Qt licensing and distribution constraints add complexity.
- It would still require a custom browser shell and product services.

### G. WebView2

**Strengths**

- Good Windows integration.
- Evergreen Edge runtime.

**Why not selected**

- Windows-only, failing the cross-platform requirement.
- Depends on the Edge runtime and its release cadence.
- Not a browser platform: profiles, extensions, updates and services must be
  built around it.
- No macOS or Linux path.

### H. Firefox/Gecko with a Chromium-looking shell (disqualified)

This keeps Gecko and imitates Chromium. It fails the central requirement that
Blink and V8 be the real engine. It is not a migration; it is a reskin.

## 6. Selected architecture

```text
Secant product repository (this repository)
├── docs/migration/                 decisions, inventories, parity evidence
├── patches/chromium/               minimal, reviewed integration patches
├── secant/                         product source, synced to Chromium src/secant
│   ├── app/                        browser main entry and process bootstrap
│   ├── browser/                    tab model, workspaces, sessions, services
│   ├── ui/views/                   BrowserFrame, native menus, dialogs
│   ├── webui/                      HTML/CSS/JS browser chrome
│   ├── platform/                   Windows, macOS, Linux integration
│   ├── theme/                      tokens, mods, boosts
│   └── resources/                  icons, fonts, localization
└── tools/                          sync, build, test and packaging scripts

Pinned Chromium checkout (generated, not committed)
├── //chrome/browser                reused services, minimal integration patches
├── //content                       Blink, V8, sandbox, site isolation, Mojo
├── //extensions                    extension platform
├── //secant                        synced product layer
└── //third_party                   Chromium dependencies
```

### 6.1 Ownership boundary

- **`//chrome/browser`**: reuse services and lifecycle. Changes are limited to
  explicit integration points and tracked in `patches/chromium/`.
- **`//secant`**: all product code. No Firefox patches are applied.
- **`//content`**: untouched engine boundary except reviewed security fixes.
- **`//extensions`**: Chromium extension platform; compatibility shims live in
  `//secant/extensions` or a dedicated component.

### 6.2 Why hybrid WebUI and Views

| Surface | Implementation | Reason |
| --- | --- | --- |
| Window frame, traffic lights, resize borders | Views / native | Native window behavior and platform integration. |
| Native menus, file dialogs, permission prompts | Views / native | OS conventions, accessibility, modality. |
| Vertical tab strip, sidebar, workspace switcher | WebUI | Dense custom layout, animation, themes, rapid iteration. |
| Folder tree, essentials, pinned tabs | WebUI | Drag/drop, nesting, inline editing. |
| Split view and glance controls | WebUI + Views composition | Custom layout over multiple `WebContents`. |
| Library, settings, site data panel | WebUI | Reuse Zen HTML/CSS/Lit patterns. |
| Boosts and mods management | WebUI + privileged service | Product-specific styling model. |
| Onboarding, about, welcome | WebUI | Content-heavy and brand-driven. |

WebUI is privileged product UI, not remote content. It must use a dedicated
scheme, a strict CSP, no remote scripts and a narrow Mojo interface. It must not
receive raw filesystem, profile or extension APIs.

### 6.3 Browser model

- `SecantBrowser` owns windows and a `SecantTabModel`.
- `SecantTabModel` wraps Chromium `TabStripModel` and adds workspace, folder,
  essential, pinned, split and glance metadata.
- One `WebContents` per tab; split view composes multiple `WebContents`.
- `SecantSessionService` wraps Chromium session restore and stores product
  metadata in a versioned sidecar.
- `WorkspaceService` owns workspace identity and routing; its isolation model is
  a Phase 3 decision.
- `ThemeService`, `BoostService` and `ModService` own product styling and mod
  packages.

### 6.4 Build and sync model

The Chromium source is **not committed** to this repository. A sync tool:

1. checks out the pinned Chromium release branch;
2. applies `patches/chromium/` in a defined order;
3. copies `secant/` into `src/secant`;
4. runs `gclient sync`, `gn gen` and `autoninja`.

This keeps the product repository reviewable and avoids storing a multi-gigabyte
engine. The fork is the pinned source plus the reviewed patch set, not a
long-lived divergent copy of every Chromium file.

## 7. Security and privacy requirements

- Keep Chromium sandboxing, site isolation, certificate validation, permission
  enforcement and Safe Browsing enabled.
- Do not port `external-patches/firefox/override_cert_checks_temp.patch`.
- WebUI must not expose privileged APIs to web content.
- Workspace isolation must not be emulated by weakening site isolation.
- Extension shims must not grant broader permissions than the underlying
  Chromium extension model.
- Any security-relevant Chromium patch requires a named reviewer and a threat
  model note.

## 8. Open questions and Phase 1 spikes

| Question | Spike | Exit criterion |
| --- | --- | --- |
| Which Chromium milestone? | Pin a stable release branch and record the commit. | Reproducible build on all three platforms. |
| Is WebUI chrome fast enough? | Build a vertical tab strip and sidebar shell. | Startup, input latency, memory and IME budgets met. |
| Can `TabStripModel` support product metadata cleanly? | Prototype workspaces, folders and split view. | No incompatible fork of the tab model. |
| Which workspace isolation model? | Compare profile, partition and metadata models. | Security review accepts the model and its limits. |
| What extension compatibility is achievable? | Port a representative extension set. | Compatibility matrix with pass/fail and policy. |
| Can data migration be lossless? | Import Firefox profile fixtures. | No silent data loss; documented gaps. |
| What is the packaging/update path? | Build installers and an update prototype. | Signed update on each platform. |
| Are codecs/DRM distributable? | Legal and packaging review. | Approved component list or documented exclusion. |

## 9. Rejected patterns

- Reapplying Firefox patches to Chromium. They target Gecko/XUL APIs and cannot
  apply.
- Building a browser on `//content` alone. It is not a browser.
- Using Electron/CEF/WebView2 as the product foundation. They fail browser
  service, extension, platform or cross-platform requirements.
- Emulating Chromium UI inside Firefox. It does not migrate the engine.
- Hiding security regressions behind feature flags. Security is a gate, not a
  trade-off.
- Claiming parity from screenshots alone. Behavioral and visual evidence are
  separate gates.

## 10. Recommendation

Approve Option A for a **Phase 1 foundation spike**, not for an immediate
product-wide port. The spike must prove:

1. a reproducible Chromium build on Windows, macOS and Linux;
2. a native Secant window with a WebUI vertical tab strip and sidebar;
3. basic tab creation, navigation and session restore through `SecantTabModel`;
4. a workspace isolation prototype with a security review;
5. measured input, startup, memory and accessibility results.

If the spike fails the WebUI performance or isolation criteria, fall back to
Option B (native Views chrome) for the affected surfaces rather than weakening
the product or security requirements.
