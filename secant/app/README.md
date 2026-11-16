<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Secant app bootstrap

Planned Phase 1 contents:

- `SecantMain` / browser process entry point.
- `SecantBrowserMainParts` over `ChromeBrowserMainParts`.
- `SecantBrowserProcess` and profile wiring.
- Native window creation with a `views::WebView` chrome surface.

This directory is a skeleton until the first Chromium patch adds the app target
to the build graph.
