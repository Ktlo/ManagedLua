/*
	project: ManagedLua
	file: DifferentLuaStatesException.h

		Contains the DifferentLuaStatesException class which instances are thrown when Lua objects
	(tables, userdatas, functions and threads) from different Lua states are used together.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Exceptions {

		///<summary>
		///The exeption that is thrown when Lua objects (tables, userdatas, functions and threads)
		///from different Lua states are used together.
		///</summary>
		ref class DifferentLuaStatesException : public Exception {
		public:

			///<summary>Constructs the exception.</summary>
			DifferentLuaStatesException();

			///<summary>Constructs the exception with additional message.</summary>
			///<param name="message">A message string.</param>
			DifferentLuaStatesException(String ^ message);

		};

	}

}