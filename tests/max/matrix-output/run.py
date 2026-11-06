"""Run the generated patches in separate Max instances; never edit user patches."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser()
parser.add_argument('directory', type=Path)
parser.add_argument('--sender', required=True, type=Path)
args = parser.parse_args()
root = args.directory.resolve()
if any(root.glob("*/events.json")) or any(root.glob("*/control.json")):
    parser.error("Use a fresh generated directory; previous run evidence must not be overwritten")
repo = Path(__file__).resolve().parents[3]
results = []
all_events = {}
loaded = {}

def wait_for(read, predicate, description, timeout=12):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        value = read()
        if predicate(value):
            return value
        time.sleep(0.1)
    raise RuntimeError('timeout: ' + description)

def pids():
    p = subprocess.run(['pgrep', '-f', '^/Applications/Max.app/Contents/MacOS/Max$'], capture_output=True, text=True)
    return set(p.stdout.split())

for name in ('one-d', 'two-d'):
    directory = root / name
    config = json.loads((directory / 'config.json').read_text())
    counter = 0
    def events():
        try:
            return json.loads((directory / 'events.json').read_text())['events']
        except (FileNotFoundError, json.JSONDecodeError):
            return []
    def command(action, **extra):
        global counter
        counter += 1
        c = dict(id=counter, action=action, **extra)
        temporary = directory / 'control.tmp'
        temporary.write_text(json.dumps(c))
        temporary.replace(directory / 'control.json')
        ev = wait_for(events, lambda ev: any(e['kind']=='command' and e['value']==c for e in ev), action)
        index = next(i for i,e in enumerate(ev) if e['kind']=='command' and e['value']==c)
        if action == 'schema':
            wait_for(events, lambda ev: any(e['kind']=='schema_status' and e['value'][0]=='compiled' for e in ev[index:]), 'schema recompile')
        return index
    def exercise(label, payload, expected=None, diagnostic=None):
        start = command('mark', label=label)
        subprocess.run([str(args.sender.resolve()), f'tcp://127.0.0.1:{config["port"]}', str(directory/payload)], check=True)
        if diagnostic:
            ev = wait_for(events, lambda ev: any(e['kind']=='diagnostic' and diagnostic in str(e['value']) for e in ev[start:]), label)
            assert not any(e['kind']=='parsed_matrix_data' for e in ev[start:]), 'invalid input emitted a matrix'
            result = dict(case=name, step=label, status='PASS', diagnostics=[e['value'] for e in ev[start:] if e['kind']=='diagnostic'])
        else:
            ev = wait_for(events, lambda ev: any(e['kind']=='parsed_matrix_data' for e in ev[start:]), label)
            subset = ev[start:]
            assert not any(e['kind']=='diagnostic' for e in subset), 'unexpected diagnostic'
            metadata = [e['value'] for e in subset if e['kind']=='parsed_matrix_metadata']
            data = [e['value'] for e in subset if e['kind']=='parsed_matrix_data']
            assert len(metadata)==len(data)==1, 'expected exactly one output matrix'
            info = metadata[0]
            for key in ('dim', 'type', 'planecount'):
                assert info[key]==expected[key], (key, info[key], expected[key])
            assert data[0]==expected['data'], (data, expected['data'])
            cell_bytes = info['planecount'] * (4 if info['type']=='float32' else 1)
            row_bytes = cell_bytes * info['dim'][0]
            assert info['dimstride'][0]==cell_bytes
            assert info['dimstride'][1]>=row_bytes and info['dimstride'][1]>0
            assert info['size'] >= info['dimstride'][1]*(info['dim'][1]-1)+row_bytes
            result = dict(case=name,step=label,status='PASS',metadata=info,data=data[0])
        results.append(result)
        print(name, label, 'PASS', flush=True)
    before = pids()
    subprocess.run(['open','-n','-a','/Applications/Max.app',str(directory/f'matrix-{name}.maxpat')], check=True)
    try:
        ev = wait_for(events, lambda ev:any(e['kind']=='ready' for e in ev), name+' startup', 30)
        assert [e['value'] for e in ev if e['kind']=='observer_control_data']==[[11,22,33,44,55,66]]
        assert any(e['kind']=='diagnostic' and 'packet_not_found' in e['value'] for e in ev)
        new = pids()-before
        assert len(new)==1, ('cannot identify dedicated Max process',new)
        pid = next(iter(new))
        mappings = subprocess.run(['lsof','-p',pid,'-Fn'],check=True,capture_output=True,text=True).stdout
        binary = str(repo/'externals/bbb.zmq.parse.mxo/Contents/MacOS/bbb.zmq.parse')
        assert 'n'+binary in mappings, 'unexpected parse build loaded'
        loaded[name] = dict(pid=int(pid),parse_binary=binary)
        exercise('truncated','short.bin',diagnostic='out of bounds')
        exercise('zero-width','zero-width.bin',diagnostic='dims must be > 0')
        expected = dict(dim=[config['width'],config['height']],planecount=config['planes'],type='char',data=([11,22,33,44,55] if name=='one-d' else list(range(1,19))))
        exercise('valid','valid.bin',expected)
        if name=='two-d':
            exercise('grow','grow.bin',dict(dim=[5,3],planecount=3,type='char',data=list(range(1,46))))
            exercise('shrink','shrink.bin',dict(dim=[2,1],planecount=3,type='char',data=[21,22,23,24,25,26]))
            command('schema',file='float.zmqdsl')
            exercise('float-type-change','float.bin',dict(dim=[3,2],planecount=1,type='float32',data=[1.25,-2.5,3.75,4.5,5.25,6.0]))
            command('schema',file='unsupported.zmqdsl')
            exercise('unsupported-integer-width','unsupported.bin',diagnostic='matrix_invalid_layout')
            command('schema',file='schema.zmqdsl')
            exercise('recover-char','valid.bin',expected)
        os.kill(int(pid),0)  # existence check only; does not send a signal
    except Exception as exc:
        results.append(dict(case=name,status='FAIL',error=str(exc)))
        print(name,'FAIL',str(exc),flush=True)
    finally:
        try:
            command('stop')
            wait_for(events, lambda ev:any(e['kind']=='patch_deleted' for e in ev), 'patch closure')
            results.append(dict(case=name,step='stop-and-close',status='PASS'))
        except Exception as exc:
            results.append(dict(case=name,step='stop-and-close',status='FAIL',error=str(exc)))
        all_events[name] = events()
        (root/'results.json').write_text(json.dumps(dict(loaded=loaded,results=results),indent=2))
        (root/'events.json').write_text(json.dumps(all_events,indent=2))

raise SystemExit(0 if all(r['status']=='PASS' for r in results) else 1)
