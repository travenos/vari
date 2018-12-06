CREATE ROLE vari WITH SUPERUSER LOGIN PASSWORD 'vari-password';
CREATE DATABASE vari WITH ENCODING = 'UTF8' OWNER = vari;
\c vari
CREATE TABLE cloth (
    "id"	SERIAL,
    "name"	TEXT UNIQUE NOT NULL,
    "cavityheight"	REAL NOT NULL,
    "permeability"	REAL NOT NULL,
    "porosity"	REAL NOT NULL
    );
ALTER TABLE cloth OWNER TO vari;
CREATE TABLE resin (
    "id"	SERIAL,
    "name"	TEXT UNIQUE NOT NULL,
    "viscosity"	REAL NOT NULL,
    "tempcoef"	REAL NOT NULL
    );
ALTER TABLE resin OWNER TO vari;
