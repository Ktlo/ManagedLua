#pragma once
#include "config.h"
#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace UserTypes {

		public ref class LuaYieldEventArgs : public EventArgs, public System::Collections::IEnumerable {
		private:
			array<LuaObject ^> ^ args;
			Collections::Generic::List<LuaObject ^> ^ results;
			Lua ^ state;

		internal:
			LuaYieldEventArgs(Lua ^ L);
			void PushOnStack();

		public:

			property LuaObject ^ default[int]{
				LuaObject ^ get(int i) {
				return args[i];
			}
			}

				property int Length {
				int get() {
					return args->GetLength(0);
				}
			}

			void PushResult(LuaObject ^ obj);

			virtual System::Collections::IEnumerator ^ GetEnumerator();

		};

	}

}