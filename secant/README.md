<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Secant product layer

`secant/` is copied into the Chromium checkout as `src/secant` by
`build/chromium/sync.py`. It owns Secant-specific browser model, UI, settings,
migration and platform code. It is not a replacement for `//chrome`.

## Phase 1 structure

| Path | Purpose | State |
| --- | --- | --- |
| `app/` | Browser bootstrap and `SecantBrowserMainParts` | Skeleton |
| `browser/` | Tab, workspace, session and product model | Tab model prototype |
| `webui/` | Privileged WebUI chrome and Mojo boundary | Skeleton |

## Rules

- Keep Chromium patches in `patches/chromium/`, not here.
- Do not disable sandboxing, site isolation, certificate validation,
  permissions or Safe Browsing.
- WebUI uses a dedicated privileged scheme, strict CSP, local assets and a
  narrow Mojo API.
- A feature is not complete without an acceptance test or visual comparison.
