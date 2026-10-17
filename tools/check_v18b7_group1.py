#!/usr/bin/env python3
"""Baseline-only reproduction; does not certify C01-C12 or release readiness."""
import json, os, pathlib, platform, shutil, subprocess, tempfile
out = pathlib.Path('docs/evidence/1.8B7-G1')
out.mkdir(parents=True, exist_ok=True)
rows = []
with tempfile.TemporaryDirectory() as tmp:
    for cc in ['gcc', 'clang']:
        if not shutil.which(cc):
            rows.append(dict(compiler=cc, status='BLOCKED: compiler unavailable'))
            continue
        for name, src in [('interact18b3','interact18b3'), ('hf18b4','hf18b4'), ('persist18b5','persist18b5'), ('corpus18b5','persist18b5')]:
            exe = str(pathlib.Path(tmp) / (cc + '-' + name))
            cmd = [cc, '-std=c11', '-O1', '-g', '-Wall', '-Wextra', '-Wpedantic', '-Werror', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-Isrc/shared', 'src/shared/nf_'+src+'.c', 'src/tests/test_'+name+'.c', '-o', exe]
            build = subprocess.run(cmd, text=True, capture_output=True)
            run = subprocess.run([exe], text=True, capture_output=True) if build.returncode == 0 else None
            rows.append(dict(command=cmd, compile_exit=build.returncode, compile_output=build.stdout+build.stderr, run_exit=run.returncode if run else None, run_output=run.stdout+run.stderr if run else '', compiler=subprocess.check_output([cc, '--version'], text=True).splitlines()[0]))
result = dict(source_sha=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip(), platform=platform.platform(), asan_options=os.environ.get("ASAN_OPTIONS", ""), runs=rows)
(out / 'baseline-tests.json').write_text(json.dumps(result, indent=2)+'\n')
print(json.dumps(result, indent=2))
