
.asmtype DRIVER
.name KEYBOARDDRIVER

NATIVE print(...);

NATIVE outb(port : UINT16, value : UINT8);
NATIVE inb(port : UINT16) → UINT8;

NATIVE toInt32(…) → INT32;
NATIVE toUInt16(…) → UINT16;

TYPE CpuState IS
	eax : UINT32;
	ebx : UINT32;
	ecx : UINT32;
END

PRI sendCommand(cmd : UINT8)
BEGIN
	# Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
	WHILE (inb(100u16) & 2u8) =/= 0u8 DO

	END
	outb(toUInt16(96), cmd);
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

PRI handleKeyPress(keycode : INT, released : BOOL)
BEGIN
	IF released = FALSE THEN
		print("[keycode=", keycode, "]\n");
	END
END

PUB main() | i : INT
BEGIN
	print("Initialize keyboard...\n");
	initKeyboard();
END

# Status-Variablen fuer das Behandeln von e0- und e1-Scancodes
VAR e0Code : BOOL;

# Wird auf 1 gesetzt, sobald e1 gelesen wurde, und auf 2, sobald das erste
# Datenbyte gelesen wurde
VAR e1Code : INT;

VAR e1Prev : UINT16;

PRI translateKeyCode(extend : INT, scancode : INT) → keycode : INT
BEGIN
	print("[extend=", extend, ",scancode=", scancode, "]\n");
	0 → keycode;
	IF extend = 0 THEN
		# LOTS OF CODE HERE. Maybe optimize this? :P
		IF scancode = 10 THEN 1 → keycode; RETURN; END
	END
END

PRI handleKeyboardIRQ() | scancode : UINT8, keycode : INT, breakcode : BOOL
BEGIN
	0 → keycode;
	FALSE → breakcode;
	inb(96u16) → scancode;

	# Um einen Breakcode handelt es sich, wenn das oberste Bit gesetzt ist und
	# es kein e0 oder e1 fuer einen Extended-scancode ist
	IF ((scancode & 0x80u8) =/= 0u8) & ((e1Code =/= 0) | (scancode =/= 0xE1u8)) & (e0Code | (scancode =/= 0xE0u8)) THEN
		TRUE → breakcode;
		(scancode & 127u8) → scancode;
	END

#	print("[scancode=", scancode, ",breakcode=", breakcode, "]");

	IF e0Code THEN
		# Fake shift abfangen und ignorieren
		IF (scancode = 0x2Au8) | (scancode = 0x36u8) THEN
			FALSE → e0Code;
			RETURN;
		END

		translateKeyCode(1, toInt32(scancode)) → keycode;
		FALSE → e0Code;
	ELSE
		IF e1Code = 2 THEN
			# Fertiger e1-Scancode
			# Zweiten Scancode in hoeherwertiges Byte packen
			(e1Prev | toUInt16(scancode * 256u8)) → e1Prev;
			translateKeyCode(2, toInt32(e1Prev)) → keycode;
			0 → e1Code;
		ELSE
			IF e1Code = 1 THEN
				# Erstes Byte fuer e1-Scancode
				toUInt16(scancode) → e1Prev;
				2 → e1Code;
			ELSE
				IF scancode = 0xE0u8 THEN
					TRUE → e0Code;
				ELSE
					IF scancode = 0xE1u8 THEN
						1 → e1Code;
					ELSE
						translateKeyCode(0, toInt32(scancode)) → keycode;
					END
				END
			END
		END
	END

	IF keycode =/= 0 THEN
		handleKeyPress(keycode, breakcode);
	END
END

PUB irq(id : INT, data : CpuState)
BEGIN
#   print("irq:", id, ", ", data.eax, " ", data.ebx, "\n");
	IF id = 33 THEN
		handleKeyboardIRQ();
	END
	~data;
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
	☑ NEW/DELETE
		☑ [TYPE] →
		☑ ~()
	☑ hex literals für Copper
	☑ 'RETURN' für Copper
Was noch gemacht werden muss, bis der Tastatur-Treiber schön aussieht:
	☐ ELSE-IF
	☐ ARRAYS
!#
