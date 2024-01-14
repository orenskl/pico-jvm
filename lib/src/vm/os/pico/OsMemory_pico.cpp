/*
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) Oren Sokoler (https://github.com/orenskl)
 *
 */

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "Globals.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void *OsMemory_allocate(size_t size) 
{
	/* 
	 * Allocates memory blocks in memory pool (heap)
	 * size: bytes to be allocated 
	 * malloc returns a void pointer to the allocated space.  
	 * if there is insufficient memory available, it returns NULL (0)
	 */
  return malloc(size);
}

void OsMemory_free(void *p) 
{
  /* Deallocates or frees a memory block (pointer *p ) */
  free(p);
}

#ifdef __cplusplus
}
#endif
