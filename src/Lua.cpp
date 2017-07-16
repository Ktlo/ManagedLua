/*
	project: ManagedLua
	file: Lua.cpp

		Contains the defenitions of the methods that were declared in the Lua class.
*/

#include "stdafx.h"
#include "Lua.h"
//Data types
#include "DataType.h"
#include "Nil.h"
#include "LuaBoolean.h"
#include "LightUserData.h"
#include "Number.h"
#include "LuaString.h"
#include "Table.h"
#include "Function.h"
#include "UserData.h"
//Exceptions
#include "LuaException.h"
#include "NotSupportedLuaTypeException.h"
#include "DifferentLuaStatesException.h"
//Tools
#include "LuaGC.h"
#include "MemManager.h"
//Native
#include "alloclua.h"
#include "luareader.h"
#include "synchlua.h"
#include "luaCLR.h"

namespace ManagedLua {

	Lua::Lua() : LuaObject(DataType::thread) {
		funcCallState = false;
		mem = nullptr;
		parentThread = nullptr;
		L = luaL_newstate();
	}

	void Lua::PushSampleType(DataType type) {
		switch (type) {
		case DataType::nil:
			lua_pushnil(L);
			break;
		case DataType::boolean:
			lua_pushboolean(L, 0);
			break;
		case DataType::lightuserdata:
			lua_pushlightuserdata(L, nullptr);
			break;
		case DataType::number:
			lua_pushinteger(L, 1);
			break;
		case DataType::luastring:
			lua_pushstring(L, "");
			break;
		case DataType::function:
			lua_pushcfunction(L, nullptr);
			break;
		case DataType::thread:
			lua_pushthread(L);
			break;
		default:
			throw gcnew Exceptions::NotSupportedLuaTypeException(type);
		}
	}

	INLINE Lua::Lua(lua_State * state) : LuaObject(DataType::thread) {
		mem = nullptr;
		parentThread = nullptr;
		L = state;
		funcCallState = true;
	}

	Lua::Lua(Lua ^ parent) : LuaObject(DataType::thread) {
		mem = nullptr;
		//It's unsafe to hold the function call states during their short life time
		parentThread = parent->funcCallState ? nullptr : parent;
		funcCallState = false;

		//Add the new thread to the list of all threads in the registry table for Lua GC protection
		lua_State * K = parent->L;
		switch (lua_rawgetp(K, LUA_REGISTRYINDEX, address)) {
		case LUA_TNIL:
		default:
			//Create new list if does not exists.
			lua_pop(K, 1);
			lua_createtable(K, 0, 0);
			lua_pushvalue(K, -1);
			lua_rawsetp(K, LUA_REGISTRYINDEX, address);
		case LUA_TTABLE:
			L = lua_newthread(K);
			lua_pushboolean(K, true);
			lua_rawset(K, -3);
			break;
		}
		lua_pop(K, 1);
	}

	Lua::Lua(Tools::MemManager ^ manager) : LuaObject(DataType::thread) {
		mem = manager;
		funcCallState = false;
		parentThread = nullptr;
		L = lua_newstate(__allocator_lua_, mem->info);
	}

	INLINE Lua::Lua(Lua ^ state, int i) : LuaObject(DataType::thread, state, i) {
		L = lua_tothread(state->L, i);
		//It's unsafe to hold the function call states during their short life time
		parentThread = state->funcCallState ? nullptr : state;
		mem = nullptr;
		funcCallState = false;
	}

	Lua ^ Lua::GetMainThread() {
		Lua ^ previous = this;
		Lua ^ next = (Lua ^)parentThread;
		while (next != nullptr) {
			previous = next;
			next = (Lua ^)next->parentThread;
		}
		return previous;
	}

	inline Lua ^ Lua::GetParentThread() {
		return parentThread;
	}

	inline Lua ^ Lua::NewThread() {
		return gcnew Lua(this);
	}

	LuaObject ^ Lua::Stack::get(int i) {
		switch (lua_type(L, i)) {
		case LUA_TNIL:
			return gcnew Nil(this, i);
		case LUA_TBOOLEAN:
			return gcnew LuaBoolean(this, i);
		case LUA_TLIGHTUSERDATA:
			return gcnew LightUserData(this, i);
		case LUA_TNUMBER:
			return gcnew Number(this, i);
		case LUA_TSTRING:
			return gcnew LuaString(this, i);
		case LUA_TTABLE:
			return gcnew Table(this, i);
		case LUA_TFUNCTION:
			return gcnew Function(this, i);
		case LUA_TUSERDATA:
			return gcnew UserData(this, i);
		case LUA_TTHREAD:
			return gcnew Lua(this, i);
		}
		return nullptr;
	}

	INLINE LuaObject ^ Lua::Pop() {
		LuaObject ^ obj = this->Stack[-1];
		lua_pop(L, 1);
		return obj;
	}

	void Lua::CopyTo(Lua ^ thread, int i) {
		if (!CompareStates(this, thread))
			throw gcnew Exceptions::DifferentLuaStatesException;
		lua_pushvalue(L, i);
		lua_xmove(L, thread->L, 1);
	}

	INLINE bool Lua::IsMainThread() {
		return GetMainThread() == this;
	}

	inline Tools::LuaGC ^ Lua::GC::get() {
		return Tools::LuaGC(this);
	}

	inline void Lua::OpenLibs() {
		luaL_openlibs(L);
	}

#define add_f(L, f, n)\
		lua_pushstring(L, n);	\
		lua_pushcfunction(L, f);\
		lua_rawset(L, -3)

	void Lua::OpenManegedLib() {
		lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		add_f(L, __typeof_lua_, "typeof");
		add_f(L, __eventHandler_lua_, "listener");
		add_f(L, __Int8, "sbyte");
		add_f(L, __UInt8, "byte");
		add_f(L, __Int16, "int16");
		add_f(L, __UInt16, "uint16");
		add_f(L, __Int32, "int32");
		add_f(L, __UInt32, "uint32");
		add_f(L, __Int64, "int32");
		add_f(L, __UInt64, "uint64");
		add_f(L, __Single, "single");
		add_f(L, __Double, "double");
		lua_pushlstring(L, "System", 6);
		__register_namespa_lua_(L, "System", 6);
		lua_rawset(L, -3);
		lua_pop(L, 1);
	}

#undef add_f

	Table ^ Lua::GetGlobal() {
		lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		Table ^ table = gcnew Table(this, -1);
		lua_pop(L, 1);
		return table;
	}

	Table ^ Lua::GetRegistry() {
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		Table ^ table = gcnew Table(this, -1);
		lua_pop(L, 1);
		return table;
	}

	Table ^ Lua::NewTable(size_t narr, size_t nrec) {
		lua_createtable(L, narr, nrec);
		Table ^ table = gcnew Table(this, -1);
		lua_pop(L, 1);
		return table;
	}

	inline Table ^ Lua::NewTable() {
		return NewTable(0, 0);
	}

	Table ^ Lua::GetTypeMetatable(DataType type) {
		PushSampleType(type);
		if (lua_getmetatable(L, -1)) {
			Table ^ result = gcnew Table(this, -1);
			lua_pop(L, 2);
			return result;
		}
		else {
			lua_pop(L, 2);
			return nullptr;
		}
	}

	void Lua::SetTypeMetatable(DataType type, Table ^ table) {
		PushSampleType(type);
		if (table == nullptr)
			lua_pushnil(L);
		else
			table->L->CopyTo(this, 1);
		lua_setmetatable(L, -2);
	}

	Function ^ Lua::Load(Stream ^ reader, String ^ name, LoadMode mode) {
		array< unsigned char > ^ buffer = gcnew array< unsigned char >(READER_BUFFER_SIZE);
		//if (!reader->CanRead)
		//	throw gcnew System::IO::InvalidDataException();
		__reader_pack_ pack = { &reader, &buffer };
		const char * m;
		switch (mode) {
		case LoadMode::text:
			m = "t";
			break;
		case LoadMode::bytecode:
			m = "b";
			break;
		case LoadMode::both:
		default:
			m = nullptr;
			break;
		}
		reader->Read(buffer, 0, 3);
		array<byte> ^ utf_8_name = Text::Encoding::UTF8->GetBytes(name);
		pin_ptr<byte> name_ptr = &utf_8_name[0];
		int error_code = lua_load(L, &__reader_lua_, &pack, (const char *)name_ptr, m);
		switch (error_code) {
		case LUA_OK: {
			Function ^ func = gcnew Function(this, -1);
			lua_pop(L, 1);
			return func;
		} break;
		case LUA_ERRSYNTAX:
		case LUA_ERRMEM:
		case LUA_ERRGCMM: {
			LuaObject ^ obj = Pop();
			throw gcnew Exceptions::LuaException((Exceptions::LuaException::ErrorCodes)error_code, obj->ToString(), obj);
		} break;
		default:
			break;
		}
		return nullptr;
	}

	inline Function ^ Lua::Load(Stream ^ reader, String ^ name) {
		return Load(reader, name, LoadMode::both);
	}

	inline Function ^ Lua::Load(Stream ^ reader, LoadMode mode) {
		return Load(reader, "ManagedLua", mode);
	}

	inline Function ^ Lua::Load(Stream ^ reader) {
		return Load(reader, "ManagedLua", LoadMode::both);
	}

	Function ^ Lua::LoadString(String ^ str, String ^ name, LoadMode mode) {
		array<byte> ^ utf8_luacode = Text::Encoding::UTF8->GetBytes(str);
		pin_ptr<byte> luacode_ptr = &utf8_luacode[0];
		__string_reader_pack_ pack = { (const char *)luacode_ptr, utf8_luacode->GetLength(0), false };
		const char * m;
		switch (mode) {
		case LoadMode::text:
			m = "t";
			break;
		case LoadMode::bytecode:
			m = "b";
			break;
		case LoadMode::both:
		default:
			m = nullptr;
			break;
		}
		array<byte> ^ utf_8_name = Text::Encoding::UTF8->GetBytes(name);
		pin_ptr<byte> name_ptr = &utf_8_name[0];
		int error_code = lua_load(L, &__string_reader_lua_, &pack, (const char *)name_ptr, m);
		switch (error_code) {
		case LUA_OK: {
			Function ^ func = gcnew Function(this, -1);
			lua_pop(L, 1);
			return func;
		} break;
		case LUA_ERRSYNTAX:
		case LUA_ERRMEM:
		case LUA_ERRGCMM: {
			LuaObject ^ obj = Pop();
			throw gcnew Exceptions::LuaException((Exceptions::LuaException::ErrorCodes)error_code, obj->ToString(), obj);
		} break;
		default:
			break;
		}
		return nullptr;
	}

	inline Function ^ Lua::LoadString(String ^ str, String ^ name) {
		return LoadString(str, name, LoadMode::both);
	}

	inline Function ^ Lua::LoadString(String ^ str, LoadMode mode) {
		return LoadString(str, "ManagedLua", mode);
	}

	Function ^ Lua::LoadString(System::String ^ str) {
		return LoadString(str, "ManagedLua", LoadMode::both);
	}

	bool Lua::CompareStates(Lua ^ a, Lua ^ b) {
		lua_rawgeti(a->L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		lua_rawgeti(b->L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		bool result = lua_tothread(a->L, -1) == lua_tothread(b->L, -1);
		lua_pop(a->L, 1);
		lua_pop(b->L, 1);
		return result;
	}

	void Lua::pushValue(Lua ^ state) {
		if (!CompareStates(this, state))
			throw gcnew Exceptions::DifferentLuaStatesException;
		lua_pushthread(L);
		lua_xmove(L, state->L, 1);
	}

	bool Lua::ToBoolean() {
		return true;
	}

	String ^ Lua::ToString() {
		lua_pushthread(L);
		size_t len;
		String ^ str = gcnew String(luaL_tolstring(L, -1, &len), 0, len);
		lua_pop(L, 2);
		return str;
	}

	Lua::!Lua() {
		if (funcCallState)
			return;
		lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);

		if (L == lua_tothread(L, -1)) {
			lua_close(L);
			this->L = nullptr;
		}
		else {
			lua_pop(L, 1);
			if (lua_rawgetp(L, LUA_REGISTRYINDEX, address) == LUA_TTABLE) {
				lua_pushthread(L);
				lua_pushnil(L);
				lua_settable(L, LUA_REGISTRYINDEX);
			}
		}
	}

}