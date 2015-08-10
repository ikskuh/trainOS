
VAR res : INT;

PUB main(x : INT) → c : INT
BEGIN
	factorial(10) → res;
END

PUB factorial(number : INT) → result : INT
BEGIN
	1 → result;
	IF number > 1 THEN
		number * factorial(number - 1) → result;
END
