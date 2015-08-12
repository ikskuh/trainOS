
VAR res : INT;

PUB main(x : INT) → c : INT
BEGIN
#	factorial(10) → res;
	fibonacci(7) → res;
END

PUB factorial(number : INT) → result : INT
BEGIN
	IF number > 1 THEN
		number * factorial(number - 1) → result;
	ELSE
		1 → result;
END

# Recursive test
PUB fibonacci(n : INT) → f : INT
BEGIN
	IF n = 0 THEN
		0 → f;
	ELSEIF n = 1 THEN
		1 → f;
	ELSE
		fibonacci(n - 1) + fibonacci(n - 2) → f;
END
