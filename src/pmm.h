#pragma once

#include "multiboot.h"

/**
 * Initializes physical memory management.
 * @param mb The multi boot structure used for gathering information about free memory.
 */
void pmm_init(const MultibootStructure *mb);

/**
 * Frees a page of physical memory.
 */
void pmm_free(void *pptr);

/**
 * Allocates a page of physical memory.
 */
void *pmm_alloc(void);

/**
 * Calculates the free memory in bytes.
 */
uint32_t pmm_calc_free(void);
