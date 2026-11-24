# RET-C H319: diagnostic evidence-schema gate

This bounded support cut adds an evidence-schema validator and synthetic regressions for the *future* dual actor→object PUSH geometry test.

Source inputs must come from actual A6-controlled source instrumentation. Require distinct actors 10 and 20, object 2010, genuine nonzero actor→object A4 contact IDs and impulses, positive independently agreed oracle result, identical owner tick/version, same A5 material epoch, both A5 FREE, body clearance and visibility. A reported value is **not proof of authenticity**. Source SHA/frame hash fields are provenance labels, not cryptographic attestation. The gate must not issue B2 grants, realize B3 effects, mutate source scenes, or publish A6.

Run: `python3 -m unittest discover -s tests -p 'test_retc_h319_evidence_gate.py' -v`
With source-derived JSON: `python3 tools/retc_h319_evidence_gate.py /path/to/evidence.json`

**Acceptance:** schema-level diagnostic only. The actual `src/tests/test_retc_h319_push_geometry.c`, independent source oracle, GCC/Clang C11 source-level exact-SHA CI and fresh owner live grants are still OPEN. H319, H320–H324, M18C.2/C3, and release stay HOLD. No native playability metrics established. Keep this branch DRAFT; never merge on diagnostic tests alone.
