
# Trainscript Test File
VAR global : INT;
VAR v1' : REAL;
VAR v2' : REAL;

PUB main(x : INT) → c : INT | m1:REAL, v1:REAL, m2:REAL, v2:REAL
	# Wagen 1
	4.0 → m1;
	1.2 → v1;

	# Wagen 2
	5.0 → m2;
	0.6 → v2;

	(((m1 - m2) * v1) + (2. * m2 * v2)) / (m1 + m2) → v1';
	(((m2 - m1) * v2) + (2. * m1 * v1)) / (m1 + m2) → v2';

	42 → c;
