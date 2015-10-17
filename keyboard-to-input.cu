== main.cu, /os/main ==

OBJ keyboard : "/io/keyboard";
OBJ console  : "/io/console";

PUB main() | c : TEXT
BEGIN
	keyboard.waitForDriver();
	
	# selects the current thread for reading keyboard input
	keyboard.setFocus();
	WHILE TRUE DO
		keyboard.getChar() → c;
		console.put(c);
	END
END

== keyboard.cu, /io/keyboard ==

TYPE KeyData IS
	…
END

SHARED VAR buffer : KeyData;

SHARED PRI init()
BEGIN
	# Initialize shared data with necessary non-nil pointer
	[KeyData] → buffer;
NED

PUB sendKeyHit(keycode : INT)
BEGIN
	…
END

PUB sendKeyRelease(keycode : INT)
BEGIN
	…
END

PUB getChar() → c : TEXT
BEGIN
	# We just always send a nice little k instead of reading actual key pressed... :P
	"k" → c;
END

== keyboard.driver.cu, /drv/keyboard ==
OBJ interrupts : "/sys/interrupts";
OBJ keyboard   : "/io/keyboard";

PRI irq(id : INT, cpu : CPUSTATE) | breakcode : BOOL, key : INT
BEGIN
	IF id =/= 0x21 THEN
		RETURN;
	END
	# Code that reads scancodes and translates them to keycodes.
	…
	IF breakcode THEN
		keyboard.sendKeyRelease(key);
	ELSE
		keyboard.sendKeyHit(key);
	END
END