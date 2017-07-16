#pragma once
#include "config.h"
#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Tools {

		public ref class TableEnumerator :
			public Collections::Generic::IEnumerator<TablePair>
		{
		private:
			Table ^ table;

		internal:
			TableEnumerator(Table ^ t);

		public:

			// Inherited via IEnumerator
			virtual property TablePair Current {
				TablePair get() override;
			}
			virtual property Object ^ ObjectCurrent {
				Object ^ get() override = System::Collections::IEnumerator::Current::get;
			}
			virtual bool MoveNext() override;
			virtual void Reset() override;
			virtual ~TableEnumerator() override;
		};

	}

}