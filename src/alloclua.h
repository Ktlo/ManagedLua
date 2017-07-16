/*
	project: ManagedLua
	file: alloclua.h
	
		Contains the Lua allocator function declaration, the memory structure and some preprocessors that helps
	to work with memory blocks in MemManager.
*/

#pragma once
#include "config.h"

#pragma managed ( push, off )

#include <inttypes.h>
#include <stddef.h>

typedef uint8_t byte;

//Structure with information about the allocated memory for Lua
typedef struct {
	size_t size;	//The size of the memory
	byte * memory;	//Pointer to the first byte of the memory
} memInfo;

//Lua allocator that allocate memory from a MemManager object
void * __allocator_lua_(void *mem, void *ptr, size_t osize, size_t nsize);

//The header size for each block allocated by Lua
#define DELTA (sizeof(size_t) + sizeof(byte))

//Preprocessor that returns the type of the allocated data in 'ptr' block
#define gettype(ptr)\
		(*((byte *)(ptr) + sizeof(size_t)))

//Returns the pointer to the data
#define getptr(ptr)\
		((void *)(ptr + DELTA))

//Gets the size of the block including the DELTA
#define lengthof(ptr)\
		(*(size_t *)(ptr))

//Checks if the given memory block is unused
#define isfree(ptr)\
		(*((byte *)(ptr) + sizeof(size_t)) == 0xFFui8)

#pragma managed ( pop )