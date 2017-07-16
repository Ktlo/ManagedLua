/*
	project: ManagedLua
	file: LuaException.h

		Contains exception class which objects are thrown when Lua raises an error while compilation or runtime.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Exceptions {

		///<summary>Lua exception that is thrown when Lua raises an error while compilation or runtime.</summary>
		public ref class LuaException : public Exception {
		public:

			///<summary>Codes for every Lua standart error.</summary>
			enum class ErrorCodes : char {
				unknown = -1,
				ok = LUA_OK,
				thread_yield = LUA_YIELD,
				runtime = LUA_ERRRUN,
				syntax = LUA_ERRSYNTAX,
				memory = LUA_ERRMEM,
				garbage_collection = LUA_ERRGCMM,
				message = LUA_ERRERR,
				file = LUA_ERRFILE,
			};

		internal:
			ErrorCodes errorCode;		//Holds an error code
			LuaObject ^ errorObject;	//Holds the object that caused an exception while runtime or garbage collection

			//The messages for every error code
			static array< String ^ > ^ messages = {
#ifdef RUS
				"успешно",
				"поток приостановлен",
				"ошибка времени выполнения",
				"синтаксическая ошибка",
				"ошибка выделения памяти",
				"ошибка внутри метафункции \"__gc\"",
				"ошибка при выполнении обработчика сообщения",
				"файловая ошибка",
#else
				"success",
				"thread yielded",
				"runtime error",
				"syntax error",
				"memory allocation error",
				"error while running a \"__gc\" metamethod",
				"error while running the message handler",
				"file error",
#endif // RUS
			};
		public:

			///<summary>Creates a new Lua exception without any information about the error.</summary>
			LuaException();

			///<summary>Creates a new unknown Lua exception with message.</summary>
			///<param name="message">A error message string.</param>
			LuaException(String ^ message);

			///<summary>Creates a new Lua exception with specific code.</summary>
			///<param name="code">The code of an error.</param>
			LuaException(ErrorCodes code);

			///<summary>Creates a new Lua exception with specific code and error Lua object.</summary>
			///<param name="code">The code of an error.</param>
			///<param name="obj">The Lua object that caused the error.</param>
			LuaException(ErrorCodes code, LuaObject ^ obj);

			///<summary>Creates a new Lua exception with specific code and additional message.</summary>
			///<param name="code">The code of an error.</param>
			///<param name="message">A error message string.</param>
			LuaException(ErrorCodes code, String ^ message);

			///<summary>Creates a new Lua exception with specific code, additional message and error Lua object.</summary>
			///<param name="code">The code of an error.</param>
			///<param name="message">A error message string.</param>
			///<param name="obj">The Lua object that caused the error.</param>
			LuaException(ErrorCodes code, String ^ message, LuaObject ^ obj);

			///<summary>Creates a new Lua exception with error Lua object.</summary>
			///<param name="obj">The Lua object that caused the error.</param>
			LuaException(LuaObject ^ obj);

			///<summary>Holds the error object of the exception.</summary>
			///<value>The Lua object that caused the error.</value>
			property LuaObject ^ ErrorObject {
				LuaObject ^ get();
			}
			
			///<summary>Holds the error code. (the one that represents actual error)</summary>
			///<value>Error code.</value>
			property ErrorCodes ErrorCode {
				ErrorCodes get();
			}
		};

	}

}