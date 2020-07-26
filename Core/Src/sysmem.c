/**
 ******************************************************************************
 * @file      sysmem.c
 * @author    Auto-generated by STM32CubeIDE
 * @brief     STM32CubeIDE Minimal System Memory calls file
 *
 *            For more information about which c-functions
 *            need which of these lowlevel functions
 *            please consult the Newlib libc-manual
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes */
#include <errno.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <cmsis_os2.h>

/* Variables */
extern int errno;
register char * stack_ptr asm("sp");

/* Functions */

/**
 _sbrk
 Increase program data space. Malloc and related functions depend on this
**/
caddr_t _sbrk(int incr)
{
	extern char end asm("end");
	static char *heap_end;
	char *prev_heap_end;

	if (osKernelGetState() == osKernelRunning)
	{
	  if (incr <= 0)
	  {
	    return NULL;
	  }
	  return pvPortMalloc(incr);
	}

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr)
	{
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

void* malloc(size_t size)
{
  void* ptr = NULL;

  if(size > 0)
  {
    // We simply wrap the FreeRTOS call into a standard form
    ptr = pvPortMalloc(size);
  } // else NULL if there was an error

  return ptr;
}

void free(void* ptr)
{
  if(ptr)
  {
    // We simply wrap the FreeRTOS call into a standard form
    vPortFree(ptr);
  }
}

