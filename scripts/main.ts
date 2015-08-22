VAR global : INT;

OBJ timer   : "/sys/timer";
OBJ console : "/sys/console";

VAR ptr : PTR(INT);
VAR pptr : PTR(PTR(INT));

PUB main() | i : INT, str : TEXT
BEGIN
	0 -> i;

# Testing pointer referencing / dereferencing as rvalue
	15 -> global;
	REF(global) -> ptr;
	console.printInt(VAL(ptr));

# Testing lvalue pointer assignment
	REF(global) -> ptr;
	25 -> VAL(ptr);
	console.printInt(global);

# Testing double pointers
	REF(ptr) -> pptr;
	35 -> VAL(VAL(pptr));
	console.printInt(global);

	"Hello " -> str;
	console.printStr(str + "World!");
	WHILE ((i + 1) -> i) <= fun() DO
	BEGIN
		hlp(i, fun() - i);
		timer.sleep(2);
	END
END

PRI hlp(i : INT, j : INT)
BEGIN
	console.print2Int(i, j);
END

PUB fun() -> i : INT
BEGIN
	5 -> i;
END
