VAR global : INT;

PUB main() | i : INT
BEGIN
	0 -> i;
	# Print numbers from 1 to 5
	WHILE ((i + 1) -> i) <= 5 DO
		printInt(i);
END

PUB fun() -> i : INT
BEGIN
	10 -> i;
END
