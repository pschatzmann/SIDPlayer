#pragma once

/**
 * @brief Defines the size of the memory which is allocated
 * 
 */
#ifndef MAX_DATA_LEN
#  define MAX_DATA_LEN 65536
#endif

/**
 * Prevent that we use file sizes which are too big
*/
#ifndef MAX_FILE_SIZE
#  define MAX_FILE_SIZE 20000
#endif

/**
 * Memory Allocation Logic
 * 1) dynamic allocation with malloc
 * 2) ESP32 using PSRAM (psmalloc) 
 * 3) static allocation
 * 4) mapped memory (Experimental/avoiding allocations)
 * 
*/
#ifndef MEMORY_ALLOCATION_LOGIC
#  define MEMORY_ALLOCATION_LOGIC 3
#endif


