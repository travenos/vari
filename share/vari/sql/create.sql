CREATE TABLE IF NOT EXISTS cloth (
    "id"	INTEGER PRIMARY KEY AUTOINCREMENT,
    "name"	TEXT UNIQUE NOT NULL,
    "cavityheight"	REAL NOT NULL,
    "permeability"	REAL NOT NULL,
    "porosity"	REAL NOT NULL
    );
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('Пример наполнителя 1', 0.002, 2e-9, 0.5);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('Пример наполнителя 2', 0.004, 5e-9, 0.3);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('Пример наполнителя 3', 0.001, 1e-9, 0.4);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('Пример наполнителя 4', 0.003, 4e-9, 0.6);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('Пример наполнителя 5', 0.002, 3e-9, 0.2);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('Катализатор', 0.002, 2e-8, 0.8);
CREATE TABLE IF NOT EXISTS resin (
    "id"	INTEGER PRIMARY KEY AUTOINCREMENT,
    "name"	TEXT UNIQUE NOT NULL,
    "viscosity"	REAL NOT NULL,
    "visctempcoef"	REAL NOT NULL DEFAULT 90000,
    "lifetime"	REAL NOT NULL DEFAULT 10800,
    "lifetimetempcoef"	REAL NOT NULL DEFAULT 90000
    );
INSERT INTO resin (name, viscosity, visctempcoef, lifetime, lifetimetempcoef) VALUES ('Пример связующего 1', 0.1, 90000, 10800, 90000);
INSERT INTO resin (name, viscosity, visctempcoef, lifetime, lifetimetempcoef) VALUES ('Пример связующего 2', 0.1, 90000, 5400, 90000);
INSERT INTO resin (name, viscosity, visctempcoef, lifetime, lifetimetempcoef) VALUES ('Пример связующего 3', 0.1, 90000, 3600, 90000);
