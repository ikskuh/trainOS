
VAR res : INT;

PUB main(x : INT) → c : INT
BEGIN
#	factorial(10) → res;
#	fibonacci(7) → res;
	problem1(1000) → res;
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

# If we list all the natural numbers below 10 that are
# multiples of 3 or 5, we get 3, 5, 6 and 9. The sum of
# these multiples is 23.
# Find the sum of all the multiples of 3 or 5 below 1000.
PUB problem1(max : INT) → r : INT | iter : INT
BEGIN
	1 → iter;
	0 → r;
	WHILE iter < max DO
	BEGIN
		IF (iter % 5) = 0 THEN
			r + iter → r;
		ELSEIF (iter % 3) = 0 THEN
			r + iter → r;
		iter + 1 → iter;
	END
END
