VAR global : INT;

# OBJ timer       : "/sys/timer";
# OBJ heap        : "/sys/malloc";
# OBJ interrupts  : "/sys/interrupt";

PUB main() | i : INT
BEGIN
	0 -> i;
	WHILE ((i + 1) -> i) <= fun() DO
	BEGIN
		hlp(i, fun() - i);
		sleep(2);
	END
END

PRI hlp(i : INT, j : INT)
BEGIN
	print2Int(i, j);
END

PUB fun() -> i : INT
BEGIN
	60 -> i;
END
