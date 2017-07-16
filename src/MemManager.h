#pragma once
#include "config.h"
#include "ManagedLua.h"

#include "alloclua.h"

using namespace System;

namespace ManagedLua {

	namespace Tools {

		public ref class MemManager sealed : public System::Collections::Generic::IEnumerable<MemObject ^> {
		internal:
			memInfo * info;
			byte enumType;

		public:

			MemManager(size_t size);

			static property size_t HeaderSize {
				size_t get();
			}

			property size_t Size {
				size_t get();
			}

			size_t Count(Byte type);

			size_t Count(DataType type);

			size_t Count();

			size_t MeasureAll(Byte type);

			size_t MeasureAll(DataType type);

			size_t MeasureAll();

			MemManager ^ EnumerateByType(DataType type);

			MemManager ^ EnumerateByType(Byte type);

			// Inherited via IEnumerable
			virtual System::Collections::Generic::IEnumerator<MemObject ^> ^ GetEnumerator();
			virtual System::Collections::IEnumerator ^ GetEnumerator2() = System::Collections::IEnumerable::GetEnumerator;

		};

	}

}