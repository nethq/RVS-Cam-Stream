import express from 'express';
import bp from 'body-parser';
import bcrypt from 'bcrypt';
import jwt from 'jsonwebtoken';
import sqlite3 from 'sqlite3';

const { JWT_SECRET, ADMIN_PASSWORD } = process.env;
const db = new sqlite3.Database(':memory:');
db.serialize(() => { db.run('CREATE TABLE u(n,p)'); db.run('INSERT INTO u VALUES(?,?)', ['admin', bcrypt.hashSync(ADMIN_PASSWORD, 10)]); });
const app = express(); app.use(bp.json());
app.post('/login', (q, s) => {
  const { user, pass } = q.body || {};
  db.get('SELECT p FROM u WHERE n=?', [user], (_, r) =>
    !r || !bcrypt.compareSync(pass, r.p) ? s.sendStatus(401) :
      s.json({ token: jwt.sign({ u: user }, JWT_SECRET) }));
});
app.listen(3000, () => console.log('[auth] ready'));
