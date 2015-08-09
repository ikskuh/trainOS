
# Trainscript Test File
VAR global : INT;
VAR other : REAL;

PUB main(x : INT) → c : INT | a : INT, b : INT
	10 → a;
	20 → b;
	a + b → global;
#   helper() → b;

PRI helper() → result : INT
	20 → result;

PRI arghelper(a0 : INT, a1 : INT, a2 : INT, a3 : INT)
	x → global;

PRI lochelper() | l0 : INT, l1 : INT, l2 : INT, l3 : INT
	x → global;
