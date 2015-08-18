VAR global : INT;

PUB main() | i : INT
BEGIN
	0 -> i;
	WHILE ((i + 1) -> i) <= 25 DO
	BEGIN
		print2Int(50 - i, i);
		sleep(2);
	END
END

PUB fun() -> i : INT
BEGIN
	10 -> i;
END
