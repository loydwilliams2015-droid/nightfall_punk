#!/usr/bin/env python3
"""Authenticated live-owner smoke; preserve logs even on failure."""
import argparse
import os
from pathlib import Path
import re
import subprocess
import time

p = argparse.ArgumentParser()
p.add_argument('--build', default='build/full')
p.add_argument('--evidence', default='build/b2-resume-evidence')
args = p.parse_args()
root = Path(args.evidence)
root.mkdir(parents=True, exist_ok=True)
for model in (3, 4):
    server_log = root / f'network-M{model}-server.log'
    client_log = root / f'network-M{model}-client.log'
    with server_log.open('w') as sf, client_log.open('w') as cf:
        server = subprocess.Popen([str(Path(args.build) / 'nightfall_server'),
            '--b2-encounter-proof', '--port', str(27778 + model),
            '--duration', '6', '--b2-model', str(model)], stdout=sf, stderr=subprocess.STDOUT)
        try:
            # Wait for the flushed readiness line, not a guessed long sleep.
            deadline = time.monotonic() + 5
            while 'scope=single-physical-actor' not in server_log.read_text():
                if server.poll() is not None or time.monotonic() > deadline:
                    raise RuntimeError('server failed to become ready')
                time.sleep(.05)
            client = subprocess.run([str(Path(args.build) / 'nightfall_v18b2_netbot'),
                '--port', str(27778 + model), '--duration', '4'], stdout=cf,
                stderr=subprocess.STDOUT, timeout=10)
            assert client.returncode == 0, client_log.read_text()
            assert server.wait(timeout=10) == 0, server_log.read_text()
        finally:
            if server.poll() is None:
                server.terminate()
                server.wait(timeout=5)
    log = server_log.read_text()
    m = re.search(r'proposals=(\d+) grants=(\d+) duplicate_packets=(\d+).*'
                  r'actual_A4_receipts=(\d+) history_samples=(\d+) balance=(\d+)', log)
    assert m, log
    proposals, grants, duplicates, receipts, history, balance = map(int, m.groups())
    assert (proposals, grants, balance) == (1, 1, 9), log
    assert duplicates > 0 and receipts > 0 and history > 0, log
    assert 'state=COMPENSATION_REQUIRED published=1 applied_receipts=1' in log, log
    print(f'M{model}: PASS authenticated one grant, one debit, replay refusal, real contact/history, interruption')
