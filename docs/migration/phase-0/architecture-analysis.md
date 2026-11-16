<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Phase 0 — Architecture analysis

## 1. Executive finding

Zen is not a standalone browser repository. It is a **product overlay on
Mozilla Firefox**, assembled by the `surfer` tool:

```text
┌──────────────────────────────────────────────────────────────────────┐
│ Upstream Firefox 160 (mozilla-central)                               │
│ Gecko layout · SpiderMonkey · Necko · NSS · Servo style · XUL        │
│ Firefox browser chrome (browser.xhtml, tabbrowser, sessionstore,      │
│ urlbar, places, add-ons, devtools, installers)                       │
└──────────────────────────────────────────────────────────────────────┘
                 ▲ 283 patches applied by surfer
┌──────────────────────────────────────────────────────────────────────┐
│ Zen patch layer (`src/**/*.patch`)                                   │
│ 122 browser · 41 toolkit · 32 layout · 18 dom · 15 widget · ...      │
└──────────────────────────────────────────────────────────────────────┘
                 ▲ imported into `engine/`
┌──────────────────────────────────────────────────────────────────────┐
│ Zen product overlay (`src/zen`)                                      │
│ JS modules · CSS · XUL/XHTML · C++ XPCOM · prefs · l10n · tokens     │
└──────────────────────────────────────────────────────────────────────┘
                 ▲ configured by
┌──────────────────────────────────────────────────────────────────────┐
│ surfer.json · configs/ · prefs/ · build/ · scripts/ · .github/       │
└──────────────────────────────────────────────────────────────────────┘
```

The Firefox source tree is **not vendored**. `engine/` does not exist in a
fresh checkout; `npm run download` (surfer) fetches it. Consequently:

- there is no browser binary in this checkout;
- the Firefox version is defined by `surfer.json` (`product: firefox`,
  `version: 160.0`);
- Zen's code is meaningful only after surfer applies it to a Mozilla tree;
- replacing the foundation means replacing the tree underneath the overlay,
  not editing a few imports.

## 2. Repository map

| Path | Role |
| --- | --- |
| `src/zen/` | Zen product code (the migration source of truth) |
| `src/**/*.patch` | Patches to Firefox files (283 total) |
| `src/external-patches/` | Third-party/Firefox patches (9) |
| `prefs/zen/` | 193 Zen prefs in 28 YAML files |
| `prefs/firefox/`, `prefs/privatefox/`, `prefs/fastfox/` | Firefox behavior overrides |
| `configs/` | mozconfigs, branding, search dumps |
| `build/` | AppDir, Flatpak, signing, Windows, TypeScript build assets |
| `tools/` | ffprefs generator, lint plugin, VirusTotal checker |
| `scripts/` | token build, FF sync, tests, packaging, release helpers |
| `locales/` | Localization |
| `.github/workflows/` | Build, release, sync, token and test CI |
| `docs/` | Product and contributor documentation |
| `surfer.json` | Product identity, Firefox version, brands, update host |
| `package.json` | Build/test/lint entry points |

### Code volume (non-test)

| Type | Lines |
| --- | ---: |
| `.mjs` | 66,689 |
| `.css` | 14,605 |
| `.js` | 3,471 |
| `.cpp` | 3,389 |
| `.h` | 874 |
| `.xhtml` | 533 |
| `.mm` | 388 |
| **Total** | **89,949** |

There are 1,371 files under `src/zen`; 1,019 are tests and 352 are non-test
files. The product layer is therefore substantial, but small relative to
Chromium. The hard part is not line count; it is the depth of coupling to
Firefox's browser chrome, session model, extension system and Gecko layout.

## 3. Build and packaging pipeline

Current flow (from `package.json` and `surfer.json`):

```text
npm run init
  ├─ surfer download      # fetch Firefox 160 source into engine/
  ├─ npm run import
  │   ├─ npm run ffprefs  # YAML prefs -> generated pref source
  │   ├─ import:dumps     # search/service dumps
  │   └─ surfer import    # copy src/zen + apply src/**/*.patch
  └─ surfer bootstrap     # toolchain/bootstrap

npm run build             # surfer build -> mach build
npm run package           # surfer package
npm start                 # cd engine && ./mach run --noprofile
```

Build configuration:

- `configs/common/mozconfig` — application `browser`, branding, LTO/PGO,
  unsigned add-on scopes, telemetry disabled, signing disabled for local dev.
- `configs/{linux,macos,windows}/mozconfig` — platform flags.
- `configs/branding/` — icons, installer branding, locale branding.
- `build/{AppDir,flatpak,signing,windows}` — Linux AppImage/Flatpak, signing,
  Windows installer assets.
- `.github/workflows/*` — release builds, profile builds, candidate checks,
  upstream sync, token checks, PR tests.

Packaging and update infrastructure are Firefox-specific: MAR updates,
`updates.zen-browser.app`, NSIS/MSIX, macOS DMG, Flatpak/AppImage. None of it
carries over to Chromium unchanged.

## 4. Runtime architecture (current)

### 4.1 Browser chrome

Firefox's `browser.xhtml` is the top-level chrome document. Zen:

- loads global modules listed in `src/zen/zen.globals.mjs`;
- imports its components through `src/zen/ZenComponents.manifest` and
  `src/zen/moz.build`;
- manipulates the DOM to create the vertical tab strip, sidebar, workspace
  switcher and panels;
- patches Firefox's `browser-init`, `browser`, `navigator-toolbox`,
  `tabbrowser`, `sessionstore`, `urlbar`, `CustomizableUI`, preferences,
  migration, places and theme files.

Key global surfaces:

| Surface | Provider | Zen use |
| --- | --- | --- |
| `gBrowser` | Firefox tabbrowser | Tabs, pinning, grouping, progress, selection |
| `gBrowserInit` | Firefox startup | Delayed startup ordering |
| `gURLBar` | Firefox urlbar | Address bar, search mode, results |
| `SessionStore` | Firefox sessionstore | Tab/window state, closed tabs |
| `PlacesUtils` | Firefox places | History, bookmarks, favicons |
| `CustomizableUI` | Firefox | Toolbar customization, widget areas |
| `AddonManager` / WebExtensions | Firefox | Extension install, APIs, UI |
| `Services.*` | XPCOM | Prefs, observers, IO, windows, prompts |
| `ChromeUtils` / `XPCOMUtils` | Gecko JS | Module loading, lazy getters, XPCOM |
| `JSWindowActor*` | Gecko IPC | Content/chrome actors (glance, boosts, drag, mods) |

### 4.2 Zen feature modules

| Module | Responsibility | Primary Firefox coupling |
| --- | --- | --- |
| `spaces/ZenSpaceManager.mjs` | Workspaces, workspace switching, tabs per space | `gBrowser`, `SessionStore`, containers, prefs |
| `spaces/ZenSpace.mjs` | Workspace UI element | XUL, `gBrowser.tabContainer` |
| `common/modules/ZenVerticalTabsManager.mjs` | Vertical tab strip, sidebar layout, window controls | XUL, `CustomizableUI`, `gBrowser.tabContainer` |
| `common/modules/ZenUIManager.mjs` | Window UI glue, panels, toasts, URL bar controller | XUL, `CustomizableUI`, `Services.*` |
| `tabs/ZenPinnedTabManager.mjs` | Pinned tabs, essentials, unload/reset | `gBrowser`, `SessionStore`, `PlacesUtils` |
| `folders/ZenFolders.mjs` | Tab folders, nesting, drag/drop | `gBrowser`, tab groups, XUL |
| `split-view/ZenViewSplitter.mjs` | Split view, panes, resize, drag | `gBrowser`, `tabpanels`, `SessionStore` |
| `glance/ZenGlanceManager.mjs` | Peek/glance overlay, actors | `gBrowser`, `Services.io`, actors |
| `library/ZenLibrary.mjs` | Sidebar library: history, downloads, media, spaces | Lit, `PlacesUtils`, `Downloads`, prefs |
| `mods/ZenMods.mjs` | Theme/mod packages, CSS injection | XPCOM backend, `SessionStore`, prefs |
| `boosts/ZenBoostsManager.sys.mjs` | Per-site CSS boosts | XPCOM backend, content actors, file picker |
| `sessionstore/ZenSessionManager.sys.mjs` | Sidebar/workspace session data, backups | `SessionStore` internals, `PlacesUtils` |
| `sync/ZenSpacesSync*.mjs` | Spaces Sync | Firefox Sync, `ContextualIdentityService` |
| `space-routing/*` | Route URLs to workspaces/containers | `ContextualIdentityService`, navigation |
| `urlbar/*` | Zen URL bar providers and behavior | Firefox `Urlbar*` modules |
| `kbs/*` | System-wide shortcuts | XUL keysets, native hotkey backends |
| `compact-mode/*` | Compact sidebar, edge hover | Native mouse tracker, XUL |
| `toolkit/common/*` | Native share, haptics | XPCOM, macOS/Windows share APIs |
| `drag-and-drop/*`, `window-drag/*` | Tab/window drag behavior | Gecko drag service, Cocoa/Win32 |
| `media/*` | Media controls | Firefox media control, XUL |
| `little-window/*` | Compact popup window | `gBrowser`, Firefox windows |
| `migration/*` | Import from Firefox/Arc/Dia | Firefox migration wizard, Places |
| `live-folders/*` | GitHub/RSS live folders | Places, fetch |
| `share/*` | Share links, boost sharing | `Services.zen`, clipboard, fetch |
| `welcome/*` | First-run welcome | XUL/XHTML |
| `about/*` | `about:zen` page | XUL about redirector |

### 4.3 Native components

Zen ships 10 XPCOM services and several platform backends:

| Component | Interface | Purpose | Gecko coupling |
| --- | --- | --- | --- |
| `ZenCommonUtils` | `nsIZenCommonUtils` | Native share, haptics | `nsIWindowMediator`, `nsISharePicker`, Cocoa/Win |
| `ZenMouseTracker` | `nsIZenMouseTracker` | Compact-mode edge hover | `nsIWidget`, `nsIObserver`, Cocoa/Win32 |
| `nsZenDragAndDrop` | `nsIZenDragAndDrop` | Tab drop landing | Gecko drag service, Cocoa |
| `nsZenWindowDragUtils` | `nsIZenWindowDragUtils` | Window drag from content | Gecko widget/window |
| `nsZenLibraryFiles` | `nsIZenLibraryFiles` | Library file access | Gecko file APIs |
| `nsZenModsBackend` | `nsIZenModsBackend` | Mod stylesheet registration | Servo style set, `Document` |
| `nsZenBoostsBackend` | `nsIZenBoostsBackend` | Per-domain boost registry | `BrowsingContext`, Servo |
| `ZenStyleSheetCache` | internal | Mod stylesheet cache | Servo |
| `ZenGlobalShortcuts` | internal | System-wide shortcuts | XUL keysets, Cocoa/Win32 |
| `ZenBoosts*` | internal | Boost CSS computation | Servo style, `Document` |

These are not portable C++: they depend on Gecko types (`mozilla::dom`,
`mozilla::widget`, `ServoStyleSet`, `BrowsingContext`, `nsIWidget`). Each needs
a Chromium equivalent or a product-level redesign.

### 4.4 Data and persistence

Firefox profile data used by Zen:

- `prefs.js` — all Zen prefs (193 registered names) and Firefox behavior.
- `places.sqlite` — history, bookmarks, favicons, workspace bookmark folders.
- `cookies.sqlite`, `permissions.sqlite`, `logins.json`/`key4.db`.
- `sessionstore.jsonlz4` / recovery files — windows, tabs, groups, Zen state.
- `extensions/` and `storage/` — add-ons and extension data.
- Zen-specific JSON/prefs for workspaces, pinned tabs, essentials, mods,
  boosts, library tabs, window sync.

Any migration must map this data into Chromium's profile layout without silent
loss. See the feature inventory for per-feature migration notes.

## 5. Test architecture (current)

| Layer | Count | Notes |
| --- | ---: | --- |
| Zen-specific test files | 232 | 32 suites under `src/zen/tests/` |
| Zen-specific suites | 32 | browser-chrome tests, some xpcshell |
| Imported upstream tests | 529 | Firefox mochitest/xpcshell suites re-run against Zen |
| Total JS test files | 761 | Includes imported upstream tests |
| gtest units | 4 | Boost C++ units under `src/zen/boosts/gtest` |
| Lint | `mach lint zen`, tokens check, ESLint/Stylelint | Firefox toolchain |

Commands: `npm test`, `npm run test:gtest`, `npm run lint`, `npm run tokens --
--check`.

These tests are **reference specifications**, not a runnable Secant suite.
They encode expected Zen behavior and are the primary source for porting
acceptance tests. They cannot execute against Chromium.

## 6. What can be reused, what must be rebuilt

| Asset / layer | Reuse | Notes |
| --- | --- | --- |
| Design tokens (`src/zen/tokens`, `zen-tokens.css`) | **High** | Platform-neutral JSON/CSS variables |
| Icons, fonts, images | **High** | Check licenses; convert as needed |
| Localization strings | **High** | FTL to Chromium l10n format; review plural/gender |
| CSS visual styling | **Medium-High** | Much is reusable in WebUI; XUL selectors must be rewritten |
| XUL/XHTML markup | **Low** | Recreate in HTML/custom elements |
| Zen JS feature logic | **Medium** | Behavior reusable; APIs and DOM bindings rewritten |
| C++ XPCOM components | **Low** | Reimplement on Chromium/Views/platform APIs |
| Firefox patches | **None directly** | Triage and reimplement behavior; never reapply |
| Firefox browser services | **None** | Replace with Chromium services |
| Tests | **Reference only** | Port assertions to Chromium/WebUI harnesses |
| Packaging/update | **None** | Replace with Chromium packaging/update |
| Build system | **None** | GN/Ninja/gclient replaces surfer/mach |

## 7. Consequences for the migration

1. **Two-layer replacement.** Replacing Gecko under the existing overlay is not
   possible: the overlay assumes Firefox's chrome document, globals, services,
   session store and extension APIs. The product layer must be ported onto a
   Chromium browser architecture.
2. **UI is the largest parity risk.** Zen's interface is XUL/CSS/JS bound to
   Firefox's DOM and `gBrowser`. A WebUI chrome surface is the closest path to
   visual parity, but it must be validated for input latency, accessibility,
   IME and window integration.
3. **Workspaces/containers are the largest behavioral risk.** Firefox
   contextual identities have no direct Chromium equivalent. The chosen
   isolation model affects cookies, storage, permissions, sessions and sync.
4. **Extensions and mods need an explicit policy.** Chrome extensions are not
   Firefox WebExtensions; Zen Mods and `userChrome`-style customization have no
   native Chromium equivalent.
5. **Security must be preserved, not traded.** Some Zen patches touch
   certificate checks and platform security. They are not ported as-is; each is
   reviewed and reimplemented only if it does not weaken Chromium protections.
6. **The migration is multi-release.** A 90k-line product layer, 283 patches,
   193 prefs and 761 tests cannot be faithfully ported in one change. The phase
   plan in `migration-plan.md` gates each layer.

## 8. Target architecture

```text
Secant (Chromium fork, pinned release branch)
├── //chrome/browser
│   ├── profiles, lifecycle, navigation, permissions, downloads
│   ├── extensions, devtools host, history, bookmarks
│   └── SecantBrowserMainParts, SecantBrowserContext
├── //content
│   ├── Blink rendering
│   ├── V8 JavaScript
│   ├── site isolation, sandbox, Mojo IPC
│   └── network service
├── //secant
│   ├── browser/        tab model, workspace service, session metadata
│   ├── services/       essentials, folders, space routing, window sync, share
│   ├── ui/             Views window shell, native menus, dialogs, platform effects
│   ├── webui/          HTML/CSS/JS browser chrome (vertical tabs, sidebar,
│   │                   workspaces, glance, split view, library, settings)
│   ├── theme/          tokens, mods, boosts, theme service
│   ├── platform/       Windows, Linux, macOS integration
│   └── resources/      icons, fonts, l10n
├── //extensions        Chrome extension platform + compatibility shims
└── //third_party       Chromium dependencies
```

The detailed decision, alternatives and limitations are in
[chromium-integration-evaluation.md](chromium-integration-evaluation.md) and
[ADR 0001](../adr/0001-chromium-foundation.md).
