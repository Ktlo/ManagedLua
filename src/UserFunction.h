#pragma once
#include "config.h"

#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace UserTypes {

		///<summary></summary>
		public ref class UserFunction abstract {
		internal:
			lua_State * L;

		protected:

			Lua ^ State;

			long long CheckInteger(LuaObject ^ arg);

			String ^ CheckString(LuaObject ^ arg);

			double CheckNumber(LuaObject ^ arg);

			void CheckType(LuaObject ^ arg, ... array< DataType > ^ types);

		internal:
			void SetState(Lua ^ state);

		public:

			UserFunction();

			virtual array< LuaObject ^ > ^ Call(... array< LuaObject ^ > ^ args) abstract;

			ref class LuaArgException : public Exception {
			internal:
				LuaObject ^ _arg;

			public:

				LuaArgException(LuaObject ^ arg, String ^ message);

				LuaArgException(LuaObject ^ arg, String ^ format_message, ... array< Object ^ > ^ fmt);

				property LuaObject ^ Argument {
					LuaObject ^ get();
				}

			};

		};

	}

}