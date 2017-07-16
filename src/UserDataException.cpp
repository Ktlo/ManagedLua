#include "stdafx.h"
#include "UserDataException.h"

namespace ManagedLua {

	namespace Exceptions {

		UserDataException::UserDataException(int i) : Exception(error_list[i]) {}

	}
	
}