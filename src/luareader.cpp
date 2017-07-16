/*
	project: ManagedLua
	file: luareader.cpp

		Contains the defenitions for Lua reader functions.
*/

#include "Stdafx.h"
#include "luareader.h"

using namespace System;
using namespace System::IO;
using namespace cli;

const char * __reader_lua_(lua_State * L, void * _reader, size_t * size) {
	Stream ^ reader = *((__reader_pack_ *)_reader)->stream;
	array< unsigned char > ^ buffer = *((__reader_pack_ *)_reader)->buffer;
	if (*size = reader->Read(buffer, 0, READER_BUFFER_SIZE)) {
		pin_ptr<System::Byte> p = &buffer[0];
		return (char *)p;
	}
	else
		return nullptr;
}

const char * __string_reader_lua_(lua_State * L, void * _reader, size_t * size) {
	if (((__string_reader_pack_ *)_reader)->readed) {
		*size = 0;
		return nullptr;
	}
	((__string_reader_pack_ *)_reader)->readed = true;
	*size = ((__string_reader_pack_ *)_reader)->len;
	return ((__string_reader_pack_ *)_reader)->str;
}