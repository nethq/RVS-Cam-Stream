import os,subprocess,sqlite3,signal,time,threading,sys
MEDIA_HOST=os.getenv("MEDIA_HOST","media")
INGRESS=os.getenv("INGRESS","UDP").upper() #srt capability
DB=sqlite3.connect("/data/store.db",check_same_thread=False)
DB.row_factory=sqlite3.Row
REFRESH=5
procs={}
def pump(n,p):
    for l in p.stdout:sys.stdout.write(f"[{n}] {l}")
def src(udp):
    if INGRESS=="UDP":
        return f"udp://0.0.0.0:{udp}?fifo_size=1000000&overrun_nonfatal=1&timeout=100000"
    return f"srt://0.0.0.0:{udp}?mode=listener&latency=200000"
def start(r):
    n,u=r["name"],r["udp"]
    if n in procs:return
    cmd=["ffmpeg","-hide_banner","-loglevel","info","-stats","-i",src(u),"-vf","showinfo","-c:v","copy","-an","-f","flv",f"rtmp://{MEDIA_HOST}:1935/{n}"]
    sys.stdout.write(f"[bridge] {' '.join(cmd)}\n")
    p=subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
    threading.Thread(target=pump,args=(n,p),daemon=True).start()
    procs[n]=p
def sync():
    rows=DB.execute("SELECT name,udp FROM cams").fetchall()
    names={r["name"] for r in rows}
    for r in rows:start(r)
    for n,p in list(procs.items()):
        if n not in names or p.poll() is not None:
            p.terminate();procs.pop(n)
def stop(*_):
    for p in procs.values():p.terminate()
    sys.exit(0)
signal.signal(signal.SIGINT,stop)
signal.signal(signal.SIGTERM,stop)
while True:
    sync()
    time.sleep(REFRESH)
