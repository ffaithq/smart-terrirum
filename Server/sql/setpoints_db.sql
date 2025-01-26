-- 1. Create your table
CREATE TABLE IF NOT EXISTS setpoints (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    waterlevel REAL NOT NULL,
    uvlevel REAL NOT NULL,
    image_filename TEXT NOT NULL,
    active BOOLEAN NOT NULL DEFAULT FALSE
);

-- 2. Create a partial unique index for the 'active' column
--    This says: "Among rows where active=TRUE, the value of active
--    must be unique." Since active is either TRUE or FALSE, this effectively
--    means only one row at a time can have active=TRUE.
CREATE UNIQUE INDEX IF NOT EXISTS idx_one_active
    ON setpoints (active)
    WHERE active = 1;  -- or active = TRUE (SQLite treats TRUE as 1)
