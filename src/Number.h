/*
	project: ManagedLua
	file: Number.h

		Contains Number class which can be integer or float Lua value type.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

using namespace System;

namespace ManagedLua {

	///<summary>Representation of 'number' values in Lua.</summary>
	public ref class Number sealed : public LuaObject {
	private:
		//Holds the value of a number. Because 'lua_Number' and 'lua_Integer' have the same size
		//there is no lua_Number variable type.
		lua_Integer val;
		//If the number is integer type then true.
		bool is_int;

	internal:
		//Gets the number value from the given Lua stack
		Number(Lua ^ state, int i);
		//Pushes the number on the Lua stack (inherited via LuaObject)
		virtual void pushValue(Lua ^ state) override;

	public:

		///<summary>Creates a new Lua Number object from a float number.</summary>
		///<param name="num">A float number value.</param>
		Number(double num);

		///<summary>Creates a new Lua Number object from an integer number.</summary>
		///<param name="num">An integer number value.</param>
		Number(long long num);

		///<summary>Checks whether the current number is integer.</summary>
		///<returns>True if integer.</returns>
		bool IsInt();

		///<summary>Checks whether the current number is float.</summary>
		///<returns>True if float.</returns>
		bool IsFloat();

		///<summary>Gets the value of the number in 64-bit integer form.</summary>
		///<returns>Integer value of the Lua number.</returns>
		long long ToInt64();

		///<summary>Gets the value of the number in 64-bit float form.</summary>
		///<returns>Float value of the Lua number.</returns>
		double ToDouble();

		///<summary>Converts Number object to double float value.</summary>
		///<param name="obj">Lua Number object.</param>
		static operator double(Number ^ obj);

		///<summary>Converts Number object to 64-bit integer value.</summary>
		///<param name="obj">Lua Number object.</param>
		static operator long long(Number ^ obj);

		// Inherited via LuaObject
		///<summary>Lua numbers are always true.</summary>
		///<returns>True.</returns>
		virtual bool ToBoolean() override;

		// Inherited via Object
		///<summary>
		///Represents Lua number as a string. Don't affected by metamethods because isn't fixed with any Lua state.
		///</summary>
		///<returns>String that contains number.</returns>
		String ^ ToString() override;
		
	};

}