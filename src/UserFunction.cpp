#include "stdafx.h"
#include "UserFunction.h"

#include "DataType.h"
#include "LuaObject.h"
#include "Number.h"
#include "LuaString.h"
#include "Lua.h"

namespace ManagedLua {

	namespace UserTypes {

		inline UserFunction::UserFunction() : State(nullptr), L(nullptr) {}

		long long UserFunction::CheckInteger(LuaObject ^ arg) {
			try {
				if (arg->type == DataType::number && ((Number ^)arg)->IsInt())
					return (long long)((Number ^)arg);
				else if (arg->type == DataType::luastring)
					return Int64::Parse(((LuaString ^)arg)->ToString());
			}
			catch (...) {}
			{
				char errstr[50] = "integer expected, got ";
				std::strcpy(errstr + 22, lua_typename(L, (int)arg->type));
				return luaL_argerror(L, arg->pos, errstr);
			}
		}

		String ^ UserFunction::CheckString(LuaObject ^ arg) {
			if (arg->type == DataType::luastring)
				return arg->ToString();
			else {
				char errstr[50] = "string expected, got ";
				std::strcpy(errstr + 21, lua_typename(L, (int)arg->type));
				luaL_argerror(L, arg->pos, errstr);
				return String::Empty;
			}
		}

		double UserFunction::CheckNumber(LuaObject ^ arg) {
			try {
				if (arg->type == DataType::number)
					return (double)((Number ^)arg);
				else if (arg->type == DataType::luastring)
					return Double::Parse(((LuaString ^)arg)->ToString());
			}
			catch (...) {}
			{
				char errstr[50] = "number expected, got ";
				std::strcpy(errstr + 21, lua_typename(L, (int)arg->type));
				luaL_argerror(L, arg->pos, errstr);
				return 0.0;
			}
		}

		void UserFunction::CheckType(LuaObject ^ arg, ... array< DataType > ^ types) {
			static const char _expected_[] = " expected, got ";
			for each (DataType type in types)
				if (arg->type == type)
					return;
			{
				luaL_Buffer buffer;
				luaL_buffinit(L, &buffer);
				luaL_addstring(&buffer, lua_typename(L, (int)types[0]));
				int length = types->GetLength(0);
				for (int i = 1; i < length; i++) {
					luaL_addchar(&buffer, '/');
					luaL_addstring(&buffer, lua_typename(L, (int)types[i]));
				}
				luaL_addlstring(&buffer, _expected_, sizeof(_expected_) - 1);
				luaL_addstring(&buffer, lua_typename(L, (int)arg->type));
				luaL_pushresult(&buffer);
				luaL_argerror(L, arg->pos, lua_tostring(L, -1));
			}
		}

		void UserFunction::SetState(Lua ^ state) {
			State = state;
			if (State == nullptr)
				L = nullptr;
			else
				L = State->L;
		}

		UserFunction::LuaArgException::LuaArgException(LuaObject ^ arg, String ^ message) : Exception(message) {
			_arg = arg;
		}

		UserFunction::LuaArgException::LuaArgException(LuaObject ^ arg, String ^ format_message, ...array<Object^>^ fmt) :
			Exception(String::Format(format_message, fmt)) {
			_arg = arg;
		}

		LuaObject ^ UserFunction::LuaArgException::Argument::get() {
			return _arg;
		}

	}

}