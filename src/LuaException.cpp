/*
	project: ManagedLua
	file: LuaException.cpp

		Contains the definitions for LuaException methods.
*/

#include "stdafx.h"
#include "LuaException.h"

namespace ManagedLua {

	namespace Exceptions {

		inline LuaException::LuaException() :
			Exception(),
			errorObject(nullptr),
			errorCode(ErrorCodes::unknown)
		{}

		inline LuaException::LuaException(String ^ message) :
			Exception(message),
			errorObject(nullptr),
			errorCode(ErrorCodes::runtime)
		{}

		LuaException::LuaException(LuaException::ErrorCodes code) :
			Exception(messages[(int)code]),
			errorObject(nullptr),
			errorCode(code)
		{}

		LuaException::LuaException(LuaException::ErrorCodes code, LuaObject ^ obj) :
			Exception(messages[(int)code]),
			errorObject(obj),
			errorCode(code)
		{}

		LuaException::LuaException(ErrorCodes code, String ^ message) :
			Exception(messages[(int)code] + L": " + message),
			errorObject(nullptr),
			errorCode(code)
		{}

		LuaException::LuaException(ErrorCodes code, String ^ message, LuaObject ^ obj) :
			Exception(messages[(int)code] + L": " + message),
			errorObject(obj),
			errorCode(code)
		{}

		LuaException::LuaException(LuaObject ^ obj) :
			errorObject(obj),
			errorCode(ErrorCodes::runtime)
		{}

		inline LuaObject ^ LuaException::ErrorObject::get() {
			return errorObject;
		}

		inline LuaException::ErrorCodes LuaException::ErrorCode::get() {
			return errorCode;
		}

	}

}