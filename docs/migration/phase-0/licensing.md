<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Licensing and distribution review

**Status:** Phase 0 issue list, not legal advice
**Required before release:** Legal review and sign-off

This document identifies licensing, notice and distribution work for the
Chromium migration. It does not grant rights or replace counsel. Every item
marked **decision required** must be resolved before a public build.

## 1. Current repository license

This repository is licensed **MPL-2.0** (`LICENSE`, `package.json`). MPL is
file-level copyleft:

- modifications to existing MPL-covered files must remain MPL-2.0;
- new files can carry another compatible license, but files containing MPL
  code may inherit MPL obligations;
- distributed binaries must make the corresponding source available;
- patent and notice terms apply.

The migration adds Chromium (mostly BSD-3-Clause) and new Secant code. The
combination is a larger work, but each file keeps its own obligations. Counsel
must confirm the boundary and the source-availability process.

## 2. Component license matrix

| Component | Current/expected license | Obligation | Action |
| --- | --- | --- | --- |
| Zen product code (`src/zen`) | MPL-2.0 | Keep MPL files MPL; provide source for binaries | Preserve headers; audit new files |
| Zen documentation | MPL-2.0 unless stated | Notice and source availability | Keep headers; check images |
| Firefox patches (`src/**/*.patch`) | MPL-2.0 (Mozilla) | Modifications to MPL files remain MPL | Behavior reference; do not copy blindly |
| Chromium source | BSD-3-Clause for most files | Retain copyright, conditions and disclaimer; no endorsement | Aggregate notices; preserve `LICENSE` |
| Chromium `//third_party` | Varies by component | Per-component notices and terms | Generate third-party notices and SBOM |
| `external-patches/librewolf` | Review required | Review required | Confirm source license before reuse |
| Fonts | Varies (OFL, proprietary, unknown) | Embedding/redistribution terms | Asset audit; replace if unclear |
| Icons and illustrations | Varies (CC, proprietary, unknown) | Attribution/redistribution terms | Asset audit and provenance record |
| Wallpapers and welcome video | Varies | Redistribution and derivative terms | Written permission or replacement |
| Localization strings | MPL/community terms | Attribution and source terms | Review Crowdin/translation terms |
| Node/npm dependencies | Varies | Notices and license compatibility | Generate notices; audit copyleft |
| Rust/Python build dependencies | Varies | Notices and license compatibility | Generate notices; audit |
| Codecs (H.264/AAC/MP3) | Patent/licensing terms | Distribution may require licenses | Legal and packaging decision |
| Widevine CDM | Proprietary | Separate license and redistribution limits | Do not bundle without approval |
| Safe Browsing / Google services | Google API terms | API keys and usage limits | Obtain own keys or disable |
| Chromium Sync | Google service terms | Account/service restrictions | Do not assume availability |
| Chrome Web Store | Store terms | Distribution and review rules | Decide store policy |
| Firefox Sync / Mozilla services | Mozilla terms | Cannot be reused as-is | Replace or remove |
| Zen/Firefox/Chromium trademarks | Trademark law | No confusing use or endorsement | Brand and naming review |
| User data (profiles, history, cookies) | User-owned | Privacy and consent obligations | Privacy policy and migration consent |

## 3. Chromium notices

Chromium's license is primarily BSD-3-Clause. A distributed Secant build must:

1. retain Chromium copyright, license text and disclaimer;
2. reproduce third-party notices for every shipped component;
3. provide an accessible `about:credits` or equivalent notice page;
4. avoid using Google/Chromium marks to imply endorsement;
5. ship only components whose licenses permit redistribution;
6. keep build scripts that regenerate the notice file.

`//third_party` is not uniformly licensed. A component that is acceptable for
development may not be acceptable for redistribution. Each shipped target needs
a license review, not just the top-level Chromium `LICENSE`.

## 4. MPL obligations

For MPL-covered files:

- keep the MPL notice and source headers;
- publish the source form of the distributed binary, including modifications;
- do not add restrictions that conflict with MPL rights;
- identify modifications where required;
- keep patent grants intact.

The release process must produce a source archive that corresponds to the
shipped binary. A git tag alone may not satisfy the source-availability
obligation if build scripts, generated files or patches are needed to rebuild.

## 5. Branding and trademarks

- **Secant Browser** branding must be legally cleared for use and distribution.
- Do not use Mozilla, Firefox, Zen, Google, Chrome, Chromium or Edge marks in a
  way that suggests endorsement or affiliation.
- Do not ship Mozilla or Zen logos, icons, wallpapers or welcome videos without
  permission.
- Internal scheme names, package IDs, bundle IDs, installer names and update
  channels must be Secant-owned.
- `chrome://` URLs are Chromium-internal; product surfaces should use
  `secant://` where appropriate.
- The word "Chromium" may be used descriptively, but the product must have a
  distinct name and identity.

## 6. Assets and provenance

Every visual asset must have a recorded source and license:

| Asset class | Required record |
| --- | --- |
| Icons | Author/source, license, modifications, attribution |
| Fonts | Foundry/source, license, embedding permission, subsetting |
| Wallpapers | Source, license, derivative permission |
| Welcome video | Source, license, codec, redistribution permission |
| Illustrations | Source, license, attribution |
| Sounds | Source, license, redistribution |
| Localization | Source, translator terms, attribution |

Unknown provenance is treated as **not redistributable** until resolved. Do not
assume that an asset in the Zen repository is automatically licensed for Secant.

## 7. Codecs, DRM and media

This is a release blocker until decided.

- H.264, AAC and MP3 involve patent licensing in some jurisdictions.
- Widevine is proprietary and has strict redistribution terms.
- Chromium open-source builds may exclude proprietary codecs or DRM.
- A "download on demand" component does not automatically remove patent or
  contract obligations.
- The product must decide: bundle, optional download, or exclude with a clear
  user-facing limitation.
- Legal review must cover all distribution channels: website, package
  repositories, app stores and enterprise images.

## 8. Services and API keys

| Service | Issue | Action |
| --- | --- | --- |
| Safe Browsing | Google API terms and keys | Obtain approved keys or disable |
| Chromium Sync | Google service/account terms | Do not assume; design alternative |
| Geolocation | Provider terms and keys | Choose provider; disclose |
| Spellcheck | Dictionary licenses | Audit dictionaries |
| Translation | Service terms and data handling | Decide provider; privacy review |
| Crash reporting | Data processing and consent | Choose backend; privacy policy |
| Updates | Signing and hosting | Own update infrastructure |
| Search | Provider agreements | Confirm default search terms |

Do not reuse Zen, Mozilla or Google production keys. Do not commit keys to the
repository.

## 9. Extensions and store policy

- Firefox WebExtensions and Chrome extensions have different licenses, APIs and
  signing rules.
- Bundling extensions requires permission from their authors.
- A Secant store or Chrome Web Store distribution needs terms, review and
  takedown policy.
- Compatibility shims must not relicense third-party extension code.
- Zen Mods may include third-party CSS/assets; require a license field and a
  provenance record.
- Boosts can import user CSS; do not redistribute user content without consent.

## 10. Contributor and dependency policy

- Keep the existing contribution terms unless counsel approves a change.
- Require a license and provenance field for new assets.
- Run a dependency license scan in CI.
- Block copyleft dependencies that are incompatible with the distribution model
  unless reviewed.
- Generate an SBOM for each release.
- Keep a `NOTICE`/credits file current.

## 11. Release checklist

- [ ] Legal review of the MPL/BSD/Apache/third-party combination.
- [ ] Chromium and third-party notices generated and shipped.
- [ ] Source archive for the exact binary produced and published.
- [ ] Asset provenance audit complete; unknown assets replaced.
- [ ] Font and icon embedding permissions confirmed.
- [ ] Codec/DRM decision documented and approved.
- [ ] Safe Browsing and other API keys approved or features disabled.
- [ ] Sync scope approved; no Mozilla service dependency.
- [ ] Extension store and signing policy approved.
- [ ] Secant trademarks cleared; no confusing Mozilla/Google/Zen marks.
- [ ] Privacy policy covers profiles, sync, crash reports and telemetry.
- [ ] SBOM and dependency license scan archived.
- [ ] Installer, package and app-store terms reviewed per channel.
- [ ] Export and sanctions review completed if required.

## 12. Open decisions

| ID | Decision | Owner | Gate |
| --- | --- | --- | --- |
| L-01 | MPL source-availability process | Legal + release | Phase 1 |
| L-02 | Chromium notice generation | Release | Phase 1 |
| L-03 | Asset provenance and replacement list | Design + legal | Phase 1 |
| L-04 | Codec/DRM distribution model | Legal + product | Phase 5 |
| L-05 | Safe Browsing/API key strategy | Security + legal | Phase 4 |
| L-06 | Sync backend and service terms | Product + legal | Phase 4 |
| L-07 | Extension store/signing policy | Product + legal | Phase 4 |
| L-08 | Secant trademark clearance | Legal | Phase 1 |
| L-09 | Privacy policy and consent | Legal + product | Phase 5 |
| L-10 | SBOM and dependency policy | Release | Phase 1 |
