/*
	project: ManagedLua
	file: LuaString.h

		Contains the structure of LuaString class. Lua strings encodes as utf-8 text. This makes perfomance problem
	to convert them to default string object and vice versa.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

using namespace System;

namespace ManagedLua {

	///<summary>Representation of 'string' values in Lua.</summary>
	public ref class LuaString sealed : public LuaObject {
	private:
		//The length of the string
		size_t len;
		//The string value
		char * c_str_val;

	internal:
		//Gets the string value from the given Lua stack
		LuaString(Lua ^ state, int i);
		//Pushes the string value on the Lua stack (inherited via LuaObject)
		void pushValue(Lua ^ state) override;

	public:

		///<summary>Creates a new string from a C string.</summary>
		///<param name="str">A C string.</summary>
		HIDE LuaString(const char * str);

		///<summary>Creates a new string from a C sting with the <paramref name="length" /> size.</summary>
		///<param name="str">A C string.</param>
		///<param name="length">The length of string <paramref name="str" />.</param>
		HIDE LuaString(const char * str, size_t length);

		///<summary>Creates a new Lua string from the string object.</summary>
		///<param name="str">A string object.</param>
		///<exception cref="System.ArgumentNullException" />
		LuaString(String ^ str);

		///<summary>Holds the string length.</summary>
		///<value>String length in bytes.</value>
		property size_t Length {
			size_t get();
		}

		///<summary>Holds the value of LuaString object in C style.</summary>
		///<value>The C string.</value>
		property const char * CString {
			HIDE const char * get();
		}

		// Inherited via LuaObject
		///<summary>Lua string valueis always true.</summary>
		///<returns>True.</returns>
		virtual bool ToBoolean() override;

		// Inherited via Object
		///<summary>
		///Converts the Lua utf-8 string value to the utf-16 string object.
		///Don't affected by metamethods because isn't fixed with any Lua state.
		///</summary>
		///<returns>Converted string.</returns>
		virtual String ^ ToString() override;

		///<summary>Deletes string from memory.</summary>
		!LuaString();
	};

}