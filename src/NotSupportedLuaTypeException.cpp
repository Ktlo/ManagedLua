/*
	project: ManagedLua
	file: NotSupportedLuaTypeException.cpp

	Contains the constructor for NotSupportedLuaTypeException class.
*/

#include "stdafx.h"
#include "NotSupportedLuaTypeException.h"

#include "LuaObject.h"

namespace ManagedLua {

	namespace Exceptions {

		NotSupportedLuaTypeException::NotSupportedLuaTypeException(DataType type) :
			Exception(String::Format("The Lua type '{0}' is not supported here", LuaObject::GetTypeName(type)))
		{}

		NotSupportedLuaTypeException::NotSupportedLuaTypeException(DataType type, String ^ message) :
			Exception(String::Format("The Lua type '{0}' is not supported here: {1}", LuaObject::GetTypeName(type), message))
		{}

	}

}