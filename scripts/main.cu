PUB main() | i : INT
BEGIN
    native.print("Hello World!\n");
    1 → i;
    WHILE i <= 5 DO
        native.print(i, "\n");
        (i + 1) → i;
    END
END

PUB irq(id : INT, data : INT)
BEGIN
    native.print("irq:", id, ", ", data.eax, " ", data.ebx, "\n");
END

#!
Was noch gemacht werden muss, bis der Tastatur-Treiber funktionieren könnte:
    - Support für beliebige Typen in Copper
    - Pushen von Typen != INT
        - erweiterung von pushi um typcode (u?int(8|16|32), bool)
        - erweiterung von pushr um typcode
    - Bitwise Operations
    - outb und inb
    - eventuell hex literals für Copper
!#
