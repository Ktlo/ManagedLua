/*
	project: ManagedLua
	file: Lua.h

		Contains the Lua class that represents the Lua states and Lua threads. Also it is used for complex
	Lua objects such as functions, tables and userdatas to access them inside Lua.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;

namespace ManagedLua {

	///<summary>The main class of this library, describes objects that represents Lua states and Lua threads.</summary>
	public ref class Lua sealed : public LuaObject {
	private:
		static const void * const address = "";	//The address in registry of the all lua threads in the state
		//The Lua object that greated this object (null if it is the main thread or if it is the function call object)
		Lua ^ parentThread;
		//Memory manager that allocates memory for the state (need to hold it
		//somewhere to avoid the destruction by the garbage collector)
		Tools::MemManager ^ mem;
		//Pushes the sample data on the Lua stack (only for type metatables)
		void PushSampleType(DataType type);

	internal:
		//True if Lua object was created by function call
		bool funcCallState = false;
		//Original Lua state
		lua_State * L;
		//Constructs new Lua object by Lua call state
		Lua(lua_State * state);
		//Cunstructs new Lua object from parent Lua state
		Lua(Lua ^ parent);
		//Special constructor for access the Lua thread from the Lua stack.
		Lua(Lua ^ state, int i);
		//Gets an object of the value at the given index in the stack.
		property LuaObject ^ Stack[int]{
			LuaObject ^ get(int i);
		};
		//Pops the last value from the Lua stack and returns it's object.
		LuaObject ^ Pop();
		//Copies the value at the given index to the given thread of the same state.
		void CopyTo(Lua ^ thread, int i);
		// Inherited via LuaObject (pushes the thread on a Lua stack)
		void pushValue(Lua ^ state) override;

		//The collection of all objects that Lua states are using.
		static List< Object ^ > ^ UserDatas = gcnew List< Object ^ >;

		//The collection of all .NET classes that Lua states are using.
		static List< System::Type ^ > ^ CLIClasses = gcnew List< System::Type ^ >;

	public:

		///<summary>The ways to load Lua code.</summary>
		enum class LoadMode : char {
			both,		///<summary>Loadable code is represented as a text or a bytecode.</summary>
			text,		///<summary>Loadable code is represented as a text.</summary>
			bytecode,	///<summary>Loadable code is represented as a bytecode.</summary>
		};

		///<summary>Creates a new Lua state that allocates memory directly from the system.</summary>
		explicit Lua();

		///<summary>
		///Creates a new Lua state that allocates memory from the preallocated
		///block in <see cref="ManagedLua.Tools.MemManager" /> object.
		///</summary>
		///<param name="manager">The memory manager object.</param>
		explicit Lua(Tools::MemManager ^ manager);

		///<summary>Returns the main thread of the given Lua state.</summary>
		///<returns>Lua thread.</returns>
		Lua ^ GetMainThread();

		///<summary>
		///Returns the parent thread of the given Lua state.
		///Returns null if the thread is the main thread or function call stack.
		///</summary>
		///<returns>Lua thread.</returns>
		Lua ^ GetParentThread();

		///<summary>Makes a new Lua thread.</summary>
		///<returns>Lua thread.</returns>
		Lua ^ NewThread();

		///<summary>Checks whether the thread is the main one.</summary>
		///<returns>True if it is the main thread.</returns>
		bool IsMainThread();

		///<summary>Holds the Lua garbage collector for the Lua state</summary>
		///<value>The Lua Garbage Collector object.</value>
		property Tools::LuaGC ^ GC {
			Tools::LuaGC ^ get();
		}

		///<summary>Loads the standart functions and Lua libraries to the Lua state.</summary>
		void OpenLibs();

		///<summary>Loads Lua library for .NET objects to the global environment.</summary>
		void OpenManegedLib();

		///<summary>Returns the global table ("_G" in Lua).</summary>
		///<returns>The global table.</returns>
		Table ^ GetGlobal();

		///<summary>Returns the registry table.</summary>
		///<returns>The registry table.</returns>
		Table ^ GetRegistry();

		///<summary>Creates a new emprty Lua table with preallocated space.</summary>
		///<param name="narr">A number of the array elements.</param>
		///<param name="nrec">A number of key - value pairs.</param>
		///<returns>The created table.</returns>
		Table ^ NewTable(size_t narr, size_t nrec);

		///<summary>Creates a new emprty Lua table.</summary>
		///<returns>The created table.</returns>
		Table ^ NewTable();

#ifdef DEBUG

		//Prints all element types to the console
		void StackState() {
			int len = lua_gettop(L);
			Console::WriteLine("Begin stack state:");
			for (int i = 1; i <= len; i++) {
				Console::WriteLine("->" + LuaObject::GetTypeName((DataType)lua_type(L, i)));
			}
		}

#endif // DEBUG

		///<summary>Gets the type fixed metatable.</summary>
		///<param name="type">A Lua type id. (Must be 'nil', 'boolean', 'lightuserdata',
		/// 'number', 'string', 'function' or 'thread')</param>
		///<returns>The type fixed metatable or null if type hasn't it.</returns>
		Table ^ GetTypeMetatable(DataType type);

		///<summary>Sets a new metatable for the Lua type.</summary>
		///<param name="type">A Lua type id. (Must be 'nil', 'boolean', 'lightuserdata',
		/// 'number', 'string', 'function' or 'thread')</param>
		///<param name="table">The new metatable or null.</param>
		void SetTypeMetatable(DataType type, Table ^ table);

		///<summary>Loads a Lua code from a stream.</summary>
		///<param name="reader">A stream where the data will be read.</param>
		///<param name="name">A name of code chunk.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentException" />
		///<exception cref="System.ArgumentNullException" />
		///<exception cref="System.ArgumentOutOfRangeException" />
		///<exception cref="System.IO.IOException" />
		///<exception cref="System.NotSupportedException" />
		///<exception cref="System.ObjectDisposedException" />
		Function ^ Load(Stream ^ reader, String ^ name, LoadMode mode);

		///<summary>Loads a Lua code from a stream.</summary>
		///<param name="reader">A stream where the data will be read.</param>
		///<param name="name">A name of code chunk.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentException" />
		///<exception cref="System.ArgumentNullException" />
		///<exception cref="System.ArgumentOutOfRangeException" />
		///<exception cref="System.IO.IOException" />
		///<exception cref="System.NotSupportedException" />
		///<exception cref="System.ObjectDisposedException" />
		Function ^ Load(Stream ^ reader, String ^ name);

		///<summary>Loads a Lua code from a stream.</summary>
		///<param name="reader">A stream where the data will be read.</param>
		///<param name="mode">A way to read code.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentException" />
		///<exception cref="System.ArgumentNullException" />
		///<exception cref="System.ArgumentOutOfRangeException" />
		///<exception cref="System.IO.IOException" />
		///<exception cref="System.NotSupportedException" />
		///<exception cref="System.ObjectDisposedException" />
		Function ^ Load(Stream ^ reader, LoadMode mode);

		///<summary>Loads a Lua code from a stream.</summary>
		///<param name="reader">A stream where the data will be read.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentException" />
		///<exception cref="System.ArgumentNullException" />
		///<exception cref="System.ArgumentOutOfRangeException" />
		///<exception cref="System.IO.IOException" />
		///<exception cref="System.NotSupportedException" />
		///<exception cref="System.ObjectDisposedException" />
		Function ^ Load(Stream ^ reader);

		///<summary>Loads a Lua code from a string.</summary>
		///<param name="str">A string which the Lua code will be readed from.</param>
		///<param name="name">A name of code chunk.</param>
		///<param name="mode">A way to read code.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentNullException" />
		Function ^ LoadString(String ^ str, String ^ name, LoadMode mode);

		///<summary>Loads a Lua code from a string.</summary>
		///<param name="str">The string which the Lua code will be readed from.</param>
		///<param name="name">The name of code chunk.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentNullException" />
		Function ^ LoadString(String ^ str, String ^ name);

		///<summary>Loads a Lua code from a string.</summary>
		///<param name="str">The string which the Lua code will be readed from.</param>
		///<param name="mode">The way to read code.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentNullException" />
		Function ^ LoadString(String ^ str, LoadMode mode);

		///<summary>Loads a Lua code from a string.</summary>
		///<param name="str">The string which the Lua code will be readed from.</param>
		///<returns>The Lua function that can interpretate the loaded code.</returns>
		///<exception cref="ManagedLua.Exceptions.LuaException" />
		///<exception cref="System.ArgumentNullException" />
		Function ^ LoadString(String ^ str);

		///<summary>Compares the Lua states of a two given Lua objects.</summary>
		///<param name="a">The first Lua object.</param>
		///<param name="b">The second Lua object.</param>
		///<returns>The result of equality.</returns>
		static bool CompareStates(Lua ^ a, Lua ^ b);

		// Inherited via LuaObject
		///<summary>Lua thread is always true.</summary>
		///<returns>true</returns>
		virtual bool ToBoolean() override;

		// Inherited via Object
		///<summary>The string representation of the Lua object. The method calls
		///"__tostring" metamethod inside Lua if it exists.</summary>
		virtual String ^ ToString() override;

		!Lua();
	};

}