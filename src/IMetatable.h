/*
	project: ManagedLua
	file: IMetatable.h

		Contains the metatable interface for the objects with their own metatable.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

using namespace System;

namespace ManagedLua {

	///<summary>
	///Is used in Lua data type with object fixed metatable. (i.e. every object of that type have their own metatable)
	///</summary>
	public interface class IMetatable {
	public:

		///<summary>Allows object's methods to bypass the metatable.</summary>
		///<value>If value is true metatable is ignored.</value>
		virtual property bool RawAccess {
			virtual bool get() abstract;
			virtual void set(bool value) abstract;
		}

		///<summary>Holds the metatable of the object and allows to change it.</summary>
		///<value>A table.</value>
		virtual property Table ^ Metatable {
			virtual Table ^ get() abstract;
			virtual void set(Table ^ tab) abstract;
		}

	};

}