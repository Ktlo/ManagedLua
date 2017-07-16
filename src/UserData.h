#pragma once
#include "config.h"
#include "ManagedLua.h"

#include "LuaObject.h"
#include "IMetatable.h"

namespace ManagedLua {

	///<summary>Representation of 'userdata' objects in Lua.</summary>
	public ref class UserData sealed :
		public LuaObject,		// UserData is the Lua object
		public IMetatable,		// It has it's own metatable
		public IEquatable<LuaObject ^>,	// It can be compared on equality with other LuaObjects.
		public IComparable<LuaObject ^>	// It also can be compared with comparative operators
	{
	public:

		///<summary>The enumeration of userdata types that can be in ManagedLua.</summary>
		enum class UserDataType : Byte {
			third_party = 0,	///<summary>The ID of a userdata that exist only inside Lua.</summary>
			object_type = 1,	///<summary>The ID of the userdata that represents a CLI object.</summary>
			class_type = 2,		///<summary>The ID of the userdata that represents a CLI class.</summary>
			ns_type = 3,		///<summary>The ID of the userdata that represents a namespace.</summary>
		};

	internal:
		// The lua theard that holds a 'third_party' userdata
		Lua ^ L;
		// The CLI object index in 'Lua.UserDatas' or the index of the class in 'Lua.CLIClasses'
		// or the namespace string (depending on UserData type ID)
		Object ^ id;
		// The type of UserData
		UserDataType data;
		// If raw == true than the metatable is ignored
		bool raw;
		// Gets the value of the userdata from the given Lua stack at the given index
		UserData(Lua ^ state, int i);
		// Pushes the userdata value on a Lua stack (inherited via LuaObject)
		void pushValue(Lua ^ state) override;

	public:

		///<summary>Transforms an object to <see cref="ManagedLua.UserData" /> object that can be used inside Lua.</summary>
		///<param name="obj">A CLI object.</param>
		///<returns>The userdata with object type.</returns>
		static UserData ^ FromObject(Object ^ obj);

		///<summary>Transforms a class name to <see cref="ManagedLua.UserData" /> object that can be used inside Lua.</summary>
		///<typeparam name="T">A class name.</typeparam>
		///<returns>The userdata with class type.</returns>
		generic <class T>
		static UserData ^ FromClass();

		///<summary>Transforms an object type to <see cref="ManagedLua.UserData" /> object that can be used inside Lua.</summary>
		///<param name="type">The information about a type.</param>
		///<returns>The userdata with class type.</returns>
		static UserData ^ FromClass(Type ^ type);

		///<summary>
		///Transforms a namespace name string to <see cref="ManagedLua.UserData" /> object that can be used inside Lua.
		///</summary>
		///<returns>The userdata with namespace type.</returns>
		static UserData ^ FromNamespace(String ^ ns);

		///<summary>Returns the type of the userdata.</summary>
		///<value>The userdata type ID.</value>
		property UserDataType UDType {
			UserDataType get();
		}

		///<summary>
		///Returns the object that the userdata contains if the type of the userdata equals to 'object_type'.
		///</summary>
		///<returns>The object that was represented like userdata.</returns>
		Object ^ GetObject();

		///<summary>
		///Returns the class type that the userdata contains if the type of the userdata equals to 'class_type'.
		///</summary>
		///<returns>The type of the class that was represented like userdata.</returns>
		Type ^ GetClassType();

		///<summary>
		///Returns the namespace string that the userdata contains if the type of the userdata equals to 'ns_type'.
		///</summary>
		String ^ GetNamespace();

		///<summary>Userdata values are always true.</summary>
		///<returns>True.</returns>
		bool ToBoolean() override;

		///<summary>Converts the Lua userdata object to the equal string.</summary>
		String ^ ToString() override;

		// Inherited via IMetatable
		///<summary>Allows object's methods to bypass the metatable.</summary>
		///<value>If value is true metatable is ignored.</value>
		virtual property bool RawAccess {
			virtual bool get();
			virtual void set(bool value);
		}

		// Inherited via IMetatable
		///<summary>Holds the metatable of the object and allows to change it.</summary>
		///<value>A table.</value>
		virtual property Table ^ Metatable {
			virtual Table ^ get();
			virtual void set(Table ^ tab);
		}

		// Inherited via IEquatable{LuaObject}
		///<summary>Checks two objects for equality.</summary>
		///<param name="obj">The second object for the equality comparision.</param>
		///<returns>The result of equality.</returns>
		virtual bool Equals(LuaObject ^ other);

		// Inherited via Object
		///<summary>Checks two objects for equality.</summary>
		///<param name="obj">The second object for the equality comparision.</param>
		///<returns>The result of equality.</returns>
		virtual bool Equals(Object ^ other) override;

		// Inherited via IComparable{LuaObject}
		///<summary>Compairs two Lua objects.</summary>
		///<param name="obj">The second object for the comparision.</param>
		///<returns>The result of equality.</returns>
		virtual int CompareTo(LuaObject ^ other) override;

	private:
		// Constructor for every UserDataType exept 'third_party'
		UserData(UserDataType d, Object ^ id);

	};

}