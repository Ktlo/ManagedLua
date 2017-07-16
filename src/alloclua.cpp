/*
	project: ManagedLua
	file: alloclua.cpp

		Contains the definition for lua functions that allocates and frees memory from preallocated block.
*/

#include "stdafx.h"
#include "alloclua.h"

#pragma managed ( push, off )

//Sets the length for the memory block
#define setlength(ptr, l)\
		(*(size_t *)(ptr) = (l))

//Sets the type for the memory block
#define settype(ptr, t)\
		(*(ptr + sizeof(size_t)) = (byte)t)

//Makes the block free
#define setnothing(ptr)\
		(*(ptr + sizeof(size_t)) = 0xFFui8)

//Gets the memory block of the pointer to allocated data
#define getblock(ptr)\
		((byte *)(ptr) - DELTA)

void __allocator_free_(void *ptr, byte * begin) {
	//Free the mem block
	byte * data = getblock(ptr);
	size_t len = lengthof(data);
	setnothing(data);
	byte * block = data + len;
	//Concat with near blocks if they are free
	//Concat with the next block
	if (isfree(block))
		setlength(data, len + lengthof(block));
	//Concat with the previous block
	for (block = begin, begin += lengthof(begin); begin < data; block = begin, begin += lengthof(begin));
	if (isfree(block) && block != data)
		setlength(block, lengthof(block) + lengthof(data));
}

void * __allocator_malloc_(memInfo * mem, size_t nsize, byte type) {
	byte * end = mem->memory + mem->size;	//The end of preallocated memory
	size_t len;								//The length of the preallocated memory
	byte * data;							//The current memory block
	//Allocating a new memory block
	for (data = mem->memory; data < end; data += lengthof(data)) {
		//If there are suitable mem block
		if (isfree(data) && (len = lengthof(data)) - DELTA >= nsize) {
			settype(data, type);
			if ((len -= DELTA + nsize) >= DELTA) {
				//Divide this block on occupied and free if possible
				setlength(data, nsize + DELTA);
				setlength(data + nsize + DELTA, len);
				setnothing(data + nsize + DELTA);
			}
			return getptr(data);
		}
	}
	//Return the null if there are no good memory block
	return nullptr;
}

void * __allocator_lua_(void *mem, void *ptr, size_t osize, size_t nsize) {
	byte * begin = ((memInfo *)mem)->memory;	//The memory beginning
	size_t len;									//The length of mem block
	byte * data;								//Current mem block
	if (ptr && nsize == 0) {
		__allocator_free_(ptr, begin);
		return nullptr;
	}
	else if (ptr) {
		//Reallocating
		data = getblock(ptr);
		len = lengthof(data);
		//If new block size is smaller than try to shrink it
		if (nsize + DELTA <= len) {
			if (len - nsize >= 2 * DELTA) {
				setlength(data, nsize + DELTA);
				data += nsize + DELTA;
				setlength(data, len - nsize - DELTA);
				setnothing(data);
			}
			return ptr;
		}
		//If the new block size is bigger than try to increase it with the next free block
		{
			byte * block = data + len;
			size_t blocklen = lengthof(block);
			if (isfree(block) && blocklen + len >= nsize + DELTA) {
				//Cut the unused part of the free block if possible
				if (blocklen + len - nsize >= 2 * DELTA) {
					setlength(data, nsize + DELTA);
					block = data + nsize + DELTA;
					setlength(block, blocklen + len - nsize - DELTA);
					setnothing(block);
				}
				else
					setlength(data, blocklen + len);
				return ptr;
			}
		}
		//Otherwise try to find a new block of the given size
		data = (byte *)__allocator_malloc_((memInfo *)mem, nsize, gettype(data));
		if (data) {
			//If it was found copy the memory to this block and free old one
			std::memcpy(data, ptr, nsize);
			__allocator_free_(ptr, begin);
		}
		return data;
	}
	if (!ptr) {
		//Allocate new mem block
		return __allocator_malloc_((memInfo *)mem, nsize, osize);
	}
	return nullptr;
}

#pragma managed ( pop )