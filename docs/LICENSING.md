# nightfall!punk licensing policy

**Chosen primary software license:** GNU General Public License, version 3 **or any later version** (`SPDX-License-Identifier: GPL-3.0-or-later`).

The complete text of GPL version 3 is preserved, unmodified, in the repository root [LICENSE](../LICENSE). The **or later** grant is the repository's explicit licensing choice for project-owned original software; it does **not** grant us ownership of external contributions or third-party material.

## Scope

- **Original project source code, related original build scripts, and software tools:** GPL-3.0-or-later, to the extent licensed by their respective copyright holders to the project. Where source files are newly created or meaningfully edited, add `SPDX-License-Identifier: GPL-3.0-or-later` and accurate copyright attribution if known; do not invent author names.
- **Externally obtained dependencies:** retain each upstream copyright and license. Raylib 5.5, ENet and libsodium have their own upstream licenses. The project root license does not replace them. Their bundled or system-provided distribution obligations remain separate.
- **Original creative assets, fonts, recordings, music, logos and branding:** do not automatically claim author rights or apply GPL through a global notice. Record provenance and obtain the copyright holder's permission; select an explicit asset license before public distribution. Trademark and brand identity are separate from copyright permissions.
- **Documentation, screenshots, research datasets and user-submitted observations:** check authorship, consent, embedded third-party material and appropriate explicit licensing. No license or permission for a person's testimony is implied by software licensing.
- **Historical code:** copyright and provenance are not retroactively established by adding this license. Legacy files that include other authors' work need an audit; preserve all existing license notices and do not silently overwrite third-party terms.

## Contributor policy (pending governance approval)

Contributors retain their own copyright unless a separate written agreement says otherwise. Contributions intended for incorporation into GPL-covered code must be offered under `GPL-3.0-or-later` or terms compatible with that coverage, with appropriate permission to contribute. No Contributor License Agreement (CLA), copyright transfer, or Developer Certificate of Origin (DCO) commitment has been assumed by this file. Decide and document the intake process before accepting external contributions at scale.

## Binary distribution and compliance

When distributing binaries, include the GPL license, applicable copyright notices, third-party notices and the **complete corresponding source code** or a legally sufficient source-access mechanism for the exact distributed build, including the build scripts and required non-system linked components. Do not imply that supplying a GitHub branch name, a README sentence or a library's license alone completes GPLv3 section 6. Preserve source SHA, build version, build configuration, and checksum provenance.

A clean source-exact CI pass certifies a tested configuration; it does not automatically certify contributor title, license compatibility, external asset rights, or release eligibility.

## Explicit open determinations before a public playtest

1. Confirm which individual(s) or legal entity/entities own or have permission to license original and historical project code. Git commit authorship alone is not a legal copyright assignment.
2. Audit any external or copied code, assets, audio, fonts and generated content; record permission, license and attribution for each exception.
3. Establish contributor intake/licensing and accurate copyright notices; specify license of separable assets/documentation when needed.
4. For the distributed VPC-01 candidate, verify an exact GPL-compliant corresponding-source package and actual dependency licenses.
5. Keep the engineering gate for **authoritative A6 two-actor / B2 / A5 / B3 physical-contractual integration, durability, concurrency and actual player-facing playtest** independent of licensing.

**Decision status:** GPL-3.0-or-later is chosen and committed. The items above are provenance/compliance and engineering verification tasks, not license alternatives.
