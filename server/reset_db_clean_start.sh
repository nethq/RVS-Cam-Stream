sqlite3 store.db <<'SQL'
PRAGMA journal_mode = DELETE;
CREATE TABLE IF NOT EXISTS cams (
  name  TEXT PRIMARY KEY,
  udp   INT  UNIQUE,
  ctrl1 INT,
  ctrl2 INT,
  ip    TEXT
);
SQL

docker compose up --build