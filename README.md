# trainOS

## Description
An operating system built around the language "trainscript"
which is used to execute all user code in a virtual machine.

## Current status:
The vm can execute some code and is able to call native code.
Also it leaks memory. A lot of memory.

## Todo List
- Fix memory leaks
-- Validation code leaks memory
-- ???
- Improve virtual machine / trainScript
-- Restructre code execution into loop-based execution instead of a recursion-based one
-- Add support for feature restriction
-- Add variable modifier `SHARED`: C/C++ `static` counterpart
-- Add JIT compiler with thunkers
- Improve kernel
-- Add support for "delegates" (callback + state): `void callback(void *state)

## Guidlines
- Calls to `die` or `die_extra` should follow the following scheme: `ContextName.ErrorName`

## Kernel Architecture
The kernel is a simple kernel that does not utilize paging. It has simple interrupt management where an
interrupt handler can be registerd for each interrupt.

### Bootstrapping
1. Setup 16 kB stack space
2. Clear screen and print boot message
3. Initialize physical memory management (page allocator)
4. Initialize interrupts
5. Enable hardware interrupts
6. Initialize timer
7. Initialize C++ support
8. Register status bar timer callback
9. Start virtual machine (and run OS)
10. Disable hardware interrupts
11. Print stop message

### Memory Allocation
The kernel features a simple malloc structure that allocates memor between 0x400000 and 0x800000 (4 MB heap space).

The allocator can be configured to "secure" its allocation list with a magic number by defining `USE_MAGIC_SECURED_MALLOC` in `config.h`.

It also features a surveillance mode that logs all allocations / frees. It can be enabled by defining `USE_VERBOSE_MALLOC` and `USE_VERBOSE_FREE` in `config.h`.

### Interrupt Mapping
|Number     | Interrupt         |
|-----------|-------------------|
|0x00 - 0x1F|System Interrupts  |
|0x20 - 0x27| IRQ0 - IRQ7       |
|0x28 - 0x2F| IRQ8 - IRQ15      |

## Virtual Machine Architecture

### Type Format
The virtual machine supports 5 base types:

| Name    | Description                                    | Size   |
|---------|------------------------------------------------|--------|
| Void    | A type that specifies a non-existent value.    | 0      |
| Int     | An integral number.                            | 32     |
| Real    | A decimal number.                              | 32/64¹ |
| Text    | A string value containing a sequence of bytes. | any²   |
| Bool    | A boolen value, either `true` or `false`.      | 1³     |

¹) A real number is either a float (32 bits) or a double (64 bits) depending on the configuration in `types.hpp`.

²) A string value is sized the length of the string in bytes plus the size of the length specifier (32 bits).

³) A boolean is representet as a 1 bit value but due to architecture limitations it is stored with 32 bits.

### Variable Format
Variables are stored in a `type+pointer` format where `type` stores the type of the variable and `pointer` either a pointer to the variable value or, if `type` is a pointer type, the pointer value of the variable.

### Native Interoperation
The virtual machine features a `NativeMethod` that allows calling a C function (`result_t __cdecl name(args...)`) from the
virtual machine. Parameters are passed as the VM types declared in `types.hpp`, except for ker::String which is passed as a `const char*`.

### Native Modules
The kernel provides native modules that can be used to interoperate with the kernel and the system hardware.

Following modules are planned:

#### Timer
The timer module provides interaction with the system timer.

#### Memory
The memory module allows allocating and freeing memory as well as getting predefined named memory areas.

#### Interrupts
The interrupt module allows handling interrupts by the current module.


## OS Architecture
To be done.

## trainScript

trainScript is the language of trainOS. All programs are written in trainScript and are compiled on execution.

trainScript programs are executed in the kernels virtual machine and can be restricted in their feature set.

### Example
This example first prints "Hello World!", then counts to 60 and displays every pair of (n, 60 - n).

	OBJ timer   : "/sys/timer";
	OBJ console : "/sys/console";
	
	PUB main() | i : INT, str : TEXT
	BEGIN
		0 → i;
		"Hello " → str;
		console.printStr(str + "World!");
		WHILE ((i + 1) → i) <= fun() DO
		BEGIN
			console.print2Int(i, fun() - i);
			timer.sleep(2);
		END
	END
	
	PUB fun() → i : INT
	BEGIN
		60 → i;
	END

### Features
trainScript is a pure imperative language with strict typing. Operators can only be called on variables of the same type, there is no auto-casting (which means you need to write 0.0 instead of 0 for getting a REAL constant).

It also features the concept of "Modules" and "Objects". Each trainScript file represents a module that exports a set of methods. An object is an imported module instance. So it is possible to use a module multiple times in another module.

### Types
trainScript features 4 base types: `INT`, `REAL`, `BOOL` and `TEXT`. The description of the four types can be found in the section `Virtual Machine Architecture: Type Format`.

### Pointers
