/*
	project: ManagedLua
	file: LuaGC.h

		Contains the Lua garbage collector value class. It allows to control the garbage collection inside Lua.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"

using namespace System;

namespace ManagedLua {

	namespace Tools {

		///<summary>The Lua garbage collector manager.</summary>
		public value class LuaGC : public IEquatable<LuaGC> {
		private:
			lua_State * L;	//Holds the pointer to the main Lua thread

		internal:
			// The constructor is not public because 'Lua' objects has property 'GC' for this purpose
			LuaGC(Lua ^ state);

		public:

			///<summary>
			///Stops the garbage collection for the Lua state. (while a Lua code is being executed garbage isn't collected)
			///</summary>
			void Stop();

			///<summary>Allows the garbage collector to continue it's work and restart it.</summary>
			void Restart();

			///<summary>
			///Gets the running state of the garbage collector. (if value is true than GC is working while a Lua code is being executed)
			///</summary>
			///<value>If true than GC is working.</value>
			property bool Running {
				bool get();
			}

			///<summary>Counts the size of RAM in kilobytes that the Lua state is using.</summary>
			///<returns>The amount of RAM in kilobytes.</returns>
			int CountKB();


			///<summary>Counts the size of RAM in bytes that the Lua state is using.</summary>
			///<returns>The amount of RAM in bytes.</returns>
			int Count();

			///<summary>Performs an incremental step of garbage collection.</summary>
			void Step();

			///<summary>Performs a full garbage-collection cycle.</summary>
			void Collect();

			///<summary>Holds the pause value in percentages. (default is 200%)</summary>
			///<remarks>https://www.lua.org/manual/5.3/manual.html#2.5</remarks>
			///<value>Pause value in percentages.</value>
			property int Pause {
				int get();
				void set(int value);
			}

			///<summary>Holds the step multiplier value in percentages. (default is 200%)</summary>
			///<remarks>https://www.lua.org/manual/5.3/manual.html#2.5</remarks>
			///<value>Step multiplier value in percentages.</value>
			property int StepMultiplier {
				int get();
				void set(int value);
			}

			// Inherited via IEquatable{LuaGC}
			///<summary>Compares LuaGC with other one on equality.</summary>
			///<param name="gc">The second equatable LuaGC value.</param>
			///<returns>True if only values are the same.</returns>
			virtual bool Equals(LuaGC gc);

			// Inherited via Object
			///<summary>Compares LuaGC with the given object on equality.</summary>
			///<param name="gc">The second equatable object.</param>
			///<returns>True if only values are the same.</returns>
			bool Equals(Object ^ obj) override;

		};

	}

}