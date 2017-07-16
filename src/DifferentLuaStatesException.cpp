/*
	project: ManagedLua
	file: DifferentLuaStatesException.cpp

		Contains the definitions for the constructors of the 'DifferentLuaStatesException'.
*/

#include "stdafx.h"
#include "DifferentLuaStatesException.h"

namespace ManagedLua {

	namespace Exceptions {

		DifferentLuaStatesException::DifferentLuaStatesException() :
			Exception("Cannot use a lua objects from a different states")
		{}

		DifferentLuaStatesException::DifferentLuaStatesException(String ^ message) :
			Exception(String::Format("Cannot use a lua objects from a different states ({0})", message))
		{}

	}

}