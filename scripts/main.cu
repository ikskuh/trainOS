NATIVE print(...);

NATIVE outb(port : UINT16, value : UINT8);
NATIVE inb(port : UINT16) → UINT8;

PRI sendCommand(cmd : UINT8)
BEGIN
    # Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
    WHILE (inb(100u16) & 2u8) =/= 0u8 DO

    END
    outb(96u16, cmd);
END

PRI initKeyboard()
BEGIN
    # Tastaturpuffer leeren
    WHILE (inb(100u16) & 1u8) =/= 0u8 DO
        inb(96u16);
    END

    # Tastatur aktivieren
#   sendCommand(244u8);
END

PUB main() | i : INT
BEGIN
    print("Initialize keyboard...\n");
    initKeyboard();

    print("Hello World!\n");
    1 → i;
    WHILE i <= 5 DO
        print(i, "\n");
        (i + 1) → i;
    END
END

TYPE CpuState IS
    eax : UINT32;
    ebx : UINT32;
    ecx : UINT32;
END

PUB irq(id : INT, data : CpuState) | scancode : UINT8
BEGIN
#   print("irq:", id, ", ", data.eax, " ", data.ebx, "\n");
    IF id = 33 THEN
        inb(96u16) → scancode;
        print("keypress: ", scancode, "\n");
    END
END

#!
Was noch gemacht werden muss, bis der Tastatur-Treiber funktionieren könnte:
    ☑ Support für beliebige Typen in Copper
    ☑ Pushen von Typen != INT
        ☑ erweiterung von pushi um typcode (u?int(8|16|32), bool)
        ☑ erweiterung von pushr um typcode
    ☑ Simple type checks in Copper
    ☑ Custom types in Copper
    ☑ Native function calls in Copper
    ☑ Bitwise Operations
    ☑ outb und inb
    ☐ eventuell hex literals für Copper
!#