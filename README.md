# trainOS

## Description
An operating system built around the language "trainscript"
which is used to execute all user code in a virtual machine.

## Current status:
The vm can execute some code and is able to call native code.
Also it leaks memory. A lot of memory.

## Todo List
- Fixing Memory Leaks
- Adding support for Modules
- Adding support for Pointer Types
- Adding support for String Type

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


## OS Architecture
To be done.
