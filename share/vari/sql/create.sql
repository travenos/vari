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
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('������ ����������� 1', 0.002, 2e-9, 0.5);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('������ ����������� 2', 0.004, 5e-9, 0.3);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('������ ����������� 3', 0.001, 1e-9, 0.4);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('������ ����������� 4', 0.003, 4e-9, 0.6);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('������ ����������� 5', 0.002, 3e-9, 0.2);
INSERT INTO cloth (name, cavityheight, permeability, porosity) VALUES ('�����������', 0.002, 2e-8, 0.8);
ALTER TABLE cloth OWNER TO vari;
CREATE TABLE resin (
    "id"	SERIAL UNIQUE,
    "name"	TEXT UNIQUE NOT NULL,
    "viscosity"	REAL NOT NULL,
    "tempcoef"	REAL NOT NULL
    );
INSERT INTO resin (name, viscosity, tempcoef) VALUES ('������ ���������� 1', 0.1, 0.8);
INSERT INTO resin (name, viscosity, tempcoef) VALUES ('������ ���������� 2', 0.1, 0.8);
INSERT INTO resin (name, viscosity, tempcoef) VALUES ('������ ���������� 3', 0.1, 0.8);
ALTER TABLE resin OWNER TO vari;
