/*
	project: ManagedLua
	file: luareader.h

		Contains the declaration for the Lua reader functions that reads from the System.IO.Stream objects and from a string.
*/

#pragma once
#include "config.h"

#pragma managed ( push, off )

#include <lua.hpp>

#define READER_BUFFER_SIZE 512 //The length of reader buffer in bytes.

//Structure for stream reader
typedef struct {
	System::IO::Stream ^* stream;		//Pointer to the reference to the Stream object
	array< unsigned char > ^* buffer;	//Pointer to the reference to the buffer object
} __reader_pack_;

//Structure for string reader
typedef struct {
	const char * str;	//The string to read from
	size_t len;			//The string length
	bool readed;		//Switches to true when string is loaded (string reader is called only one time per string)
} __string_reader_pack_;

#pragma managed ( pop )

//Loads a Lua code from a stream
const char * __reader_lua_(lua_State * L, void * _reader, size_t * size);
//Loads a Lua code from a string
const char * __string_reader_lua_(lua_State * L, void * _reader, size_t * size);