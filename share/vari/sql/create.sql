CREATE TABLE IF NOT EXISTS cloth_a (
    "id"	INTEGER PRIMARY KEY AUTOINCREMENT,
    "name"	TEXT UNIQUE NOT NULL,
    "cavityheight"	REAL NOT NULL,
    "xpermeability"	REAL NOT NULL,
    "ypermeability"	REAL NOT NULL,
    "porosity"	REAL NOT NULL
    );
INSERT INTO cloth_a (name, cavityheight, xpermeability, ypermeability, porosity) VALUES ('Пример наполнителя 1', 0.002, 2e-9, 3e-9, 0.5);
INSERT INTO cloth_a (name, cavityheight, xpermeability, ypermeability, porosity) VALUES ('Пример наполнителя 2', 0.004, 5e-9, 6e-9, 0.3);
INSERT INTO cloth_a (name, cavityheight, xpermeability, ypermeability, porosity) VALUES ('Пример наполнителя 3', 0.001, 1e-9, 1e-9, 0.4);
INSERT INTO cloth_a (name, cavityheight, xpermeability, ypermeability, porosity) VALUES ('Пример наполнителя 4', 0.003, 4e-9, 5e-9, 0.6);
INSERT INTO cloth_a (name, cavityheight, xpermeability, ypermeability, porosity) VALUES ('Пример наполнителя 5', 0.002, 3e-9, 4e-9, 0.2);
INSERT INTO cloth_a (name, cavityheight, xpermeability, ypermeability, porosity) VALUES ('Катализатор', 0.002, 2e-8, 2e-8, 0.8);
CREATE TABLE IF NOT EXISTS resin (
    "id"	INTEGER PRIMARY KEY AUTOINCREMENT,
    "name"	TEXT UNIQUE NOT NULL,
    "viscosity"	REAL NOT NULL,
    "visctempcoef"	REAL NOT NULL,
    "lifetime"	REAL NOT NULL,
    "lifetimetempcoef"	REAL NOT NULL
    );
INSERT INTO resin (name, viscosity, visctempcoef, lifetime, lifetimetempcoef) VALUES ('Пример связующего 1', 0.1, 9000, 10800, 6000);
INSERT INTO resin (name, viscosity, visctempcoef, lifetime, lifetimetempcoef) VALUES ('Пример связующего 2', 0.1, 9000, 5400, 7000);
INSERT INTO resin (name, viscosity, visctempcoef, lifetime, lifetimetempcoef) VALUES ('Пример связующего 3', 0.1, 9000, 3600, 8000);
