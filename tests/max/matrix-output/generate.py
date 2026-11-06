from pathlib import Path
import json
import argparse
import struct
parser=argparse.ArgumentParser(description="Generate isolated Max matrix-output acceptance patches")
parser.add_argument("output",type=Path)
args=parser.parse_args()
root=args.output.resolve()
root.mkdir(parents=True,exist_ok=True)
repo=Path(__file__).resolve().parents[3]
if any(root.glob("*/events.json")):
 raise SystemExit("Use a fresh output directory to preserve previous evidence")
for name,planes,width,height,port in [('one-d',1,5,1,59481),('two-d',3,3,2,59482)]:
 d=root/name; d.mkdir(exist_ok=True)
 for cls in ['schema','recv','parse']:
  p=d/f'bbb.zmq.{cls}.mxo'
  if not p.exists(): p.symlink_to(repo/f'externals/bbb.zmq.{cls}.mxo',target_is_directory=True)
 # Also make the shared runtime available next to the symlink bundles.
 p=d/'libbbb_zmq_runtime.dylib'
 if not p.exists(): p.symlink_to(repo/'externals/libbbb_zmq_runtime.dylib')
 schema='qa_matrix_'+name.replace('-','_')
 (d/'schema.zmqdsl').write_text(f'''schema {schema} {{
 endian little;
 u8 width;
 u8 height;
 matrix u8:{planes}[width][height] pixels;
 emit image pixels@matrix;
}}\n''')
 config=dict(case=name,planes=planes,width=width,height=height,port=port,directory=str(d))
 (d/'config.json').write_text(json.dumps(config,indent=2))
 data=[11,22,33,44,55] if name=='one-d' else list(range(1,19))
 (d/'valid.bin').write_bytes(bytes([width,height]+data))
 (d/'short.bin').write_bytes(bytes([width,height,99]))
 (d/'zero-width.bin').write_bytes(bytes([0,height]))
 if name=='two-d':
  (d/'grow.bin').write_bytes(bytes([5,3]+list(range(1,46))))
  (d/'shrink.bin').write_bytes(bytes([2,1]+[21,22,23,24,25,26]))
  for typ,planes_,filename in [('f32',1,'float'),('u16',1,'unsupported')]:
   (d/(filename+'.zmqdsl')).write_text(f"schema {schema} {{ endian little; u8 width; u8 height; matrix {typ}:{planes_}[width][height] pixels; emit image pixels@matrix; }}\n")
  (d/'float.bin').write_bytes(bytes([3,2])+struct.pack('<6f',1.25,-2.5,3.75,4.5,5.25,6.0))
  (d/'unsupported.bin').write_bytes(bytes([3,1])+struct.pack('<3H',100,200,300))
 code='''autowatch = 0;
inlets = 1; outlets = 1;
var config = CONFIG;
var events = [];
var lastCommand = -1;
var poll = new Task(control, this);
function record(kind, value) {
    events.push({time: new Date().toISOString(), kind: kind, value: value});
    var f = new File(config.directory + "/events.json", "write");
    if (!f.isopen) { error("QA log could not open"); return; }
    f.eof = 0;
    f.writestring(JSON.stringify({config:config, events:events}, null, 2));
    f.close();
}
function inspectMatrix(name, kind) {
    var m = new JitterMatrix(name);
    var dims = m.dim;
    var count = m.planecount;
    for (var i=0; i<dims.length; i++) count *= dims[i];
    var info = {name:name, dim:dims, planecount:m.planecount, type:m.type, dimstride:m.dimstride, size:m.size};
    record(kind + "_metadata", info);
    if (count > 65536 || count < 1) { record("unexpected_matrix_size",count); return; }
    var a;
    if (m.type === "char") a = new Uint8Array(count);
    else if (m.type === "long") a = new Int32Array(count);
    else if (m.type === "float32") a = new Float32Array(count);
    else if (m.type === "float64") a = new Float64Array(count);
    else { record("unexpected_matrix_type",m.type); return; }
    m.copymatrixtoarray(a);
    record(kind + "_data", Array.from(a));
}
function start() {
    record("started",config.case);
    // Positive control: prove this observer can read an actual Jitter matrix.
    var probe = new JitterMatrix(1,"char",[3,2]);
    probe.copyarraytomatrix(new Uint8Array([11,22,33,44,55,66]));
    inspectMatrix(probe.name,"observer_control");
    this.patcher.getnamed("schema").message("read",config.directory + "/schema.zmqdsl");
    this.patcher.getnamed("parse").message("packet",2147483646);
    this.patcher.getnamed("recv").message("start");
    record("ready",config.port);
    poll.interval = 250;
    poll.repeat();
}
function control() {
    var f = new File(config.directory + "/control.json","read");
    if (!f.isopen) return;
    var s=f.readstring(f.eof); f.close();
    try {
        var c=JSON.parse(s);
        if(c.id===lastCommand) return;
        lastCommand=c.id;
        record("command",c);
        if(c.action==="schema") {
            this.patcher.getnamed("schema").message("read",config.directory + "/" + c.file);
        }
        if(c.action==="stop") {
            this.patcher.getnamed("recv").message("stop");
            record("stopped",true);
            poll.cancel();
            outlet(0,"wclose");
        }
    } catch(e) { record("control_error",String(e)); }
}
function received() { record("received",arrayfromargs(arguments)); }
function parsed() {
    var a=arrayfromargs(arguments);
    record("parsed",a);
    if(a[0]==="jit_matrix") {
        try { inspectMatrix(a[1],"parsed_matrix"); }
        catch(e) { record("observer_error",String(e)); }
    }
}
function diagnostic() { record("diagnostic",arrayfromargs(arguments)); }
function schema_status() { record("schema_status",arrayfromargs(arguments)); }
function schema_error() { record("schema_error",arrayfromargs(arguments)); }
function notifydeleted() { poll.cancel(); record("patch_deleted",true); }
'''.replace('CONFIG',json.dumps(config))
 (d/'observe.js').write_text(code)
 boxes=[];lines=[]
 def box(i,text,x,y,var=None):
  b=dict(id=i,maxclass='newobj',text=text,patching_rect=[x,y,400,22])
  if var:b['varname']=var
  boxes.append({'box':b})
 def line(a,ao,b,bi=0,order=None):
  q=dict(source=[a,ao],destination=[b,bi])
  if order is not None:q['order']=order
  lines.append({'patchline':q})
 box('load','loadbang',20,20); box('delay','delay 1500',20,50);box('start','prepend start',20,80)
 box('observer','v8 observe.js',20,400)
 box('schema','bbb.zmq.schema',20,130,'schema')
 box('recv',f'bbb.zmq.recv @type pull @bind 1 @endpoint tcp://127.0.0.1:{port}',20,190,'recv')
 box('parse',f'bbb.zmq.parse {schema}',20,250,'parse')
 for i,text,y in [('ss','prepend schema_status',130),('se','prepend schema_error',160),('rx','prepend received',190),('po','prepend parsed',250),('pe','prepend diagnostic',280)]:
  box(i,text,470,y);line(i,0,'observer')
 line('load',0,'delay');line('delay',0,'start');line('start',0,'observer')
 line('schema',0,'ss');line('schema',1,'se')
 line('recv',0,'rx',order=0);line('recv',0,'parse',order=1)
 line('parse',0,'po');line('parse',1,'pe')
 box('defer','deferlow',20,440);box('this','thispatcher',20,470)
 line('observer',0,'defer');line('defer',0,'this')
 (d/f'matrix-{name}.maxpat').write_text(json.dumps({'patcher':dict(fileversion=1,appversion=dict(major=9,minor=1,revision=5,architecture='x64',modernui=1),rect=[80,80,950,560],boxes=boxes,lines=lines)},indent=2))
