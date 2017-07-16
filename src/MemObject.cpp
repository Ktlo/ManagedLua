#include "stdafx.h"
#include "MemObject.h"

#include "DataType.h"

#include "alloclua.h"

namespace ManagedLua {

	namespace Tools {

		inline MemObject::MemObject(void * p) : ptr(p) {}

		String ^ MemObject::ToString() {
			return String::Format("memory object of {0} type that weights {1} bytes is situated at 0x{2}",
				(DataType)gettype(ptr), lengthof(ptr) - DELTA, ((size_t)getptr((byte *)ptr)).ToString("x"));
		}

		DataType MemObject::LuaType::get() {
			return (DataType)gettype(ptr);
		}

		Byte MemObject::ObjectType::get() {
			return gettype(ptr);
		}

		size_t MemObject::Size::get() {
			return lengthof(ptr) - DELTA;
		}

	}

}