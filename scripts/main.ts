VAR global : INT;

PUB main() | i : INT
BEGIN
	fun() -> i;
END

PUB fun() -> i : INT
BEGIN
	10 -> i;
END
