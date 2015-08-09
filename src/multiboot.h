#pragma once

#include <inttypes.h>

#define MB_MEMSIZE (1<<0)
#define MB_BOOTDEVICE (1<<1)
#define MB_COMMANDLINE (1<<2)
#define MB_MODULES (1<<3)
#define MB_SYMS_AOUT (1<<4)
#define MB_SYMS_ELF (1<<5)
#define MB_MEMORYMAP (1<<6)
#define MB_DRIVES (1<<7)
#define MB_CONFIG_TABLE (1<<8)
#define MB_BOOTLOADER_NAME (1<<9)
#define MB_APS_TABLE (1<<10)
#define MB_VBE (1<<11)

typedef struct {
    uint32_t    entry_size;
    uint64_t    base;
    uint64_t    length;
    uint32_t    type;
} __attribute__((packed)) MultibootMemoryMap;

typedef struct {
    uintptr_t start;
    uintptr_t end;
    uintptr_t name;
    uint32_t reserved;
} __attribute__((packed)) MultibootModule;

typedef struct {
    uint32_t size;
    uint8_t number;
    uint8_t mode;
    uint16_t cylinders;
    uint8_t heads;
    uint8_t sectors;
    uint16_t ports[0];
    // 0x10	size-0x10	drive_ports	I/O-Ports, die von diesem Gerät benutzt werden
} __attribute__((packed)) MultibootDrive;

typedef struct
{
    uint16_t version;
    uint16_t cseg;
    uint32_t offset;
    uint16_t cseg_16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg_16_len;
    uint16_t dseg_len;
} __attribute__((packed)) MultibootAPMTable;

typedef struct {
    uint32_t flags;
    uint32_t memLower;
    uint32_t memUpper;
    uint32_t bootDevice;
    uintptr_t commandline;
    uint32_t moduleCount;
    uintptr_t modules;
    union {
        struct {
            uint32_t tabsize;
            uint32_t strsize;
            uint32_t addr;
            uint32_t reserved;
        } __attribute__((packed)) symsAssemblerOut;
        struct {
            uint32_t num;
            uint32_t size;
            uintptr_t addr;
            uintptr_t shndx;
        } __attribute__((packed)) symsELF;
    };
    uint32_t memoryMapLength;
    uintptr_t memoryMap;
    uint32_t drivesLength;
    uintptr_t drives;
    uintptr_t configTable;
    uintptr_t bootLoaderName;
    uintptr_t apmTable;
    uint32_t vbeControlInfo;
    uint32_t vbeModeInfo;
    uint16_t vbeMode;
    uint16_t vbeInterfaceSegment;
    uint16_t vbeInterfaceOffset;
    uint16_t vbeInterfaceLength;
} __attribute__((packed)) MultibootStructure;
