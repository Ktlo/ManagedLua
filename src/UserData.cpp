#include "stdafx.h"
#include "UserData.h"

#include "DataType.h"
#include "LuaString.h"
#include "Table.h"
#include "Lua.h"
#include "DifferentLuaStatesException.h"
#include "UserDataException.h"

#include "luaCLR.h"

namespace ManagedLua {

	INLINE UserData::UserData(Lua ^ state, int i) : LuaObject(DataType::userdata, state, i) {
		L = state->NewThread();
		state->CopyTo(L, i);
		lua_State * G = L->L;
		if (__is_object_lua_(G, 1)) {
			data = UserDataType::object_type;
			id = *(int *)lua_touserdata(G, 1);
		}
		else if (__is_class_lua_(G, 1)) {
			data = UserDataType::class_type;
			id = *(int *)lua_touserdata(G, 1);
		}
		else if (__is_namespa_lua_(G, 1)) {
			data = UserDataType::ns_type;
			lua_rawgetp(G, 1, __namespa);
			size_t len;
			id = gcnew String(lua_tolstring(G, -1, &len), 0, len);
		}
		else
			data = UserDataType::third_party;
		if (data != UserDataType::third_party)
			id = *(int *)lua_touserdata(G, 1);
		raw = false;
	}

	UserData::UserData(UserDataType d, Object ^ i) : LuaObject(DataType::userdata), L(nullptr) {
		data = d;
		id = i;
		raw = false;
	}

	UserData ^ UserData::FromObject(Object ^ obj) {
		int id;
		int len = Lua::UserDatas->Count;
		for (id = 0; id < len; id++) {
			if (Object::ReferenceEquals(Lua::UserDatas[id], obj)) break;
			if (Lua::UserDatas[id] == nullptr) {
				Lua::UserDatas[id] = obj;
				break;
			}
		}
		if (id == len) Lua::UserDatas->Add(obj);
		return gcnew UserData(UserDataType::object_type, id);
	}

	generic<class T>
	inline UserData ^ UserData::FromClass() {
		return FromClass(T::typeid);
	}

	UserData ^ UserData::FromClass(Type ^ type) {
		int id;
		int len = Lua::CLIClasses->Count;
		for (id = 0; id < len; id++) {
			if (Lua::CLIClasses[id] == type) break;
		}
		if (id == len)
			for (id = 0; id < len; id++) {
				if (Lua::CLIClasses[id] == nullptr) {
					Lua::CLIClasses[id] = type;
					break;
				}
			}
		if (id == len) Lua::CLIClasses->Add(type);
		return gcnew UserData(UserDataType::class_type, id);
	}

	UserData ^ UserData::FromNamespace(String ^ ns) {
		return gcnew UserData(UserDataType::ns_type, ns);
	}

	inline UserData::UserDataType UserData::UDType::get() {
		return data;
	}

	Object ^ UserData::GetObject() {
		if (data != UserDataType::object_type)
			throw gcnew Exceptions::UserDataException(1);
		return Lua::UserDatas[safe_cast<int>(id)];
	}

	Type ^ UserData::GetClassType() {
		if (data != UserDataType::class_type)
			throw gcnew Exceptions::UserDataException(1);
		return Lua::CLIClasses[safe_cast<int>(id)];
	}

	String ^ UserData::GetNamespace() {
		if (data != UserDataType::ns_type)
			throw gcnew Exceptions::UserDataException(1);
		return safe_cast<String ^>(id);
	}

	void UserData::pushValue(Lua ^ state) {
		if (data != UserDataType::third_party) {
			if (data == UserDataType::object_type) {
				*(int *)lua_newuserdata(state->L, sizeof(int)) = safe_cast<int>(id);
				__get_object_metatable_lua_(state->L);
			}
			else if (data == UserDataType::class_type) {
				*(int *)lua_newuserdata(state->L, sizeof(int)) = safe_cast<int>(id);
				__get_class_metatable_lua_(state->L);
			}
			else if (data == UserDataType::ns_type) {
				lua_createtable(state->L, 0, 1);
				LuaString(safe_cast<String ^>(id)).pushValue(state);
				lua_rawsetp(state->L, -2, __namespa);
				__get_namespa_metatable_lua_(state->L);
			}
			lua_setmetatable(state->L, -2);
		}
		else {
			if (!Lua::CompareStates(L, state))
				throw gcnew Exceptions::DifferentLuaStatesException;
			L->CopyTo(state, 1);
		}
	}

	bool UserData::ToBoolean() {
		return true;
	}

	String ^ UserData::ToString() {
		switch (data) {
		case UserDataType::third_party: {
			size_t len;
			auto str = gcnew String(luaL_tolstring(L->L, 1, &len), 0, len);
			lua_pop(L->L, 1);
			return str;
		}
		case UserDataType::object_type:
			return Lua::UserDatas[safe_cast<int>(id)]->ToString();
		case UserDataType::class_type:
			return Lua::CLIClasses[safe_cast<int>(id)]->ToString();
			//case UserDataType::ns_type:
		}
	}

	bool UserData::RawAccess::get() {
		return raw;
	}

	void UserData::RawAccess::set(bool value) {
		raw = value;
	}

	Table ^ UserData::Metatable::get() {
		if (data != UserDataType::third_party)
			throw gcnew Exceptions::UserDataException(0);
		lua_getmetatable(L->L, 1);
		Table ^ result = gcnew Table(L, -1);
		lua_pop(L->L, 1);
		return result;
	}

	void UserData::Metatable::set(Table ^ tab) {
		if (data != UserDataType::third_party)
			throw gcnew Exceptions::UserDataException(0);
		if (!Lua::CompareStates(tab->L, L))
			throw gcnew Exceptions::DifferentLuaStatesException;
		lua_State * G = tab->L->L;
		lua_pushvalue(G, 1);
		lua_xmove(G, L->L, 1);
		lua_setmetatable(L->L, 1);
	}

	bool UserData::Equals(LuaObject ^other) {
		try {
			other->pushValue(L);
		}
		catch (Exceptions::DifferentLuaStatesException ^) {
			return false;
		}
		bool result = raw ? lua_rawequal(L->L, 1, -1) : lua_compare(L->L, 1, -1, LUA_OPEQ);
		lua_pop(L->L, 1);
		return result;
	}

	bool UserData::Equals(Object ^other) {
		if (other->GetType()->BaseType == LuaObject::typeid)
			return Equals(safe_cast<LuaObject ^>(other));
		throw gcnew ArgumentException(String::Format("invalid object type '{0}' ('{1} inheritance expected')",
			other->GetType(), LuaObject::typeid), "ManagedLua.UserData.Equals(object obj)");
	}

	int UserData::CompareTo(LuaObject ^other) {
		other->pushValue(L);
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

}