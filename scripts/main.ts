VAR global : INT;

OBJ timer   : "/sys/timer";
OBJ console : "/sys/console";

VAR ptr : PTR(INT);

PUB main() | i : INT, str : TEXT
BEGIN
	0 -> i;
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
	60 -> i;
END
