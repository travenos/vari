CREATE ROLE vari WITH SUPERUSER LOGIN PASSWORD 'vari-password';
CREATE DATABASE vari WITH ENCODING = 'UTF8' OWNER = vari;
\c vari
CREATE TABLE cloth (
    "id"	SERIAL UNIQUE,
    "name"	TEXT UNIQUE NOT NULL,
    "cavityheight"	REAL NOT NULL,
    "permeability"	REAL NOT NULL,
    "porosity"	REAL NOT NULL
    );
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('Пример наполнителя', 0.002, 2e-9, 0.5);
ALTER TABLE cloth OWNER TO vari;
CREATE TABLE resin (
    "id"	SERIAL UNIQUE,
    "name"	TEXT UNIQUE NOT NULL,
    "viscosity"	REAL NOT NULL,
    "tempcoef"	REAL NOT NULL
    );
INSERT INTO resin (name, viscosity, tempcoef) VALUES ('Пример связующего', 0.1, 0.8);
ALTER TABLE resin OWNER TO vari;
