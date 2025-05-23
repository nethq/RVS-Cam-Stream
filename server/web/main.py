import os,json,sqlite3,time,socket,jwt
from fastapi import FastAPI,Request,Form,Depends,HTTPException,status
from fastapi.responses import RedirectResponse,JSONResponse
from fastapi.templating import Jinja2Templates

JWT_SECRET=os.getenv("JWT_SECRET","secret")
ADMIN_USER=os.getenv("ADMIN_USER","admin")
ADMIN_PASSWORD=os.getenv("ADMIN_PASSWORD","admin")
DEVICE_KEY=os.getenv("DEVICE_KEY","register123")
BASE_UDP=int(os.getenv("BASE_UDP","5001"))

DB=sqlite3.connect("/store.db",check_same_thread=False)
DB.row_factory=sqlite3.Row
controls=json.load(open("control.json"))

app=FastAPI()
templates=Jinja2Templates(directory="templates")

def auth(r:Request):
    if not r.cookies.get("token"):
        raise HTTPException(status_code=status.HTTP_302_FOUND,headers={"Location":"/login"})

@app.get("/login")
def login_page(request:Request):
    return templates.TemplateResponse("login.html",{"request":request})

@app.post("/login")
def login(request:Request,user:str=Form(...),password:str=Form(alias="pass")):
    if user!=ADMIN_USER or password!=ADMIN_PASSWORD:
        return RedirectResponse("/login",302)
    token=jwt.encode({"user":user,"t":int(time.time()*1000)},JWT_SECRET,algorithm="HS256")
    resp=RedirectResponse("/",302)
    resp.set_cookie("token",token,httponly=True)
    return resp

@app.get("/logout")
def logout():
    resp=RedirectResponse("/login",302)
    resp.delete_cookie("token")
    return resp

@app.post("/register")
async def register(request:Request,payload:dict):
    if payload.get("key")!=DEVICE_KEY:
        raise HTTPException(401)
    ip=request.headers.get("x-forwarded-for",request.client.host).split(",")[0].replace("::ffff:","")
    m=DB.execute("SELECT MAX(udp) m FROM cams").fetchone()
    udp=(m["m"] or BASE_UDP-1)+1
    DB.execute("INSERT OR REPLACE INTO cams VALUES(?,?,?,?,?)",
               (payload["name"],udp,payload["ctrl1"],payload["ctrl2"],ip))
    DB.commit()
    return JSONResponse({"udpPort":udp})

@app.post("/cmd")
async def cmd(request:Request,_=Depends(auth)):
    f=await request.form()
    name,f_halt,f_custom,f_ip=f.get("name"),f.get("halt"),f.get("custom"),f.get("ip")
    row=DB.execute("SELECT ctrl1,ctrl2,ip FROM cams WHERE name=?",(name,)).fetchone()
    if not row:
        return RedirectResponse("/",302)
    ports=[row["ctrl1"],row["ctrl2"]]
    target=f_ip or row["ip"]
    def send(cmd:str):
        for p in ports:
            s=socket.socket();s.settimeout(1)
            try:
                s.connect((target,int(p)))
                s.sendall(cmd.encode())
            except:
                pass
            finally:
                s.close()
    if f_custom:
        send(f_custom)
    elif f_halt:
        send("halt=1")
    else:
        for k,v in f.items():
            if k in ("name","halt","custom","ip"):
                continue
            send(f"{k}:{v}")
    return RedirectResponse("/",302)

@app.get("/")
def index(request:Request,_=Depends(auth)):
    cams=[dict(r) for r in DB.execute("SELECT * FROM cams").fetchall()]
    token=jwt.encode({"user":"", "t":int(time.time()*1000)},JWT_SECRET,algorithm="HS256")
    host=request.url.hostname
    return templates.TemplateResponse("index.html",{"request":request,"cams":cams,"controls":controls,"host":host,"tok":token})
