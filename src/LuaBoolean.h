/*
	project: ManagedLua
	file: LuaBoolean.h

		Contains the class structure for 'boolean' values in Lua.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

using namespace System;

namespace ManagedLua {

	///<summary>Represents 'boolean' values in Lua.</summary>
	public ref class LuaBoolean sealed : public LuaObject {
	private:
		bool val;	//The value

	internal:
		//Constructor to sets the index for Lua object inside Lua stack
		LuaBoolean(Lua ^ state, int i);
		//Pushes boolean value on to the given stack (inherited via LuaObject)
		virtual void pushValue(Lua ^ L) override;

	public:

		///<summary>Creates new 'boolean' Lua object from Boolean value.</summary>
		///<param name="b">The Boolean value for object.</param>
		LuaBoolean(bool b);

		// Inherited via LuaObject
		///<summary>Returns the value of the object.</summary>
		///<returns>Boolean value.</returns>
		virtual bool ToBoolean() override;

		// Inherited via Object
		///<summary>
		///Represents boolean value as the string 'true' or 'false'. (not affected by the '__tostring' metamethod)
		///</summary>
		///<returns>'true' or 'false' depending on the value.</returns>
		virtual String ^ ToString() override;
	};

}