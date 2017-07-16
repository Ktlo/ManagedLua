#pragma once
#include "config.h"
#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Tools {

		ref class MemManager;

		public ref class MemEnumerator : public Collections::Generic::IEnumerator<MemObject ^> {
		private:
			MemManager ^ mem;
			Byte * cur;
			Byte type;

		internal:
			MemEnumerator(MemManager ^ manager);

		public:

			// Inherited via IEnumerator
			virtual property MemObject ^ Current {
				MemObject ^ get() override;
			}
			virtual property Object ^ Current2 {
				Object ^ get() override = System::Collections::IEnumerator::Current::get;
			}
			virtual bool MoveNext() override;
			virtual void Reset() override;
			virtual ~MemEnumerator() override;

		};

	}

}