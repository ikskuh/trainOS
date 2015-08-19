VAR global : INT;

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
	30 -> i;
END
