// File: web/server.js
import fs from 'fs'
import express from 'express'
import bp from 'body-parser'
import cookie from 'cookie-parser'
import jwt from 'jsonwebtoken'
import sqlite3 from 'sqlite3'
import net from 'net'
import layouts from 'express-ejs-layouts'

const app = express()
app.use(bp.json())
app.use(bp.urlencoded({ extended: false }))
app.use(cookie())
app.use(layouts)
app.set('view engine', 'ejs')
app.set('layout', 'layout')
app.set('views', './views')

const DB = new sqlite3.Database('/store.db')
const controls = JSON.parse(fs.readFileSync('./control.json'))
const { JWT_SECRET, DEVICE_KEY, BASE_UDP } = process.env

const ensureAuth = (q, s, n) => { if (!q.cookies.token) return s.redirect('/login'); n() }
const nextUDP = cb => DB.get('SELECT MAX(udp) m FROM cams', (_, r) => cb((r?.m || BASE_UDP - 1) + 1))

app.post('/login', (q, s) =>
  fetch('http://localhost:3000/login', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(q.body)
  })
    .then(r => r.json())
    .then(j => { s.cookie('token', j.token, { httpOnly: true }); s.redirect('/') })
    .catch(() => s.redirect('/login'))
)
app.get('/login', (_q, s) => s.render('login'))

app.get('/logout', ensureAuth, (q, s) => { s.clearCookie('token'); s.redirect('/login') })

app.post('/register', (q, s) => {
  const { key, name, ctrl1, ctrl2 } = q.body
  if (key !== DEVICE_KEY) return s.sendStatus(401)
  const ip = q.headers['x-forwarded-for']?.split(',')[0] || q.socket.remoteAddress.replace('::ffff:', '')
  nextUDP(u => DB.run(
    'INSERT OR REPLACE INTO cams VALUES(?,?,?,?,?)',
    [name, u, ctrl1, ctrl2, ip],
    () => s.json({ udpPort: u })
  ))
  console.log("Recieved connection from device -", key, name, ctrl1, ctrl2, ip);
})

app.post('/cmd', ensureAuth, (q, s) => {
  const { name, halt, custom, ip } = q.body

  DB.get('SELECT ctrl1,ctrl2,ip FROM cams WHERE name=?', [name], (_, row) => {
    if (!row) return s.redirect('/')
    const ports = [row.ctrl1, row.ctrl2]
    // if client provided an ip override, use it; otherwise use stored IP
    const targetIp = ip || row.ip

    if (custom) {
      ports.forEach(p => {
        const sock = new net.Socket()
        sock.connect(Number(p), targetIp, () => { sock.write(custom); sock.end() })
          .on('error', () => { })
      })
      return s.redirect('/')
    }

    if (halt) {
      ports.forEach(p => {
        const sock = new net.Socket()
        sock.connect(Number(p), targetIp, () => { sock.write('halt=1'); sock.end() })
          .on('error', () => { })
      })
    } else {
      Object.entries(q.body)
        .filter(([k]) => !['name', 'halt', 'custom', 'ip'].includes(k))
        .forEach(([k, v]) => {
          ports.forEach(p => {
            const sock = new net.Socket()
            sock.connect(Number(p), targetIp, () => { sock.write(`${k}:${v}`); sock.end() })
              .on('error', () => { })
          })
        })
    }

    s.redirect('/')
  })
})

app.get('/', ensureAuth, (_q, s) =>
  DB.all('SELECT * FROM cams', (_, c) => s.render('index', {
    cams: c, controls, host: _q.hostname, tok: jwt.sign({ t: Date.now() }, JWT_SECRET)
  }))
)

app.listen(4000, () => console.log('[web] ui 4000'))
