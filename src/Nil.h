/*
	project: ManagedLua
	file: Nil.h

		Contains the class structure for the 'nil' objects.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

using namespace System;

namespace ManagedLua {

	///<summary>Null value in Lua.</summary>
	public ref class Nil sealed : public LuaObject {
	internal:
		//Used to set the position in the stack
		Nil(Lua ^ state, int i);
		//Pushes 'nil' value (inherited via LuaObject)
		void pushValue(Lua ^ L) override;

	public:

		///<summary>Creates the new instance of 'nil' object value.</summary>
		Nil();

		// Inherited via LuaObject
		///<summary>Nil values always are false.</summary>
		///<returns>False.</returns>
		virtual bool ToBoolean() override;

		// Inherited via Object
		///<summary>
		///Return only 'nil' string. Don't affected by metamethods because isn't fixed with any Lua state.
		///</summary>
		///<returns>String: 'nil'.</returns>
		virtual String ^ ToString() override;
	};
}