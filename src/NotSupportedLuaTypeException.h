/*
	project: ManagedLua
	file: NotSupportedLuaTypeException.h

		Contains the structure for NotSupportedLuaTypeException class.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Exceptions {

		///<summary>The exception is thrown when the Lua boject type is not supported in the method.</summary>
		public ref class NotSupportedLuaTypeException sealed : public Exception {
		public:

			///<summary>Creates exception for specific type id.</summary>
			///<param name="type">A Lua type id.</param>
			NotSupportedLuaTypeException(DataType type);

			///<summary>Creates exception for specific type id with an additional message.</summary>
			///<param name="type">A Lua type id.</param>
			///<param name="message">An additional message for the exeption.</param>
			NotSupportedLuaTypeException(DataType type, String ^ message);

		};

	}

}