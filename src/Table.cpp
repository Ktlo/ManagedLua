/*
	project: ManagedLua
	file: Table.cpp

		Contains the definitions for the Table class methods.
*/

#include "stdafx.h"
#include "Table.h"

#include "DataType.h"
#include "Lua.h"
#include "NotSupportedLuaTypeException.h"
#include "TableEnumerator.h"

namespace ManagedLua {

	INLINE Table::Table(Lua ^ state, int i) : LuaObject(DataType::table, state, i) {
		enumer = nullptr;
		L = state->NewThread();
		state->CopyTo(L, -1);
		raw = false;
	}

	bool Table::RawAccess::get() {
		return raw;
	}

	void Table::RawAccess::set(bool value) {
		raw = value;
	}

	int Table::Length::get() {
		if (raw)
			return lua_rawlen(L->L, 1);
		else {
			lua_State * G = L->L;
			lua_len(G, 1);
			if (lua_isinteger(G, -1)) {
				int result = lua_tointeger(G, -1);
				lua_pop(G, 1);
				return result;
			}
			else {
				lua_pop(G, 1);
				throw gcnew Exceptions::NotSupportedLuaTypeException((DataType)lua_type(G, -1));
			}
		}
	}

	LuaObject ^ Table::default::get(LuaObject ^ key) {
		key->pushValue(L);
		if (raw)
			lua_rawget(L->L, 1);
		else
			lua_gettable(L->L, 1);
		return L->Pop();
	}

	void Table::default::set(LuaObject ^ key, LuaObject ^ value) {
		key->pushValue(L);
		value->pushValue(L);
		if (raw)
			lua_rawset(L->L, 1);
		else
			lua_settable(L->L, 1);
	}

	LuaObject ^ Table::default::get(int i) {
		if (raw)
			lua_rawgeti(L->L, 1, i);
		else
			lua_geti(L->L, 1, i);
		return L->Pop();
	}

	void Table::default::set(int i, LuaObject ^ value) {
		value->pushValue(L);
		if (raw)
			lua_rawseti(L->L, 1, i);
		else
			lua_seti(L->L, 1, i);
	}

	LuaObject ^ Table::default::get(String ^ key) {
		array<Byte> ^ encodedBytes = Text::Encoding::UTF8->GetBytes(key);
		pin_ptr<Byte> str = &encodedBytes[0];
		lua_pushlstring(L->L, (char *)str, encodedBytes->Length);
		if (raw)
			lua_rawget(L->L, 1);
		else
			lua_gettable(L->L, 1);
		return L->Pop();
	}

	void Table::default::set(String ^ key, LuaObject ^ val) {
		array<Byte> ^ encodedBytes = Text::Encoding::UTF8->GetBytes(key);
		pin_ptr<Byte> str = &encodedBytes[0];
		lua_pushlstring(L->L, (char *)str, encodedBytes->Length);
		val->pushValue(L);
		if (raw)
			lua_rawset(L->L, 1);
		else
			lua_settable(L->L, 1);
	}

	System::Collections::Generic::IEnumerator<System::Collections::Generic::KeyValuePair<LuaObject ^, LuaObject ^>> ^ Table::GetEnumerator() {
		if (enumer == nullptr)
			return enumer = gcnew Tools::TableEnumerator(this);
		else {
			enumer->Reset();
			return enumer;
		}
	}

	Table ^ Table::Metatable::get() {
		if (lua_getmetatable(L->L, 1)) {
			Table ^ meta = gcnew Table(L, -1);
			lua_pop(L->L, 1);
			return meta;
		}
		else
			return nullptr;
	}

	void Table::Metatable::set(Table ^ table) {
		table->pushValue(L);
		lua_setmetatable(L->L, 1);
	}

	void Table::pushValue(Lua ^ state) {
		L->CopyTo(state, 1);
	}

	bool Table::ToBoolean() {
		return true;
	}

	String ^ Table::ToString() {
		size_t len;
		auto str = gcnew String(luaL_tolstring(L->L, 1, &len), 0, len);
		lua_pop(L->L, 1);
		return str;
	}

	bool Table::Equals(LuaObject ^ obj) {
		try {
			obj->pushValue(L);
		}
		catch (Exceptions::DifferentLuaStatesException ^) {
			return false;
		}
		bool result = raw ? lua_rawequal(L->L, 1, -1) : lua_compare(L->L, 1, -1, LUA_OPEQ);
		lua_pop(L->L, 1);
		return result;
	}

	bool Table::Equals(Object ^ obj) {
		if (obj->GetType()->BaseType == LuaObject::typeid)
			return Equals(safe_cast<LuaObject ^>(obj));
		throw gcnew ArgumentException(String::Format("invalid object type '{0}' ('{1} inheritance expected')",
			obj->GetType(), LuaObject::typeid), "ManagedLua.Table.Equals(object obj)");
	}

	int Table::CompareTo(LuaObject ^ obj) {
		obj->pushValue(L);
		int resultLe = lua_compare(L->L, -1, 1, LUA_OPLE);
		int resultLt = lua_compare(L->L, -1, 1, LUA_OPLT);
		lua_pop(L->L, 1);
		if (resultLe && !resultLt)
			return 0;
		else if (resultLt)
			return 1;
		else
			return -1;
	}

#ifdef EXTRA

	inline bool Table::operator==(LuaObject ^ table) {
		return Equals(table);
	}

	bool Table::operator!=(LuaObject ^ table) {
		return !Equals(table);
	}

	bool Table::operator<(LuaObject ^ table) {
		table->pushValue(L);
		int result = lua_compare(L->L, 1, -1, LUA_OPLT);
		lua_pop(L->L, 1);
		return result;
	}

	bool Table::operator>(LuaObject ^ table) {
		table->pushValue(L);
		int result = lua_compare(L->L, -1, 1, LUA_OPLT);
		lua_pop(L->L, 1);
		return result;
	}

	bool Table::operator<=(LuaObject ^ table) {
		table->pushValue(L);
		int result = lua_compare(L->L, 1, -1, LUA_OPLE);
		lua_pop(L->L, 1);
		return result;
	}

	bool Table::operator>=(LuaObject ^ table) {
		table->pushValue(L);
		int result = lua_compare(L->L, -1, 1, LUA_OPLE);
		lua_pop(L->L, 1);
		return result;
	}

#endif // EXTRA

	System::Collections::IEnumerator ^ Table::GetObjectEnumerator() {
		return GetEnumerator();
	}

	int Table::Count::get() {
		lua_State * G = L->L;
		lua_pushnil(G);
		int count = 0;
		for (; lua_next(G, 1); count++) {
			lua_pop(G, 1);
		}
		return count;
	}

	bool Table::IsReadOnly::get() {
		return false;
	}

	void Table::Add(Tools::TablePair item) {
		item.Key->pushValue(L);
		item.Value->pushValue(L);
		if (raw)
			lua_rawset(L->L, 1);
		else
			lua_settable(L->L, 1);
	}

	void Table::Clear() {
		auto keys = Keys;
		lua_State * G = L->L;
		for each (LuaObject ^ key in keys) {
			key->pushValue(L);
			lua_pushnil(G);
			if (raw)
				lua_rawset(G, 1);
			else
				lua_settable(G, 1);
		}
	}

	bool Table::Contains(Tools::TablePair item) {
		item.Key->pushValue(L);
		if (raw)
			lua_rawget(L->L, 1);
		else
			lua_gettable(L->L, 1);
		item.Value->pushValue(L);
		bool result = lua_compare(L->L, -2, -1, LUA_OPEQ);
		lua_pop(L->L, 2);
		return result;
	}

	void Table::CopyTo(array<Tools::TablePair, 1> ^array, int arrayIndex) {
		int arraySize = array->GetLength(0);
		lua_State * G = L->L;
		lua_pushnil(G);
		while (lua_next(G, 1)) {
			if (arrayIndex >= arraySize) {
				lua_pop(G, 2);
				break;
			}
			array[arrayIndex] = Tools::TablePair(L->Stack[-2], L->Stack[-1]);
			lua_pop(G, 1);
			arrayIndex++;
		}
	}

	bool Table::Remove(Tools::TablePair item) {
		item.Key->pushValue(L);
		lua_State * G = L->L;
		lua_pushvalue(G, -1);
		if (raw)
			lua_rawget(G, 1);
		else
			lua_gettable(G, 1);
		item.Value->pushValue(L);
		bool result = raw ? lua_rawequal(G, 1, -1) : lua_compare(G, 1, -1, LUA_OPEQ);
		if (result) {
			lua_pop(G, 2);
			lua_pushnil(G);
			if (raw)
				lua_rawset(G, 1);
			else
				lua_settable(G, 1);
		}
		else {
			lua_pop(G, 3);
		}
		return result;
	}

	bool Table::Remove(LuaObject ^ key) {
		key->pushValue(L);
		lua_pushvalue(L->L, -1);
		bool result = raw ? (lua_rawget(L->L, 1) != LUA_TNIL) : (lua_gettable(L->L, 1) != LUA_TNIL);
		lua_pop(L->L, 1);
		lua_pushnil(L->L);
		if (raw)
			lua_rawset(L->L, 1);
		else
			lua_settable(L->L, 1);
		return result;
	}

	Collections::Generic::ICollection<LuaObject ^> ^ Table::Keys::get() {
		List<LuaObject ^> ^ keys = gcnew List<LuaObject ^>(Count);
		lua_State * G = L->L;
		lua_pushnil(G);
		for (int i = 0; lua_next(G, 1); i++) {
			keys[i] = L->Stack[-2];
			lua_pop(G, 1);
		}
		return keys;
	}

	Collections::Generic::ICollection<LuaObject ^> ^ Table::Values::get() {
		List<LuaObject ^> ^ values = gcnew List<LuaObject ^>(Count);
		lua_State * G = L->L;
		lua_pushnil(G);
		for (int i = 0; lua_next(G, 1); i++) {
			values[i] = L->Stack[-1];
			lua_pop(G, 1);
		}
		return values;
	}

	bool Table::ContainsKey(LuaObject ^ key) {
		key->pushValue(L);
		bool result = raw ? (lua_rawget(L->L, 1) != LUA_TNIL) : (lua_gettable(L->L, 1) != LUA_TNIL);
		lua_pop(L->L, 1);
		return result;
	}

	void Table::Add(LuaObject ^ key, LuaObject ^ value) {
		key->pushValue(L);
		value->pushValue(L);
		if (raw)
			lua_rawset(L->L, 1);
		else
			lua_settable(L->L, 1);
	}

	bool Table::TryGetValue(LuaObject ^ key, LuaObject ^%value) {
		key->pushValue(L);
		bool result = (raw ? lua_rawget(L->L, 1) : lua_gettable(L->L, 1)) != LUA_TNIL;
		if (result) {
			value = L->Stack[-1];
		}
		lua_pop(L->L, 1);
		return result;
	}

}