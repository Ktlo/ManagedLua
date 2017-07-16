#pragma once
#include "config.h"
#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Tools {

		public ref class MemObject {
		internal:
			void * ptr;
			MemObject(void * p);

		public:

			property DataType LuaType {
				DataType get();
			}

			property Byte ObjectType {
				Byte get();
			}

			property size_t Size {
				size_t get();
			}

			String ^ ToString() override;

		};

	}

}