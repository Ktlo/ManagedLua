/*
	project: ManagedLua
	file: Function.h

		Contains the structure for Function class that represents 'function' objects inside Lua.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

using namespace System;

namespace ManagedLua {

	///<summary>Representation of 'function' objects in Lua.</summary>
	public ref class Function sealed : public LuaObject {
	private:
		// Just an empty method to fill the 'WhenYield' event to avoid unexpected errors.
		static void OnWhenYield(Object ^ sender, EventArgs ^ e);

	internal:
		// The theard that holds the Lua function at the first element in the stack
		Lua ^ L;
		// The list aff all user defined functions for Lua. (now has a very big problem with deleting from memory)
		static Collections::Generic::List< UserTypes::UserFunction ^ > ^ AllUserFunctions = gcnew Collections::Generic::List< UserTypes::UserFunction ^ >;
		// Internal access to 'WhenYield' event
		void InvokeEvent(EventArgs ^ args);
		// Gets the function value from the stack at the given index
		Function(Lua ^ state, int i);
		// Pushes the function value on a Lua stack (inherited via LuaObject)
		void pushValue(Lua ^ state) override;

	public:

		///<summary>Makes new Lua function from user defined function.</summary>
		///<param name="state">Lua state where a new function will be created.</param>
		///<param name="func">User defined function.</param>
		Function(Lua ^ state, UserTypes::UserFunction ^ func);

		///<summary>Calls the Lua function and returns it's result.</summary>
		///<param name="args">A function arguments.</param>
		///<returns>Lua function results.</returns>
		array< LuaObject ^ > ^ Call(... array< LuaObject ^ > ^ args);

		///<summary>Calls the Lua function and returns it's result. (allows it to yield)</summary>
		///<param name="args">A function arguments.</param>
		///<returns>Lua function results.</returns>
		array< LuaObject ^ > ^ YieldableCall(... array< LuaObject ^ > ^ args);

		///<summary>The method for <see cref="System.EventHandler" /> delegates.</summary>
		///<param name="sender">A sender object for an event.</param>
		///<param name="e">Event arguments.</param>
		void OnEvent(Object ^ sender, EventArgs ^ e);

		///<summary>The event that happen when the function is yielded.</summary>
		event EventHandler ^ WhenYield;

		// Inherited via LuaObject
		///<summary>Lua 'function' objects are always true.</summary>
		///<returns>True.</returns>
		bool ToBoolean() override;

		// Inherited via Object
		///<summary>Converts the Lua function to the equalient string that represents it.</summary>
		///<returns>The result of conversion.</returns>
		String ^ ToString() override;

	};

}