NATIVE print(...);

NATIVE shutdown();

PUB main() | i : INT
BEGIN
    print("Hello World!\n");
	1 → i;
    WHILE i <= 5 DO
        print(i, "\n");
        (i + 1) → i;
    END
END
