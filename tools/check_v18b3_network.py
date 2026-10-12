#!/usr/bin/env python3
"""Actual authenticated B3 server/transport proof; no human/UI inference."""
import argparse
from pathlib import Path
import re
import subprocess
import time

p = argparse.ArgumentParser()
p.add_argument('--build', default='build/full')
p.add_argument('--evidence', default='build/b3-evidence')
args = p.parse_args()
root = Path(args.evidence)
root.mkdir(parents=True, exist_ok=True)
for model in (3, 4):
    for kind in range(7):
        server_log = root / f'network-M{model}-kind{kind}-server.log'
        client_log = root / f'network-M{model}-kind{kind}-client.log'
        port = 27800 + model * 10 + kind
        with server_log.open('w') as sf, client_log.open('w') as cf:
            server = subprocess.Popen([str(Path(args.build) / 'nightfall_server'),
                '--b3-physical-proof', '--port', str(port), '--duration', '3',
                '--b2-model', str(model), '--b3-kind', str(kind)], stdout=sf, stderr=subprocess.STDOUT)
            try:
                deadline = time.monotonic() + 5
                while 'scope=single-physical-actor' not in server_log.read_text():
                    if server.poll() is not None or time.monotonic() > deadline:
                        raise RuntimeError('server not ready: ' + server_log.read_text())
                    time.sleep(.05)
                result = subprocess.run([str(Path(args.build) / 'nightfall_v18b2_netbot'),
                    '--port', str(port), '--duration', '2', '--b3-kind', str(kind)],
                    stdout=cf, stderr=subprocess.STDOUT, timeout=10)
                assert result.returncode == 0, client_log.read_text()
                assert server.wait(timeout=10) == 0, server_log.read_text()
            finally:
                if server.poll() is None:
                    server.terminate()
                    server.wait(timeout=5)
        log = server_log.read_text()
        assert 'proposals=1 grants=1' in log, log
        duplicates = re.search(r'duplicate_packets=(\d+)', log)
        assert duplicates and int(duplicates[1]) > 0, log
        summary = re.search(r'\[b3-summary\].*state=COMPLETED motor_steps=(\d+) contacts=(\d+).*'
            r'inventory=(\d+) health=(\d+) salvage=(\d+) balance=(\d+) slots=0 carried_by=0', log)
        assert summary, log
        motors, contacts, inventory, health, salvage, tokens = map(int, summary.groups())
        assert (inventory, health, salvage, tokens) == ((1, 0, 8, 10) if kind == 2 else
            (0, 3, 5, 7) if kind == 5 else (0, 0, 8, 10)), log
        if kind in (3, 4):
            assert contacts > 0, log
        elif kind != 2:
            assert motors > 0, log
        print(f'M{model} kind={kind}: PASS one grant, realized effect, balanced resources, released slot', flush=True)
