#include "stdafx.h"
#include "luaCLR.h"

#include "LightUserData.h"
#include "Table.h"
#include "Function.h"
#include "UserData.h"
#include "Lua.h"
#include "LuaObject.h"
#include "LuaException.h"
#include "EventLuaData.h"

using System::Text::Encoding;
using namespace ManagedLua;
using namespace System::Reflection;

#pragma region CommonREG

extern const char * __objects = "CLI object";
extern const char * __classes = "CLI class";
extern const char * __namespa = "namespace";

const char * error_overloads = "the property has overloads, use prop call instead";
const char * error_override = "are you trying to override the method?";
const char * error_tooFewParam = "not enough parameters for indexed property";
const char * error_replaceClass = "are you trying to replace the nested class?";
const char * error_defaultProp = "no default member";
const char * error_noMember = "no such member";
const char * error_noMethodOverload = "no method with this parameter types";
const char * error_eventOverride = "are you trying to register a new event handler?";

Object ^ __convert_stack_object_lua_(Lua ^ state, int i) {
	lua_State * L = state->L;
	switch (lua_type(L, i)) {
	case LUA_TNONE:
	case LUA_TNIL:
		return Type::Missing;
	case LUA_TBOOLEAN:
		return (bool)lua_toboolean(L, i);
	case LUA_TLIGHTUSERDATA:
		return gcnew LightUserData(lua_touserdata(L, i));
	case LUA_TNUMBER:
		if (lua_isinteger(L, i)) {
			return (int)lua_tointeger(L, i);
		}
		else {
			return lua_tonumber(L, i);
		}
	case LUA_TSTRING: {
		size_t len;
		return gcnew String(lua_tolstring(L, i, &len), 0, len);
	}
	case LUA_TTABLE:
		return gcnew Table(state, i);
	case LUA_TFUNCTION:
		return gcnew Function(state, i);
	case LUA_TUSERDATA: {
		if (lua_getmetatable(L, i)) {
			__get_object_metatable_lua_(L);
			if (lua_compare(L, -1, -2, LUA_OPEQ)) {
				lua_pop(L, 2);
				Object ^ obj = Lua::UserDatas[(*(int *)lua_touserdata(L, i))];
				return obj;
			}
			lua_pop(L, 2);
		}
		return gcnew UserData(state, i);
	}
	case LUA_TTHREAD:
		return gcnew Lua(state, i);
	}
}

void __push_object_lua_(lua_State * L, Object ^ obj) {
	if (obj == nullptr) {
		lua_pushnil(L);
		return;
	}
	Type ^ T = obj->GetType();
	if (T->BaseType == LuaObject::typeid) {
		safe_cast<LuaObject ^>(obj)->pushValue(gcnew Lua(L));
	}
	else if (T == Boolean::typeid)
		lua_pushboolean(L, safe_cast<Boolean>(obj));
	else if (T == String::typeid) {
		array<Byte> ^ encodedBytes = Encoding::UTF8->GetBytes(safe_cast<String ^>(obj));
		pin_ptr<Byte> s = &encodedBytes[0];
		lua_pushlstring(L, (char *)s, encodedBytes->Length);
	}
	else if (T == Int64::typeid)
		lua_pushinteger(L, safe_cast<Int64>(obj));
	else if (T == UInt64::typeid)
		lua_pushinteger(L, safe_cast<UInt64>(obj));
	else if (T == Int32::typeid)
		lua_pushinteger(L, safe_cast<Int32>(obj));
	else if (T == UInt32::typeid)
		lua_pushinteger(L, safe_cast<UInt32>(obj));
	else if (T == Int16::typeid)
		lua_pushinteger(L, safe_cast<Int16>(obj));
	else if (T == UInt16::typeid)
		lua_pushinteger(L, safe_cast<UInt16>(obj));
	else if (T == SByte::typeid)
		lua_pushinteger(L, safe_cast<SByte>(obj));
	else if (T == Byte::typeid)
		lua_pushinteger(L, safe_cast<Byte>(obj));
	else if (T == Single::typeid)
		lua_pushnumber(L, safe_cast<Single>(obj));
	else if (T == Double::typeid)
		lua_pushnumber(L, safe_cast<Double>(obj));
	else
		__register_object_lua_(L, obj);
}

void * __object_p_ = (void *)1;
void * __class_p_ = (void *)2;
void * __number_p_ = (void *)3;
void * __name_p_ = (void *)4;
void * __stackid_p_ = (void *)5;

int __prop_index_lua_(lua_State * L) {
	lua_rawgetp(L, 1, __stackid_p_);
	lua_rawgetp(L, 1, __number_p_);
	int stack_id = (int)lua_touserdata(L, -2) + 1;
	int n = (int)lua_touserdata(L, -1);
	lua_settop(L, 2);
	if (stack_id < n) {
		lua_pushlightuserdata(L, (void *)stack_id);
		lua_rawsetp(L, 1, __stackid_p_);
		lua_rawseti(L, 1, stack_id);
		return 1;
	}
	else {
		array<Object ^> ^ indexes = gcnew array<Object ^>(n);
		Lua ^ state = gcnew Lua(L);
		for (int i = 1; i < stack_id; i++) {
			lua_rawgeti(L, 1, i);
			indexes[i - 1] = __convert_stack_object_lua_(state, 3);
			lua_settop(L, 2);
		}
		indexes[n - 1] = __convert_stack_object_lua_(state, 2);
		try {
			Type ^ T;
			Object ^ obj;
			BindingFlags bind;
			if (lua_rawgetp(L, 1, __object_p_) == LUA_TNIL) {
				lua_rawgetp(L, 1, __class_p_);
				T = Lua::CLIClasses[*(int *)lua_touserdata(L, 4)];
				obj = nullptr;
				bind = BindingFlags::Static | BindingFlags::Public | BindingFlags::GetProperty | BindingFlags::OptionalParamBinding;
			}
			else {
				obj = Lua::UserDatas[*(int *)lua_touserdata(L, 3)];
				T = obj->GetType();
				bind = BindingFlags::Instance | BindingFlags::Static | BindingFlags::Public | BindingFlags::GetProperty | BindingFlags::OptionalParamBinding;
			}
			lua_settop(L, 2);
			lua_rawgetp(L, 1, __name_p_);
			size_t len;
			String ^ propName = gcnew String(lua_tolstring(L, 3, &len), 0, len);
			__push_object_lua_(L, T->InvokeMember(propName, bind, nullptr, obj, indexes));
			return 1;
		}
		catch (Exception ^ except) {
			__register_object_lua_(L, except);
			return lua_error(L);
		}
	}
}

int __prop_newindex_lua_(lua_State * L) {
	lua_rawgetp(L, 1, __stackid_p_);
	int stack_id = (int)lua_touserdata(L, -2) + 1;
	lua_settop(L, 2);
	array<Object ^> ^ indexes = gcnew array<Object ^>(stack_id + 1);
	Lua ^ state = gcnew Lua(L);
	indexes[stack_id] = __convert_stack_object_lua_(state, 3);
	lua_settop(L, 2);
	for (int i = 1; i < stack_id; i++) {
		lua_rawgeti(L, 1, i);
		indexes[i - 1] = __convert_stack_object_lua_(state, 3);
		lua_settop(L, 2);
	}
	indexes[stack_id - 1] = __convert_stack_object_lua_(state, 2);
	try {
		Type ^ T;
		Object ^ obj;
		BindingFlags bind;
		if (lua_rawgetp(L, 1, __object_p_) == LUA_TNIL) {
			lua_rawgetp(L, 1, __class_p_);
			T = Lua::CLIClasses[*(int *)lua_touserdata(L, 4)];
			obj = nullptr;
			bind = BindingFlags::Static | BindingFlags::Public | BindingFlags::GetProperty | BindingFlags::OptionalParamBinding;
		}
		else {
			obj = Lua::UserDatas[*(int *)lua_touserdata(L, 3)];
			T = obj->GetType();
			bind = BindingFlags::Instance | BindingFlags::Static | BindingFlags::Public | BindingFlags::GetProperty | BindingFlags::OptionalParamBinding;
		}
		lua_settop(L, 2);
		lua_rawgetp(L, 1, __name_p_);
		size_t len;
		String ^ propName = gcnew String(lua_tolstring(L, 3, &len), 0, len);
		T->InvokeMember(propName, bind, nullptr, obj, indexes);
		return 0;
	}
	catch (Exception ^ except) {
		__register_object_lua_(L, except);
		return lua_error(L);
	}
}

void __propmeta_lua_(lua_State * L, int n, int i) {
	lua_createtable(L, n, 4);
	lua_pushvalue(L, 1);
	if (__is_object_lua_(L, 1))
		lua_rawsetp(L, -2, __object_p_);
	else
		lua_rawsetp(L, -2, __class_p_);
	lua_pushlightuserdata(L, (void *)n);
	lua_rawsetp(L, -2, __number_p_);
	lua_pushlightuserdata(L, (void *)i);
	lua_rawsetp(L, -2, __stackid_p_);
	lua_pushvalue(L, 2);
	if (i) {
		lua_rawseti(L, -2, i);
		lua_pushstring(L, "Item");
	}
	lua_rawsetp(L, -2, __name_p_);

	if (lua_rawgetp(L, LUA_REGISTRYINDEX, &__propmeta_lua_) == LUA_TNIL) {
		lua_pop(L, 1);
		lua_createtable(L, 0, 2);
		lua_pushvalue(L, -1);
		lua_rawsetp(L, LUA_REGISTRYINDEX, &__propmeta_lua_);
		lua_pushcfunction(L, &__prop_index_lua_);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, &__prop_newindex_lua_);
		lua_setfield(L, -2, "__newindex");
	}

	lua_setmetatable(L, -2);
}

int __first_indexer_lua_(Type ^ T, Object ^ obj, lua_State * L) {
	switch (lua_type(L, 2)) {
	case LUA_TSTRING: {
		size_t len;
		String ^ memberName = gcnew String(lua_tolstring(L, 2, &len), 0, len);
		if (memberName == L"default") {
			lua_settop(L, 1);
			lua_pushstring(L, "Item");
			delete memberName;
			memberName = L"Item";
		}
		try {
			array<MemberInfo ^> ^ members = T->GetMember(memberName,
				BindingFlags::Public | BindingFlags::Static | BindingFlags::Instance);
			if (members->GetLength(0) == 0) {
				return luaL_argerror(L, 2, error_noMember);
			}
			MemberTypes memberType = members[0]->MemberType;
			switch (memberType) {
			case MemberTypes::Field: {
				FieldInfo ^ field = safe_cast<FieldInfo ^>(members[0]);
				__push_object_lua_(L, field->GetValue(obj));
			} break;
			case MemberTypes::Method:
				if (obj == nullptr)
					lua_pushcclosure(L, &__class_method_lua_, 2);
				else
					lua_pushcclosure(L, &__object_method_lua_, 1);
				break;
			case MemberTypes::Property: {
				if (members->GetLength(0) > 1)
					return luaL_argerror(L, 2, error_overloads);
				PropertyInfo ^ prop = safe_cast<PropertyInfo ^>(members[0]);
				int paramlen = prop->GetIndexParameters()->GetLength(0);
				if (paramlen == 0)
					__push_object_lua_(L, prop->GetValue(obj));
				else
					__propmeta_lua_(L, paramlen, 0);
			} break;
			case MemberTypes::TypeInfo:
			case MemberTypes::NestedType:
				__register_class_lua_(L, safe_cast<Type ^>(members[0]));
				break;
			case System::Reflection::MemberTypes::Event:
				__register_object_lua_(L, gcnew EventLuaData(safe_cast<EventInfo ^>(members[0]), obj));
				break;
			case System::Reflection::MemberTypes::Custom:
				__register_object_lua_(L, gcnew NotImplementedException());
				return lua_error(L);
			}
			return 1;
		}
		catch (Exception ^ except) {
			__register_object_lua_(L, except);
			return lua_error(L);
		}
	}
	default:
		try {
			PropertyInfo ^ prop = T->GetProperty("Item", BindingFlags::Public | BindingFlags::Static
				| BindingFlags::Instance);
			if (prop == nullptr)
				luaL_argerror(L, 2, error_defaultProp);
			int paramlen = prop->GetIndexParameters()->GetLength(0);
			if (paramlen == 1) {
				array<Object ^> ^ args = { __convert_stack_object_lua_(gcnew Lua(L), 2) };
				__push_object_lua_(L, T->InvokeMember("Item", BindingFlags::Public | BindingFlags::Static
					| BindingFlags::Instance | BindingFlags::GetProperty | BindingFlags::OptionalParamBinding, nullptr,
					obj, args));
				delete args;
			}
			else
				__propmeta_lua_(L, paramlen, 1);
			return 1;
		}
		catch (AmbiguousMatchException ^ except) {
			return luaL_argerror(L, 2, error_overloads);
		}
		catch (Exception ^ except) {
			__register_object_lua_(L, except);
			return lua_error(L);
		}
	}
}

int __first_newindexer_lua_(Type ^ T, Object ^ obj, lua_State * L) {
	switch (lua_type(L, 2)) {
	case LUA_TSTRING: {
		size_t len;
		String ^ memberName = gcnew String(lua_tolstring(L, 2, &len), 0, len);
		if (memberName == L"default") {
			lua_pushstring(L, "Item");
			lua_replace(L, 2);
			delete memberName;
			memberName = L"Item";
		}
		array<MemberInfo ^> ^ members = T->GetMember(memberName,
			BindingFlags::Public | BindingFlags::Static | BindingFlags::Instance);
		if (members->GetLength(0) == 0) {
			return luaL_argerror(L, 2, error_noMember);
		}
		MemberTypes memberType = members[0]->MemberType;
		switch (memberType) {
		case MemberTypes::Field: {
			FieldInfo ^ field = safe_cast<FieldInfo ^>(members[0]);
			field->SetValue(obj, __convert_stack_object_lua_(gcnew Lua(L), 3));
		} break;
		case MemberTypes::Method:
			return luaL_argerror(L, 2, error_override);
		case MemberTypes::Property: {
			if (members->GetLength(0) > 1)
				return luaL_argerror(L, 2, error_overloads);
			PropertyInfo ^ prop = safe_cast<PropertyInfo ^>(members[0]);
			int paramlen = prop->GetIndexParameters()->GetLength(0);
			if (paramlen == 0)
				prop->SetValue(obj, __convert_stack_object_lua_(gcnew Lua(L), 3));
			else
				return luaL_argerror(L, 2, error_tooFewParam);
		} break;
		case MemberTypes::TypeInfo:
		case MemberTypes::NestedType:
			return luaL_argerror(L, 2, error_replaceClass);
		case System::Reflection::MemberTypes::Event:
			return luaL_argerror(L, 2, error_eventOverride);
		case System::Reflection::MemberTypes::Custom:
			__register_object_lua_(L, gcnew NotImplementedException());
			return lua_error(L);
		}
		return 0;
	}
	default: {
		try {
			PropertyInfo ^ prop = T->GetProperty("Item", BindingFlags::Public | BindingFlags::Static
				| BindingFlags::Instance);
			if (prop == nullptr)
				luaL_argerror(L, 2, error_defaultProp);
			int paramlen = prop->GetIndexParameters()->GetLength(0);
			if (paramlen == 1) {
				Lua ^ state = gcnew Lua(L);
				array<Object ^> ^ args = { __convert_stack_object_lua_(state, 2), __convert_stack_object_lua_(state, 3) };
				__push_object_lua_(L, T->InvokeMember("Item", BindingFlags::Public | BindingFlags::Static
					| BindingFlags::Instance | BindingFlags::SetProperty | BindingFlags::OptionalParamBinding, nullptr,
					obj, args));
				delete args;
			}
			else
				return luaL_argerror(L, 2, error_tooFewParam);
			return 0;
		}
		catch (AmbiguousMatchException ^ except) {
			return luaL_argerror(L, 2, error_overloads);
		}
		catch (Exception ^ except) {
			__register_object_lua_(L, except);
			return lua_error(L);
		}
	}
	}
}

#pragma endregion

#pragma region ObjectREG

#define __id_object(L, n)\
		(*(int *)luaL_checkudata(L, n, ::__objects))

#define __check_object(L, n)\
		::ManagedLua::Lua::UserDatas[__id_object(L, n)]

int __register_object_lua_(lua_State * L, Object ^ obj) {
	int len = Lua::UserDatas->Count;
	int id;
	for (id = 0; id < len; id++) {
		if (Object::ReferenceEquals(Lua::UserDatas[id], obj)) break;
		if (Lua::UserDatas[id] == nullptr) {
			Lua::UserDatas[id] = obj;
			break;
		}
	}
	if (id == len) Lua::UserDatas->Add(obj);
	*(int *)lua_newuserdata(L, sizeof(int)) = id;
	__get_object_metatable_lua_(L);
	lua_setmetatable(L, -2);
	return id;
}

void __get_object_metatable_lua_(lua_State * L) {
	if (lua_rawgetp(L, LUA_REGISTRYINDEX, __objects) == LUA_TNIL) {
		lua_pop(L, 1);
		luaL_newmetatable(L, __objects);
		lua_pushcfunction(L, __object_gc_lua_);
		lua_setfield(L, -2, "__gc");
		lua_pushcfunction(L, __object_tostring_lua_);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, __object_eq_lua_);
		lua_setfield(L, -2, "__eq");
		lua_pushcfunction(L, __object_index_lua_);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, __object_newindex_lua_);
		lua_setfield(L, -2, "__newindex");
		lua_pushcfunction(L, __object_len_lua_);
		lua_setfield(L, -2, "__len");

		lua_pushlightuserdata(L, (void *)__objects);
		lua_setfield(L, -2, "__metatable");
		lua_pushvalue(L, -1);
		lua_rawsetp(L, LUA_REGISTRYINDEX, __objects);
	}
}

bool __is_object_lua_(lua_State * L, int i) {
	__get_object_metatable_lua_(L);
	lua_getmetatable(L, i);
	bool result = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	return result;
}

int __object_gc_lua_(lua_State * L) {
	int id = __id_object(L, 1);
	Lua::UserDatas[id] = nullptr;
	return 0;
}

int __object_tostring_lua_(lua_State * L) {
	Object ^ obj = __check_object(L, 1);
	array<Byte> ^ encodedBytes = Encoding::UTF8->GetBytes(obj->ToString());
	pin_ptr<Byte> s = &encodedBytes[0];
	lua_pushlstring(L, (char *)s, encodedBytes->Length);
	return 1;
}

int __object_eq_lua_(lua_State * L) {
	Object ^ a = __check_object(L, 1);
	Object ^ b = __check_object(L, 2);
	lua_pushboolean(L, a->Equals(b));
	return 1;
}

int __object_index_lua_(lua_State * L) {
	Object ^ obj = __check_object(L, 1);
	return __first_indexer_lua_(obj->GetType(), obj, L);
}

int __object_method_lua_(lua_State * L) {
	Object ^ obj = __check_object(L, 1);
	String ^ methodName = gcnew String(lua_tostring(L, lua_upvalueindex(1)));
	int count = lua_gettop(L) - 1;
	array<Object ^> ^ args = gcnew array<Object ^>(count);
	Lua ^ state = gcnew Lua(L);
	for (int i = 0; i < count; i++)
		args[i] = __convert_stack_object_lua_(state, i + 2);
	try {
		__push_object_lua_(L, obj->GetType()->InvokeMember(methodName,
			BindingFlags::Instance | BindingFlags::Static | BindingFlags::InvokeMethod | BindingFlags::OptionalParamBinding | BindingFlags::Public,
			nullptr, obj, args));
		delete args;
		return 1;
	}
	catch (MissingMethodException ^ except) {
		return luaL_argerror(L, 2, error_noMethodOverload);
	}
	catch (Exception ^ except) {
		__register_object_lua_(L, except);
		return lua_error(L);
	}
}

int __object_newindex_lua_(lua_State * L) {
	Object ^ obj = __check_object(L, 1);
	return __first_newindexer_lua_(obj->GetType(), obj, L);
}

int __object_len_lua_(lua_State * L) {
	Object ^ obj = __check_object(L, 1);
	PropertyInfo ^ prop = obj->GetType()->GetProperty("Length", BindingFlags::Instance | BindingFlags::Static | BindingFlags::Public);
	if (prop == nullptr) {
		char errstr[256] = "attempt to get length of ";
		{
			array<Byte> ^ encoded = Encoding::UTF8->GetBytes(obj->GetType()->ToString());
			pin_ptr<Byte> ptr = &encoded[0];
			std::memcpy(errstr + 25, ptr, encoded->GetLength(0));
		}
		return luaL_argerror(L, 1, errstr);
	}
	else
		__push_object_lua_(L, prop->GetValue(obj));
}

#pragma endregion

#pragma region ClassREG

#define __id_class(L, n)\
		(*(int *)luaL_checkudata(L, n, ::__classes))

#define __check_class(L, n)\
		::ManagedLua::Lua::CLIClasses[__id_class(L, n)]

int __register_class_lua_(lua_State * L, System::Type ^ T) {
	int len = Lua::CLIClasses->Count;
	int id;
	for (id = 0; id < len; id++) {
		if (Lua::CLIClasses[id] == T) break;
	}
	if (id == len)
		for (id = 0; id < len; id++) {
			if (Lua::CLIClasses[id] == nullptr) {
				Lua::CLIClasses[id] = T;
				break;
			}
		}
	if (id == len) Lua::CLIClasses->Add(T);
	*(int *)lua_newuserdata(L, sizeof(int)) = id;
	__get_class_metatable_lua_(L);
	lua_setmetatable(L, -2);
	return id;
}

void __get_class_metatable_lua_(lua_State * L) {
	if (lua_rawgetp(L, LUA_REGISTRYINDEX, __classes) == LUA_TNIL) {
		lua_pop(L, 1);
		luaL_newmetatable(L, __classes);
		lua_pushcfunction(L, __class_gc_lua_);
		lua_setfield(L, -2, "__gc");
		lua_pushcfunction(L, __class_tostring_lua_);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, __class_call_lua_);
		lua_setfield(L, -2, "__call");
		lua_pushcfunction(L, __class_index_lua_);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, __class_newindex_lua_);
		lua_setfield(L, -2, "__newindex");

		lua_pushlightuserdata(L, (void *)__classes);
		lua_setfield(L, -2, "__metatable");
		lua_pushvalue(L, -1);
		lua_rawsetp(L, LUA_REGISTRYINDEX, __classes);
	}
}

bool __is_class_lua_(lua_State * L, int i) {
	__get_class_metatable_lua_(L);
	lua_getmetatable(L, i);
	bool result = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	return result;
}

int __class_gc_lua_(lua_State * L) {
	int id = __id_class(L, 1);
	Lua::CLIClasses[id] = nullptr;
	return 0;
}

int __class_tostring_lua_(lua_State * L) {
	Type ^ T = __check_class(L, 1);
	array<Byte> ^ encodedBytes = Encoding::UTF8->GetBytes(T->ToString());
	pin_ptr<Byte> s = &encodedBytes[0];
	lua_pushlstring(L, (char *)s, encodedBytes->Length);
	return 1;
}

int __class_call_lua_(lua_State * L) {
	Type ^ T = __check_class(L, 1);
	int count = lua_gettop(L) - 1;
	array<Object ^> ^ args = gcnew array<Object ^>(count);
	Lua ^ state = gcnew Lua(L);
	for (int i = 0; i < count; i++)
		args[i] = __convert_stack_object_lua_(state, i + 2);
	try {
		__push_object_lua_(L, T->InvokeMember(nullptr,
			BindingFlags::DeclaredOnly | BindingFlags::Instance | BindingFlags::Static | BindingFlags::CreateInstance | BindingFlags::OptionalParamBinding | BindingFlags::Public,
			nullptr, nullptr, args));
		return 1;
	}
	catch (Exception ^ except) {
		__push_object_lua_(L, except);
		return lua_error(L);
	}
}

int __class_index_lua_(lua_State * L) {
	return __first_indexer_lua_(__check_class(L, 1), nullptr, L);
}

int __class_method_lua_(lua_State * L) {
	Type ^ T = __check_class(L, lua_upvalueindex(1));
	String ^ methodName = gcnew String(lua_tostring(L, lua_upvalueindex(2)));
	int count = lua_gettop(L);
	array<Object ^> ^ args = gcnew array<Object ^>(count);
	Lua ^ state = gcnew Lua(L);
	for (int i = 0; i < count; i++)
		args[i] = __convert_stack_object_lua_(state, i + 1);
	try {
		__push_object_lua_(L, T->InvokeMember(methodName,
			BindingFlags::Static | BindingFlags::InvokeMethod | BindingFlags::OptionalParamBinding | BindingFlags::Public,
			nullptr, nullptr, args));
		return 1;
	}
	catch (Exception ^ except) {
		__register_object_lua_(L, except);
		return lua_error(L);
	}
}

int __class_newindex_lua_(lua_State * L) {
	Object ^ obj = __check_object(L, 1);
	return __first_newindexer_lua_(obj->GetType(), obj, L);
}

#pragma endregion

#pragma region NamespaceREG

const char * __check_namespa(lua_State * L, int i, size_t * len) {
	if (__is_namespa_lua_(L, i)) {
		lua_rawgetp(L, i, __namespa);
		return lua_tolstring(L, -1, len);
	}
	luaL_checkudata(L, i, __namespa);
	return nullptr;
}

void __register_namespa_lua_(lua_State * L, const char * ns, size_t len) {
	lua_createtable(L, 0, 1);
	lua_pushlstring(L, ns, len);
	lua_rawsetp(L, -2, __namespa);
	__get_namespa_metatable_lua_(L);
	lua_setmetatable(L, -2);
}

void __get_namespa_metatable_lua_(lua_State * L) {
	if (lua_rawgetp(L, LUA_REGISTRYINDEX, __namespa) == LUA_TNIL) {
		lua_pop(L, 1);
		luaL_newmetatable(L, __namespa);
		lua_pushcfunction(L, __namespa_tostring_lua_);
		lua_setfield(L, -2, "__tostring");
		lua_pushcfunction(L, __namespa_index_lua_);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, __namespa_newindex_lua_);
		lua_setfield(L, -2, "__newindex");

		lua_pushlightuserdata(L, (void *)__namespa);
		lua_setfield(L, -2, "__metatable");
		lua_pushvalue(L, -1);
		lua_rawsetp(L, LUA_REGISTRYINDEX, __namespa);
	}
}

bool __is_namespa_lua_(lua_State * L, int i) {
	__get_namespa_metatable_lua_(L);
	lua_getmetatable(L, i);
	bool result = lua_rawequal(L, -1, -2);
	lua_pop(L, 2);
	return result;
}

int __namespa_tostring_lua_(lua_State * L) {
	__check_namespa(L, 1, nullptr);
	return 1;
}

int __namespa_index_lua_(lua_State * L) {
	size_t len;
	String ^ data = gcnew String(__check_namespa(L, 1, &len), 0, len) + L'.' + gcnew String(luaL_checklstring(L, 2, &len), 0, len);
	Type ^ t = Type::GetType(data);
	if (t == nullptr) {
		array<Byte> ^ encodedBytes = Encoding::UTF8->GetBytes(safe_cast<String ^>(data));
		pin_ptr<Byte> s = &encodedBytes[0];
		__register_namespa_lua_(L, (char *)s, encodedBytes->Length);
	}
	else {
		__register_class_lua_(L, t);
	}
	delete data;
	lua_pushvalue(L, 2);
	lua_pushvalue(L, -2);
	lua_rawset(L, 1);
	return 1;
}

int __namespa_newindex_lua_(lua_State * L) {
	lua_pushstring(L, "attempt to set values to a namespace");
	return lua_error(L);
}

#pragma endregion
