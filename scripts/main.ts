VAR global : INT;

# OBJ timer       : "/sys/timer";
# OBJ heap        : "/sys/malloc";
# OBJ interrupts  : "/sys/interrupt";

PUB main() | i : INT
BEGIN
	0 -> i;
	WHILE ((i + 1) -> i) <= fun() DO
	BEGIN
		print2Int(fun() - i, i);
		sleep(2);
	END
END

PUB fun() -> i : INT
BEGIN
	60 -> i;
END
