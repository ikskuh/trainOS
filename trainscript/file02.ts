
# Trainscript Test File
VAR v1' : REAL;
VAR v2' : REAL;

# Execute some stuff
PUB main(x : INT) → c : INT
	physics();
	factorial(2);
	42 → c;

# Physics exercise with elastic collision
PRI physics() | m1:REAL, v1:REAL, m2:REAL, v2:REAL
	# Wagen 1
	4.0 → m1;
	1.2 → v1;

	# Wagen 2
	5.0 → m2;
	0.6 → v2;

	(((m1 - m2) * v1) + (2. * m2 * v2)) / (m1 + m2) → v1';
	(((m2 - m1) * v2) + (2. * m1 * v1)) / (m1 + m2) → v2';

# Recursive test
PUB fibonacci(n : INT) → f : INT
  #IF n = 0
	0 → f;
  #ELSEIF n = 1
	1 → f;
  #ELSE
	fibonacci(n - 1) + fibonacci(n - 2) → f;

PUB factorial(number : INT) → result : INT
  #IF number <= 1
	1 → result;
  #ELSE
	number * factorial(number - 1) → result;
