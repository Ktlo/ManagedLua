/*
	project: ManagedLua
	file: LuaObject.h

		Contains the base for Lua typed classes. It can't be inherited in other assembly
	because all it's constructors are internal, so library users can't define their own
	Lua object types like any in there.

*/

#pragma once
#include "config.h"

#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	///<summary>The base class for all classes that represents the Lua data types.</summary>
	public ref class LuaObject abstract {
	internal:
		int pos;		//The position in the Lua stack.
		DataType type;	//The type of the object.

		//Initializes Lua object with the given type id.
		LuaObject(DataType t);

		//Initializes Lua object with the given type id and sets it's position in the Lua stack.
		LuaObject(DataType t, Lua ^ state, int position);

		//Method that pushes the Lua object on the given Lua stack.
		virtual void pushValue(Lua ^ state) abstract;

	public:

		///<summary>Holds the Lua type identificator.</summary>
		///<value>Lua type id.</value>
		property DataType LuaType {
			DataType get();
		}

		///<summary>Resolves a type as the string object that is associated with it.</summary>
		///<param name="t">A type id.</param>
		///<returns>Type id as a string value.</returns>
		static String ^ GetTypeName(DataType t);

		///<summary>Holds the Lua object type id as a object value.</summary>
		///<value>Lua type id as the string.</value>
		property String ^ Typename {
			String ^ get();
		}

		///<summary>Holds the position in Lua function argument array.</summary>
		///<value>The position in argument array that begins from the element index 1.</value>
		property int Position {
			int get();
		}

		///<summary>Converts the Lua object to the boolean value.</summary>
		virtual bool ToBoolean() abstract;

		///<summary>Implicitly converts the Lua object to the boolean value.</summary>
		static operator bool(LuaObject ^ obj);
	};

}