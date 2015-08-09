
# Trainscript Test File
VAR global : INT;

PUB main() → c : INT | a : INT, b : INT
	10 → a;
	helper() → b;
	a + b → c;

PRI helper() → result : INT
	20 → result;
