<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Chromium patches

This directory is for reviewed patches against the pinned Chromium source.
`build/chromium/sync.py` applies every `*.patch` here with `git apply --3way`
before copying `secant/` into `src/secant`.

## Applied patches

| Patch | Owner | Reason | Upstream plan |
| --- | --- | --- | --- |
| `0001-build-expose-secant-target.patch` | Secant browser team | Adds a non-default `secant_product` root group so GN loads and can build `//secant` | Fold into the final browser target once `//secant` is wired into `//chrome`; no upstream intent |

## Naming

`NNNN-short-description.patch`, for example `0001-build-expose-secant-target.patch`.

## Policy

- Every patch needs an owner, a reason, a Chromium bug or upstream link, and a
  removal or upstreaming plan.
- Security-relevant patches need a named security reviewer and a threat-model
  note.
- Do not add patches that disable the sandbox, site isolation, certificate
  validation, permissions or Safe Browsing.
- Do not port Firefox patches from `src/external-patches/firefox/`.
- Rebase and test every patch at each Chromium pin update.
