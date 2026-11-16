<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Phase 0 — Firefox/Gecko dependency map

## 1. Method

This map is based on static inspection of the checkout:

- 1,371 files under `src/zen` (1,019 tests, 352 non-test files);
- 89,949 non-test source lines;
- 283 patches under `src/`;
- 193 Zen prefs in 28 YAML files;
- API-usage counts from `rg` over `src/zen` excluding tests;
- manifests, `moz.build` files, `components.conf` files and native headers.

It is complete at the **subsystem** level. File-by-file behavioral mapping
continues in Phase 1 through the patch and module registers. "Chromium
equivalent" below means the intended replacement, not an already implemented
feature.

## 2. Engine-level dependencies

| Firefox/Gecko technology | Where it is used | Chromium replacement | Port risk |
| --- | --- | --- | --- |
| Gecko layout and painting | All content; 32 layout patches, 4 gfx patches, 4 image patches | Blink layout/paint, cc/Viz, Skia | High for Zen visual effects |
| SpiderMonkey | Chrome JS, content JS, all `.mjs` modules | V8 | Medium: language-compatible; APIs are not |
| Servo style system | 4 servo patches; Zen mods/boosts style backends | Blink style engine | High for mods/boosts and native theme effects |
| XUL | 28 files use `MozXULElement`; 22 use `createXULElement`; `browser.xhtml`, panels, menus | HTML/custom elements in WebUI; Views for native surfaces | High |
| XPCOM / `nsI*` | 98 files reference `nsI*`; 43 reference XPCOM | Mojo interfaces, C++ services, `base::` APIs | High |
| Necko networking | Firefox network stack; URL fixup, proxy, DNS, certs | Chromium NetworkService, URLLoader, BoringSSL | Medium |
| NSS / certificate store | Firefox cert validation and enterprise roots | Chromium cert verifier, Chrome Root Store | Medium; behavior differs |
| Gecko IPC / JS actors | 12 files use `JSWindowActor*`; `PContent.ipdl` patch | Mojo, `RenderFrameHost`, `WebContents` | High |
| Firefox WebExtensions | 11 extension/add-on patches; `Extension*` in 14 files | Chromium extensions (MV2/MV3) + shims | High |
| Firefox sessionstore | 7 patches; `ZenSessionManager`; `SessionStore` in 25 files | Chromium `SessionService` + Secant metadata | High |
| Firefox Places | `PlacesUtils` in 13 files; history/bookmarks/favicons | Chromium `HistoryService`, `BookmarkModel` | Medium-high |
| Firefox browser chrome | `gBrowser` in 49 files; `browser.xhtml` and 122 browser patches | Chromium `Browser`/`BrowserView` + Secant WebUI | High |
| Firefox prefs | 193 Zen prefs; 238 `Services.prefs` occurrences in 50 files | Chromium `PrefService` + WebUI settings | Medium |
| Firefox installers/update | MAR, NSIS/MSIX, DMG, Flatpak, AppImage, update host | Chromium installers/update (Omaha, Sparkle, portals) | High |

## 3. Dependency matrix by domain

### 3.1 Browser shell and chrome UI

**Zen implementation**

- `common/modules/ZenStartup.mjs` — per-window startup ordering and layout.
- `common/modules/ZenUIManager.mjs` — panels, toasts, URL bar glue, toolbar updates.
- `common/modules/ZenVerticalTabsManager.mjs` — sidebar/toolbar layout, window controls.
- `common/modules/ZenMenubar.mjs`, `ZenToasts.mjs`, `ZenSidebarNotification.mjs`.
- `spaces/zen-workspaces.css`, `tabs/zen-tabs/*.css`, `common/styles/*.css`.
- Patches: `browser.xhtml`, `navigator-toolbox`, `browser-init`, `browser`,
  `browser-sets`, `main-popupset`, `panelUI`, `CustomizableUI`, themes.

**Firefox/Gecko dependencies**

- XUL DOM, `MozXULElement`, `createXULElement`, XUL `<panel>`, `<menupopup>`.
- `gBrowser`, `gBrowserInit`, `gURLBar`, `CustomizableUI`, `gNavToolbox`.
- `Services.prefs`, `Services.obs`, `Services.wm`, `Services.scriptloader`.
- Firefox CSS variables and theme infrastructure.

**Chromium replacement**

- Native `views::Widget`/`BrowserFrame` for the top-level window, window
  controls, menus, dialogs and OS integration.
- `//secant/webui` browser-chrome surface hosted in a `views::WebView`:
  vertical tab strip, sidebar, workspace switcher, panels, library, glance and
  split-view controls.
- WebUI communicates with the browser process through Mojo message handlers.
- Design tokens and most CSS are reused; XUL selectors and bindings are
  rewritten as HTML/custom elements.

**Risk:** High. A full WebUI chrome surface is non-standard for desktop
Chromium; input latency, focus, IME, accessibility and window dragging must be
validated in the Phase 1 spike.

### 3.2 Tab management

**Zen implementation**

- Firefox `tabbrowser` plus 10+ patches (`Tabbrowser`, `TabUnloader`,
  `TabsList`, `AsyncTabSwitcher`, `tab`, `tabs`, `tab-context-menu`,
  `drag-and-drop`, `tabgroup`, `browser-ctrlTab`).
- `tabs/ZenPinnedTabManager.mjs`, `tabs/ZenTabRenamer.mjs`,
  `tabs/ZenEssentialsPromo.mjs`.
- `folders/ZenFolders.mjs`, `folders/ZenFolder.mjs`.
- `split-view/ZenViewSplitter.mjs`; `glance/ZenGlanceManager.mjs`.
- `spaces/ZenSpaceManager.mjs` owns workspace membership.

**Firefox/Gecko dependencies**

- `gBrowser` (49 files), `MozTabbrowserTabGroup`, `tabContainer`,
  `tabpanels`, `TabState`, `TabStateFlusher`, `AsyncTabSwitcher`.
- XUL tab elements and Firefox drag/drop.
- `SessionStore` for tab state and restoration.

**Chromium replacement**

- `TabStripModel` as a low-level ordered tab model, wrapped by
  `SecantTabModel` for workspace, folder, essential, pinned, split and glance
  metadata.
- One `WebContents` per tab; split view composes multiple `WebContents` in a
  custom layout.
- Custom `TabStripLayout`/WebUI vertical tab strip; do not use Chromium's
  horizontal `TabStrip` view.
- `TabRestoreService` for closed tabs, extended with Secant metadata.

**Risk:** High. Chromium's tab model assumes a horizontal strip and one
`WebContents` per tab. Split view, glance tabs, folders and essentials are
product-specific extensions that must not fork `TabStripModel` in incompatible
ways.

### 3.3 Session management

**Zen implementation**

- `sessionstore/ZenSessionManager.sys.mjs` (1,077 lines) and
  `ZenWindowSync.sys.mjs`.
- Patches: `SessionStore`, `SessionFile`, `SessionSaver`, `SessionStartup`,
  `TabState`, `TabStateFlusher`, `TabGroupState`.
- Workspace/sidebar session data, closed tabs, backups, window sync.

**Firefox/Gecko dependencies**

- `SessionStore` internals (`_windows`, `_browserWindows`, deferred initial
  state), `JSONFile`, `PlacesUtils` DB connection, `Services.dirsvc`,
  `Services.appinfo`.
- `sessionstore.jsonlz4` and Firefox recovery format.

**Chromium replacement**

- Chromium `sessions::SessionService` / `SessionService` for navigation and
  window/tab state.
- `SecantSessionMetadata` sidecar (versioned JSON or SQLite) keyed by session
  and tab IDs for workspaces, folders, essentials, split groups and glance
  state.
- Chromium crash-restore and `TabRestoreService`; custom backup policy.
- Window sync becomes a Secant service using a defined transport, not Firefox
  Sync internals.

**Risk:** High. Session data loss is unacceptable. The format, restore order,
crash recovery, private windows and multi-window behavior need dedicated
acceptance tests and migration fixtures.

### 3.4 Profiles and private browsing

**Zen implementation**

- Firefox profile service patch (`nsToolkitProfileService`), profile context
  menu, little window, migration wizard.
- `little-window/*`, `migration/*`.
- Private windows/workspaces via Firefox private browsing.

**Firefox/Gecko dependencies**

- `nsToolkitProfileService`, `nsXREDirProvider`, Firefox profile layout,
  `browser.startup.*` prefs.
- Firefox private-browsing session model.

**Chromium replacement**

- `ProfileManager`, `Profile`, `ProfileAttributesStorage`, profile picker.
- `OffTheRecord`/incognito profile for private windows.
- Secant profile metadata for workspace and window-sync settings.
- Profile migration from Firefox profile (see 3.10).

**Risk:** Medium-high. Per-window profiles, little-window semantics and
profile switching differ between Firefox and Chromium.

### 3.5 Extensions and add-ons

**Zen implementation**

- Patches: `AddonManager`, `Blocklist`, `AddonUpdateChecker`, `ExtensionsUI`,
  `ext-browser`, `ext-tabs`, `ext-runtime`, `ext-tabs-base`, `aboutaddons`,
  `ActivityStream`.
- Firefox WebExtension APIs and unsigned add-on scopes.

**Firefox/Gecko dependencies**

- `AddonManager`, `ExtensionParent`, `Extension`, `ExtensionData`,
  `browser.*` APIs, Firefox add-on signing and blocklist.
- `about:addons` XUL/HTML.

**Chromium replacement**

- `ExtensionSystem`, `ExtensionRegistry`, `ExtensionService`,
  `ExtensionsBrowserClient`.
- Chrome extension APIs (MV2/MV3), Chrome Web Store or a Secant store.
- Compatibility shims for high-value Firefox APIs where semantics can be
  preserved (`tabs`, `runtime`, `storage`, `webNavigation`).
- Zen Mods are **not** WebExtensions; they become a separate Secant theme/mod
  package system.
- Firefox-only APIs (`sidebar_action`, containers, tab hiding, theme
  experiments) require explicit product decisions; some cannot be reproduced.

**Risk:** High. Extension compatibility is a product promise that cannot be
fully met for Firefox-only APIs. A compatibility matrix and user-facing policy
are required before Phase 4.

### 3.6 Networking, certificates and safe browsing

**Zen implementation**

- Minimal product code; URL fixup, proxy/DNS prefs, safe browsing keys in
  `configs/common/mozconfig`.
- `external-patches/firefox/override_cert_checks_temp.patch` (must not be
  ported).
- Safe browsing and remote settings tests imported from Firefox.

**Firefox/Gecko dependencies**

- Necko, NSS, Firefox certificate store, Safe Browsing, remote settings,
  DoH/TRR, proxy handling.

**Chromium replacement**

- Chromium NetworkService, URLLoader, cookie/store, HSTS, proxy resolution,
  DNS/DoH, BoringSSL and Chromium cert verifier.
- Chromium Safe Browsing and download protection.
- API keys and service terms require review.

**Risk:** Medium. Behavioral differences in certificate handling, enterprise
roots, proxy and DNS are expected. Security review must confirm no protection
is weakened.

### 3.7 Permissions and site settings

**Zen implementation**

- `urlbar/ZenSiteDataPanel.sys.mjs`; site control tests.
- Workspace/container-scoped permissions and data clearing.

**Firefox/Gecko dependencies**

- `nsIPermissionManager`, Firefox site identity panel, contextual identities,
  `permissions.sqlite`.

**Chromium replacement**

- `PermissionManager`, `HostContentSettingsMap`, `ContentSettingsType`,
  Page Info / Site Settings WebUI.
- Workspace isolation model determines permission scope (see 3.8).

**Risk:** High for container/workspace-scoped permissions; medium for standard
site permissions.

### 3.8 Workspaces, containers and space routing (highest risk)

**Zen implementation**

- `spaces/ZenSpaceManager.mjs`, `ZenSpace.mjs`, `ZenSpaceCreation.mjs`,
  `ZenSpaceIcons.mjs`, `ZenSpaceBookmarksStorage.js`, `ZenSpacesSwipe.mjs`.
- `space-routing/ZenSpaceRoutingManager.sys.mjs`,
  `ZenSpaceRoutingNavigation.sys.mjs`.
- `sync/ZenSpacesSync*.mjs`; `ZenMigrationUtils` container creation.
- Prefs: `zen.workspaces.*`, `zen.containers.*`, `zen.spaces-sync.*`.

**Firefox/Gecko dependencies**

- Firefox contextual identities (`ContextualIdentityService`, `userContextId`).
- `gBrowser` tab membership, `SessionStore`, `PlacesUtils`, Firefox Sync.
- `browser.tabs`/container APIs for extensions.

**Chromium replacement — unresolved**

Chromium has profiles and storage partitions, but no first-class lightweight
"container" identity. Candidate models:

1. **Profile per workspace** — strongest isolation, highest cost; separate
   cookies/cache/storage; history/session sharing is hard.
2. **Storage partition per workspace** — custom `BrowserContext`/storage
   partition keyed by workspace; requires Chromium changes and careful cookie
   jar isolation; likely the best fidelity/cost tradeoff if feasible.
3. **Emulated containers in an extension layer** — cookie partition tricks;
   incomplete for cache, permissions, auth and site data; not acceptable as the
   sole model.
4. **Workspace as metadata only** — tabs grouped by workspace but share
   cookies/storage; least faithful; only acceptable if explicitly documented.

A Phase 3 spike must choose and prove one model before workspace UI is built.
The choice affects cookies, permissions, sessions, sync, private browsing and
site data clearing.

**Risk:** Critical. This is the largest behavioral-parity risk in the project.

### 3.9 Preferences

**Zen implementation**

- 193 prefs in `prefs/zen/*.yaml`, generated by `tools/ffprefs`.
- `Services.prefs` used 238 times across 50 files; `defineLazyPreferenceGetter`
  62 times.
- Firefox behavior overrides in `prefs/firefox`, `prefs/privatefox`,
  `prefs/fastfox`.

**Firefox/Gecko dependencies**

- Firefox pref service, `StaticPrefList.yaml`, pref observers, sync allowlist.

**Chromium replacement**

- `PrefService` and registered pref names in `//secant`.
- WebUI settings pages for user-facing options.
- A pref migration table mapping Zen/Firefox names to Secant names and
  defaults; unsupported Firefox prefs documented.
- Chromium Sync for a subset; Zen Spaces Sync needs a new backend.

**Risk:** Medium. Pref semantics and defaults are product behavior; silent
renames break parity and user settings.

### 3.10 History, bookmarks, downloads and library

**Zen implementation**

- `library/ZenLibrary.mjs`, `ZenLibraryWidget.sys.mjs`,
  `ZenLibraryUtils.sys.mjs`, section modules for history, downloads, media,
  spaces, boosts, search and space routing.
- `spaces/ZenSpaceBookmarksStorage.js`; bookmarks-as-workspaces migration.
- `downloads/ZenDownloadAnimation.mjs`; library download tests.
- `live-folders/*` for GitHub/RSS live folders.

**Firefox/Gecko dependencies**

- `PlacesUtils` (13 files), `places.sqlite`, favicons, Firefox downloads API,
  Firefox bookmarks UI patches.

**Chromium replacement**

- `HistoryService`, `BookmarkModel`, `DownloadManager`, `DownloadItem`,
  favicon service.
- Secant metadata for workspace bookmark folders and library tabs.
- Live folders become a Secant service with explicit network permissions.

**Risk:** Medium-high. Data migration, favicon mapping, download UI and
workspace bookmark semantics need fixtures and acceptance tests.

### 3.11 Developer tools

**Zen implementation**

- No Zen-specific DevTools product layer; Firefox DevTools is inherited.
- DevTools locale patches only.

**Firefox/Gecko dependencies**

- Firefox DevTools frontend and protocol.

**Chromium replacement**

- Chromium DevTools frontend and CDP.
- Standard behavior; document differences (panels, shortcuts, remote
  debugging).

**Risk:** Low, but user-visible differences must be documented.

### 3.12 Platform integration

| Platform | Zen implementation | Firefox dependencies | Chromium replacement | Risk |
| --- | --- | --- | --- | --- |
| Windows | Mica/acrylic, window controls, NSIS/MSIX, global shortcuts, share, drag | `widget/windows`, `nsCocoaWindow` equivalents, Win32 APIs | `//chrome/installer`, Views/Win32, DWM, `RegisterHotKey`, OS share | High |
| macOS | Vibrancy, native popovers, mono window controls, haptics, share, global shortcuts, window drag | Cocoa `NSVisualEffectView`, `NSPopover`, `NSHapticFeedbackManager`, `NSSharingService` | `//chrome/browser/mac`, `//ui/views/cocoa`, same Cocoa APIs | High |
| Linux | GTK transparency, rounded corners, AppImage/Flatpak, swipe/gestures | GTK, X11/Wayland, `nsDragSessionSource`, `SwipeTracker` | `//chrome/browser/linux`, `//ui/gtk`, Ozone, portals | High |
| Window management | Compact mode, little window, split view, drag-from-content, sidebar drag | Gecko widget/window, native mouse tracking | Views `Widget`/`BrowserFrame`, platform event monitors | High |

Known platform limitation: global pointer tracking for compact-mode edge hover
is restricted on Wayland. A portal or compositor-specific fallback is required;
exact parity may not be possible on all compositors.

### 3.13 Native components

| Zen component | Gecko coupling | Chromium target | Risk |
| --- | --- | --- | --- |
| `ZenCommonUtils` (share/haptics) | `nsIWindowMediator`, `nsISharePicker`, Cocoa/Win | `//secant/platform`, OS share APIs, haptic API | Medium |
| `ZenMouseTracker` | `nsIWidget`, `nsIObserver`, Cocoa/Win32 | `ui::EventHandler`, platform global mouse monitor | High (Wayland) |
| `nsZenDragAndDrop` | Gecko drag service, Cocoa | Views drag/drop, `OSExchangeData`, custom tab drag | High |
| `nsZenWindowDragUtils` | Gecko widget/window | `views::Widget::StartDrag`, `BrowserFrame` | Medium |
| `nsZenLibraryFiles` | Gecko file APIs | `base::File`, profile paths | Low |
| `nsZenModsBackend` / `ZenStyleSheetCache` | Servo style set, `Document` | Secant theme service, WebUI CSS, content user styles | High |
| `nsZenBoostsBackend` / content sheets | `BrowsingContext`, Servo | Boost service, Mojo, Blink user stylesheet injection | High |
| `ZenGlobalShortcuts` | XUL keysets, Cocoa/Win32 | `ui::GlobalAcceleratorListener`, OS hotkeys, portals | Medium-high |
| `ZenBoosts*` style computation | Servo style, `Document` | Blink CSSOM/user styles | High |
| `nsZenSwipeOverrides` | Gecko gesture/overscroll | Chromium overscroll/gesture pipeline | Medium |

### 3.14 Build, packaging, updates and tests

| Area | Current | Chromium target |
| --- | --- | --- |
| Build | surfer + mozilla-central + mach | `gclient` + GN + Ninja + `autoninja` |
| Toolchain | Mozilla bootstrap, clang, rust | Chromium `depot_tools`, clang, rust, CIPD |
| Packaging | MAR, NSIS/MSIX, DMG, Flatpak, AppImage | Chromium installers, DMG, Linux packages |
| Updates | `updates.zen-browser.app`, MAR | Omaha/Sparkle/portal update or Secant service |
| Signing | Apple notarization, Windows signing | Chromium signing/notarization pipeline |
| Tests | 232 Zen tests + 529 imported Firefox tests, mach | Chromium `browser_tests`, `unit_tests`, gtest, Playwright |
| Lint | `mach lint`, ESLint, Stylelint, token check | Chromium presubmit, clang-format, ESLint, Stylelint, token check |

## 4. API usage evidence

Counts are occurrences in `src/zen` excluding tests; file counts are files
containing the symbol.

| Symbol | Occurrences | Files | Meaning |
| --- | ---: | ---: | --- |
| `Services.prefs` | 238 | 50 | Pref reads/writes/observers |
| `XPCOMUtils.defineLazyPreferenceGetter` | 62 | — | Lazy pref getters |
| `Services.obs` | 58 | 23 | Observer service |
| `ChromeUtils.defineESModuleGetters` | 57 | — | Gecko module loading |
| `ChromeUtils.defineLazyGetter` | 46 | — | Lazy globals |
| `Services.io` | 26 | 16 | URI creation |
| `ChromeUtils.importESModule` | 21 | — | Gecko module loading |
| `Services.zen` | 17 | — | Zen native XPCOM service |
| `Services.scriptSecurityManager` | 17 | — | Principal/URI security |
| `Services.prompt` | 17 | — | Native prompts |
| `Services.appinfo` | 17 | — | App metadata |
| `Services.wm` | 9 | 7 | Window mediator |
| `gBrowser` | — | 49 | Firefox tabbrowser |
| `SessionStore` | — | 25 | Firefox session store |
| `MozXULElement` | — | 28 | XUL element construction |
| `createXULElement` | — | 22 | XUL DOM |
| `PlacesUtils` | — | 13 | Places history/bookmarks |
| `JSWindowActor` | — | 12 | Gecko actor IPC |
| `CustomizableUI` | — | 9 | Firefox toolbar customization |
| `gURLBar` | — | 11 | Firefox address bar |
| `nsI*` | — | 98 | XPCOM interfaces |
| `XPCOM` | — | 43 | XPCOM module patterns |
| `nsZen*` | — | 88 | Zen-specific Gecko modules |
| `moz-src:///` | — | 34 | Firefox internal modules |
| `chrome://browser` | — | 69 | Firefox chrome resources |
| `resource:///` | — | 52 | Firefox resource modules |
| `Extension` | — | 14 | Firefox extension APIs |

Interpretation: the coupling is broad and structural, not a small adapter
surface. A compatibility shim that emulates `gBrowser`, `SessionStore`,
`PlacesUtils`, XUL and XPCOM would amount to rebuilding Firefox's chrome; it is
not a viable migration strategy.

## 5. Patch dependency map

Full list: [patch-inventory.csv](patch-inventory.csv). Counts by subsystem:

| Subsystem | Patches | Provisional disposition |
| --- | ---: | --- |
| `browser/` | 122 | Reimplement behavior in Secant services/WebUI/Views |
| `toolkit/` | 41 | Reimplement in Secant services/UI; platform review |
| `layout/` | 32 | Replace with Blink/CSS/Views; visual review |
| `dom/` | 18 | Input/navigation/IPC; security review |
| `widget/` | 15 | Platform/window effects; reimplement |
| `tools/` | 10 | Drop; replace with Chromium presubmit |
| `external-patches/` | 9 | Legal/security review; mostly not portable |
| `testing/` | 4 | Replace with Chromium test harness |
| `servo/` | 4 | Blink style replacement |
| `gfx/` | 4 | Blink/cc/Skia replacement |
| `build/` | 4 | GN/Ninja replacement |
| `modules/` | 2 | Pref mapping |
| `docshell/` | 2 | Navigation replacement |
| `devtools/` | 2 | Standard Chromium DevTools |
| `accessible/` | 2 | Chromium accessibility |
| `xpfe/` | 1 | Views window shell |
| `services/` | 1 | Sync replacement |
| `security/` | 1 | Security review |
| `python/` | 1 | Tooling replacement |
| `image/` | 1 | Blink image replacement |
| `editor/` | 1 | Blink editing |
| Root lint/config | 6 | Drop/replace |
| **Total** | **283** | |

### High-risk patches

| Patch | Why it matters | Disposition |
| --- | --- | --- |
| `external-patches/firefox/override_cert_checks_temp.patch` | Temporarily overrides certificate checks | **Do not port.** Security review required |
| `browser/components/sessionstore/*` | Session format and restore semantics | Reimplement on Chromium `SessionService` + sidecar |
| `browser/components/tabbrowser/*` | Tab model, groups, unload, drag | Reimplement in `SecantTabModel` |
| `browser/components/urlbar/*` | Address bar behavior and results | Reimplement in WebUI/omnibox |
| `browser/components/extensions/*` | Extension API behavior | Compatibility shims + policy |
| `layout/*`, `servo/*`, `gfx/*`, `widget/*` | Zen visual effects and native window behavior | Blink/Views/platform reimplementation |
| `browser/installer/*`, `toolkit/mozapps/installer/*` | Packaging and updates | Chromium packaging/update |

## 6. Non-portable items and required decisions

| Item | Status | Decision needed |
| --- | --- | --- |
| XUL chrome | No Chromium equivalent | WebUI/Views recreation |
| Firefox profile/session formats | Not readable by Chromium | One-time migration + fallback |
| Contextual identities/containers | No direct equivalent | Workspace isolation model (Phase 3 spike) |
| Firefox WebExtension APIs | Partial overlap only | Compatibility policy and shims |
| Zen Mods / `userChrome` styling | No native equivalent | Secant mod/theme package system |
| Firefox Sync | Not usable | New sync backend or documented removal |
| Firefox DevTools | Replaced | Chrome DevTools; document differences |
| MAR updates | Replaced | Chromium/Secant update service |
| Gecko visual effects | Replaced | Blink/Views/native effects |
| Wayland global pointer tracking | Platform limitation | Portal/compositor fallback |
| Codecs/Widevine | Licensing/redistribution | Legal review; optional components |
| Chrome Web Store | Terms/branding | Store policy decision |

## 7. Phase 1 unknowns to close

1. Chromium milestone and release-branch pin.
2. Whether a WebUI chrome surface meets latency/accessibility/IME budgets.
3. Whether `TabStripModel` can be extended cleanly for workspaces/folders/split.
4. Workspace/container isolation feasibility and cost.
5. Extension compatibility scope and store policy.
6. Profile/session migration fidelity.
7. Packaging, signing and update infrastructure.
8. Codec/Widevine and service-key licensing.
9. Global shortcuts and edge-hover behavior on Wayland.
10. Reproducible-build and CI cost on Blacksmith runners.
