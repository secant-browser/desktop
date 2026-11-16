<!--
   - This Source Code Form is subject to the terms of the Mozilla Public
   - License, v. 2.0. If a copy of the MPL was not distributed with this
   - file, You can obtain one at http://mozilla.org/MPL/2.0/.
   -->

# Risk register

**Status:** Phase 0 baseline
**Review cadence:** Monthly and at every phase gate
**Owners:** Role-based until named owners are assigned before Phase 1

## 1. Scoring

Impact and likelihood are scored 1–5. Rating is `impact × likelihood`.

| Rating | Score | Response |
| --- | ---: | --- |
| Critical | 20–25 | Stop-the-line; executive/architecture decision required |
| High | 12–19 | Active mitigation and phase-gate evidence required |
| Medium | 6–11 | Monitor; mitigation plan before the relevant phase |
| Low | 1–5 | Accept or monitor |

**Risk appetite:** zero tolerance for weakening sandbox, site isolation,
certificate validation or permission enforcement; very low tolerance for silent
data loss; documented, tested differences are acceptable for non-security
behavior.

## 2. Summary

| ID | Risk | Impact | Likelihood | Rating | Phase | Owner |
| --- | --- | ---: | ---: | ---: | --- | --- |
| R-01 | Workspace/container isolation has no direct Chromium equivalent | 5 | 5 | **25 Critical** | 3 | Workspaces + Security |
| R-02 | WebUI browser chrome misses performance/accessibility/IME budgets | 4 | 4 | **16 High** | 1–2 | WebUI + QA |
| R-03 | Firefox extension compatibility cannot be preserved | 4 | 4 | **16 High** | 4 | Extensions + Product |
| R-04 | Zen Mods/userChrome customization has no safe equivalent | 4 | 4 | **16 High** | 4 | Mods + Security |
| R-05 | Boosts cannot reproduce Gecko per-site style behavior | 4 | 3 | **12 High** | 4 | Boosts + WebUI |
| R-06 | Session/profile migration loses or corrupts data | 4 | 3 | **12 High** | 3 | Sessions + Data |
| R-07 | Chromium fork becomes unmaintainable across rebases | 4 | 4 | **16 High** | Ongoing | Architecture + Release |
| R-08 | A security protection is weakened by a port or WebUI API | 5 | 3 | **15 High** | Ongoing | Security |
| R-09 | Codec/DRM licensing blocks distribution | 4 | 4 | **16 High** | 5 | Legal + Release |
| R-10 | Sync cannot be replaced within scope | 4 | 4 | **16 High** | 4 | Sync + Product |
| R-11 | Extension store/signing policy blocks the add-on strategy | 4 | 3 | **12 High** | 4 | Product + Legal |
| R-12 | Native platform integration is incomplete | 3 | 3 | **9 Medium** | 5 | Platform |
| R-13 | Installer/update/signing infrastructure slips | 4 | 3 | **12 High** | 5 | Release |
| R-14 | Visual effects and gradients do not match | 3 | 3 | **9 Medium** | 2 | Design + WebUI |
| R-15 | `TabStripModel` cannot support folders/split/glance cleanly | 4 | 3 | **12 High** | 2 | Browser model |
| R-16 | Many-tab performance/memory regresses | 4 | 3 | **12 High** | 2–6 | Browser model + QA |
| R-17 | Parity evidence is incomplete or not trustworthy | 3 | 4 | **12 High** | 1–6 | QA |
| R-18 | Team lacks Chromium build/fork expertise | 4 | 3 | **12 High** | Program | Leadership |
| R-19 | Upstream Chromium API churn breaks product code | 3 | 4 | **12 High** | Ongoing | Architecture |
| R-20 | Trademark/asset/licensing issue blocks release | 4 | 3 | **12 High** | 1–5 | Legal + Design |
| R-21 | Users reject behavioral differences | 3 | 3 | **9 Medium** | 6 | Product |
| R-22 | Custom chrome is inaccessible | 4 | 3 | **12 High** | 2–6 | Accessibility |
| R-23 | Sync/privacy design creates compliance risk | 4 | 3 | **12 High** | 4 | Security + Legal |
| R-24 | CI build times and cost are unsustainable | 3 | 4 | **12 High** | 1–5 | Release |
| R-25 | Dependence on Google services/keys is unacceptable | 3 | 3 | **9 Medium** | 4 | Product + Legal |

## 3. Detailed risks

### R-01 — Workspace/container isolation

**Risk:** Firefox contextual identities provide lightweight cookie/storage/
permission separation that Chromium does not expose directly. A profile-per-
workspace model may be too expensive; a partition model may require Chromium
changes; metadata-only workspaces may be unacceptable.

**Impact:** Cookie leakage, permission leakage, broken site sessions, data-loss
on workspace switch, security review failure, loss of a core product promise.

**Mitigation:** Phase 3 spike comparing profile, storage-partition and metadata
models. Security threat model. Isolation tests for cookies, cache, storage,
permissions, auth and private spaces. Product decision recorded before UI work
depends on it.

**Contingency:** Ship metadata-only workspaces only with explicit product and
security sign-off and a user-facing explanation; do not claim container-level
isolation.

**Trigger:** Spike cannot demonstrate isolation for cookies and storage without
unacceptable profile duplication or a risky Chromium fork.

### R-02 — WebUI chrome performance, accessibility and IME

**Risk:** A WebUI vertical tab strip/sidebar may add startup cost, input latency,
focus bugs, IME issues or accessibility gaps compared with native Views.

**Impact:** The core UI feels slow or broken; accessibility gate fails; fallback
to native Views increases cost and reduces visual parity.

**Mitigation:** Phase 1 prototype with measured startup, input-to-paint, memory
and accessibility budgets. Narrow Mojo API. Native fallback for critical
controls. Keyboard and screen-reader tests. Performance regression harness.

**Contingency:** Use native Views for the affected surface, accepting lower
visual parity for that surface.

**Trigger:** Budgets miss by more than the approved threshold after optimization.

### R-03 — Extension compatibility

**Risk:** Firefox WebExtensions and Chromium extensions differ in APIs, manifest
versions, background models and signing. Zen-specific extension patches do not
carry over.

**Impact:** Popular add-ons fail; users lose workflows; extension-dependent
features break.

**Mitigation:** Compatibility matrix for high-value APIs (`tabs`, `runtime`,
`storage`, `webNavigation`). Shims with least-privilege behavior. Clear
user-facing policy for unsupported Firefox-only APIs. Store/signing decision.

**Contingency:** Support a curated extension set and document unsupported APIs;
do not promise full Firefox add-on compatibility.

### R-04 — Zen Mods and userChrome customization

**Risk:** Zen Mods can alter browser-chrome CSS/JS. Chromium WebUI does not expose
an equivalent privileged customization surface.

**Impact:** Loss of a signature feature; users cannot reproduce themes/mods;
security risk if arbitrary code is allowed.

**Mitigation:** Design a versioned Secant mod package format limited to declared
CSS variables/selectors and reviewed assets. No arbitrary remote JS. Sandboxed
preview. Migration guidance for existing mods.

**Contingency:** Support CSS-only mods and a curated catalog; explicitly
deprecate arbitrary browser-chrome JS mods.

### R-05 — Boosts

**Risk:** Boosts perform per-site style computation and injection using Gecko/
Servo internals. Blink's style pipeline and extension APIs differ.

**Impact:** Boost fidelity drops; existing boosts render differently; editor and
sharing break.

**Mitigation:** Prototype Blink user stylesheet injection and CSSOM editing.
Define supported CSS surface. Visual fixtures per boost. Migration converter for
common patterns.

**Contingency:** Limit Boosts to a documented CSS subset and mark unsupported
rules.

### R-06 — Session and profile migration

**Risk:** Firefox session JSON, `places.sqlite`, container metadata and profile
layout do not map cleanly to Chromium.

**Impact:** Lost tabs, broken workspaces, lost history/bookmarks, user distrust.

**Mitigation:** Versioned session sidecar. Import fixtures for Firefox/Arc/Dia.
Dry-run migration with a report. Backups before import. No silent drops.

**Contingency:** Import only supported data and show a clear report; keep the
source profile untouched.

### R-07 — Fork maintenance

**Risk:** Chromium moves quickly; product patches and `//secant` integration can
break on every release.

**Impact:** Security updates delayed; rebase cost grows; team stuck on an old
Chromium.

**Mitigation:** Pin a stable release branch. Minimal `patches/chromium/` budget.
Nightly rebase check. Upstream-first strategy for generic fixes. Documented
patch owners and expiry. Rebase budget per release.

**Contingency:** Delay a feature release, not a security release; temporarily
disable a non-security product patch.

### R-08 — Security regression

**Risk:** A port, WebUI API or platform integration weakens sandbox, site
isolation, certificate validation, permissions or Safe Browsing.

**Impact:** Critical vulnerability; loss of trust; release blocker.

**Mitigation:** Security review for every `//chrome` patch and privileged WebUI
API. Threat models for workspaces, mods and boosts. Automated isolation and
permission tests. No cert-override patch. Named security reviewer.

**Contingency:** Revert the change; ship without the feature. Security is never
traded for parity.

### R-09 — Codecs and DRM

**Risk:** H.264/AAC/MP3 and Widevine distribution terms may block bundling or
require royalties.

**Impact:** Video/DRM sites fail; distribution blocked in some channels.

**Mitigation:** Legal review; component inventory; optional download or
exclusion; clear user messaging; channel-specific packaging.

**Contingency:** Ship without proprietary codecs/DRM and document limitations.

### R-10 — Sync

**Risk:** Firefox Sync cannot be reused; Chromium Sync may not be available or
acceptable; a new backend is a major service.

**Impact:** No cross-device spaces/tabs; user expectation gap; privacy/compliance
work.

**Mitigation:** Decide scope early. Prefer a minimal, end-to-end encrypted Secant
sync for a defined data set. Threat model and privacy review. If not approved,
defer with a clear product statement.

**Contingency:** Ship without sync in the first release; do not silently depend
on Mozilla or Google services.

### R-11 — Extension store and signing

**Risk:** Chrome Web Store terms, signing policy and review may not fit Secant;
self-hosting has trust and update costs.

**Impact:** Add-ons unavailable or insecure; enterprise adoption blocked.

**Mitigation:** Evaluate store options, signing model and review policy. Support
a curated set and documented sideloading policy. Security review of update
channels.

**Contingency:** Curated store plus signed sideloading; no unsigned production
add-ons.

### R-12 — Native platform integration

**Risk:** Window effects, global shortcuts, share, haptics and Wayland edge-hover
behave differently or are unavailable.

**Impact:** Platform polish gap; compact mode/shortcuts degraded.

**Mitigation:** Platform matrix and per-platform acceptance tests. Portals and
native APIs. Document compositor limitations.

**Contingency:** Platform-specific fallback; document unsupported compositors.

### R-13 — Installer, update and signing

**Risk:** Chromium packaging/update/signing is a new pipeline; macOS
notarization, Windows signing and Linux packaging each have failure modes.

**Impact:** Cannot ship or update securely.

**Mitigation:** Phase 5 release-engineering workstream. Signed test builds early.
Update rollback tests. Key management and CI isolation.

**Contingency:** Delay release; never ship unsigned or non-updatable builds.

### R-14 — Visual effects

**Risk:** Gradients, backdrop filters, vibrancy and animation differ between
Gecko/Servo and Blink/Views.

**Impact:** Zen's visual identity weakens; visual parity gate fails.

**Mitigation:** Token-first design. Visual baseline per platform. Reimplement
effects with CSS/Views/native APIs. Approve documented differences.

**Contingency:** Simplify an effect with design approval; do not fake a
comparison.

### R-15 — Tab model mismatch

**Risk:** Chromium `TabStripModel` assumes a horizontal strip and one
`WebContents` per tab. Folders, split view and glance are product extensions.

**Impact:** Fragile tab code, regressions, difficult upstream merges.

**Mitigation:** Wrap, do not fork, `TabStripModel`. Keep product metadata in
`SecantTabModel`. Prototype split/glance early. Upstream-compatible extension
points.

**Contingency:** Implement split/glance as a separate layout layer over tabs;
avoid changing core tab invariants.

### R-16 — Many-tab performance

**Risk:** WebUI chrome, session metadata and split/glance increase memory and
render cost with hundreds of tabs.

**Impact:** Slow startup/switch; battery and memory regressions.

**Mitigation:** Virtualized tab list. Lazy metadata. Performance budgets and
large-tab-count fixtures. Profiling in Phase 2 and 6.

**Contingency:** Cap expensive previews; degrade gracefully.

### R-17 — Parity evidence integrity

**Risk:** Tests are incomplete, flaky or self-reported; scores become marketing
instead of engineering.

**Impact:** False confidence; release with broken features.

**Mitigation:** Test IDs and evidence links. CI-computed scores. No manual score
edits. Flake quarantine. Phase-gate audits.

**Contingency:** Block release until evidence is reproducible.

### R-18 — Team expertise

**Risk:** Chromium build, Mojo, Views, Blink and release engineering require
specialized skills.

**Impact:** Slow phase gates; key-person dependency; poor upstream hygiene.

**Mitigation:** Phase 1 spike with build/release expert. Documentation. Pairing.
Upstream engagement. Realistic staffing plan.

**Contingency:** Extend schedule or narrow initial platform scope; do not
compress security work.

### R-19 — Upstream API churn

**Risk:** Chromium APIs and build files change frequently.

**Impact:** Rebase breakage; patch drift; delayed security updates.

**Mitigation:** Stable branch pin. Small patch surface. Upstream-first fixes.
Automated rebase checks. Deprecation tracking.

**Contingency:** Carry a temporary patch with an expiry date; prioritize removal.

### R-20 — Trademark, assets and licensing

**Risk:** Zen/Firefox/Chromium marks, fonts, wallpapers, video or codecs are not
licensed for Secant distribution.

**Impact:** Legal takedown; release block; brand confusion.

**Mitigation:** Asset provenance audit. Trademark clearance. Replace unknown
assets. Generate notices/SBOM. Legal review before beta.

**Contingency:** Replace assets or delay release; do not ship unclear licenses.

### R-21 — User rejection of differences

**Risk:** Users expect Zen behavior; unavoidable differences feel like
regressions.

**Impact:** Low adoption; support burden; community backlash.

**Mitigation:** Difference register, release notes, in-product migration report,
feedback loop, prioritize P0/P1 parity.

**Contingency:** Adjust roadmap; do not hide differences.

### R-22 — Accessibility

**Risk:** Custom WebUI chrome lacks keyboard, screen-reader, focus or high-
contrast support.

**Impact:** Excludes users; fails release gate; legal risk.

**Mitigation:** Accessibility requirements in Phase 1. Platform accessibility
tree tests. Keyboard-only flows. Design review. Native fallback where needed.

**Contingency:** Native Views fallback for inaccessible surfaces.

### R-23 — Sync/privacy compliance

**Risk:** Sync, crash reporting, telemetry or migration handle personal data
without proper consent or safeguards.

**Impact:** Privacy violation; regulatory risk; user trust loss.

**Mitigation:** Data inventory, threat model, consent design, retention policy,
encryption, privacy policy, legal review.

**Contingency:** Disable the data flow until approved.

### R-24 — CI cost and build times

**Risk:** Chromium builds are large; a three-platform matrix can be slow and
expensive.

**Impact:** Developer velocity drops; CI budget overrun.

**Mitigation:** Remote caching, incremental builds, split workflows, Blacksmith
runners, nightly full builds, PR smoke builds.

**Contingency:** Reduce PR matrix; keep release matrix complete.

### R-25 — Google service dependency

**Risk:** Safe Browsing, Sync, geolocation or other services depend on Google
terms/keys that may not fit Secant.

**Impact:** Feature removal or legal exposure.

**Mitigation:** Service inventory, own keys or alternatives, feature flags,
privacy review.

**Contingency:** Disable the service and document the limitation.

## 4. Top five for the architecture review

1. **R-01 Workspace/container isolation** — decide the model before building
   workspace UI.
2. **R-02 WebUI chrome budgets** — prove the shell in Phase 1.
3. **R-08 Security regression** — zero tolerance; review every privileged
   surface.
4. **R-07 Fork maintenance** — staff the rebase and patch budget from day one.
5. **R-03 Extension compatibility** — set expectations and policy early.

## 5. Governance

- Every risk has an owner before Phase 1.
- Critical risks require an explicit decision recorded in the migration log.
- Security risks cannot be accepted to ship; they must be fixed or the feature
  removed.
- Risk status changes require evidence, not optimism.
- The register is reviewed monthly and at every phase gate.
- New risks are added with a new ID; old risks are not deleted.
