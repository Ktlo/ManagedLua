#pragma once
#include "config.h"
#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Exceptions {

		ref class UserDataException : public Exception {
		private:
			static array<String ^> ^ error_list{
#ifdef RUS
				"нельзя получить метатаблицу типа CLI",
				"тип UserData не соответствует получаемому значению",
#else
				"attempt to access the metatable of a CLI data type",
				"cant use for this UserData type",
#endif // RUS
			};

		internal:
			// Choose a message for the exception from the array
			UserDataException(int i);

		};

	}

}