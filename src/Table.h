/*
	project: ManagedLua
	file: Table.h

		Contains the class that represents a table objects in Lua. It is used like dictionary object.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"
#include "IMetatable.h"

using namespace System;

namespace ManagedLua {

	///<summary>Representation of 'table' values in Lua.</summary>
	public ref class Table sealed :
		public LuaObject,	//Inherit because it's a Lua object
		public IMetatable,	//Instances have their own metatable
		public Collections::Generic::IDictionary<LuaObject ^, LuaObject ^>,	//Lua table works like dictionary object
		public IComparable<LuaObject ^>,	//Implements comparision with other Lua objects (metatable friendly)
		public IEquatable<LuaObject ^>		//Implements equality
	{
	private:
		//If raw = true then metatable for this table will be ignored
		bool raw;
		 
	internal:
		//Holds the Lua thread. Table object uses it for access to the table inside Lua
		Lua ^ L;
		//Every table has only one enumerator for perfomance purpose.
		//As a result you can't enumerate a table while enumerating the same table.
		Tools::TableEnumerator ^ enumer;
		//Gets a table from a Lua stack at the index 'i'
		Table(Lua ^ state, int i);
		//Pushes the table value on the given stack (inherited via LuaObject)
		void pushValue(Lua ^ state) override;

	public:

		///<summary>Gets the length of the array in amount of elements. (operator "#" in Lua)</summary>
		///<value>The number of array elements.</value>
		property int Length {
			int get();
		}

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>Sets and gets a field of the table. ("tab[index]" in Lua)</summary>
		///<param name="key">A table key.</param>
		///<value>Value of the table field.</value>
		virtual property LuaObject ^ default[LuaObject ^]{
			virtual LuaObject ^ get(LuaObject ^ key);
			virtual void set(LuaObject ^ key, LuaObject ^ value);
		}

		///<summary>Sets and gets an array element of the table. ("tab[i]" in Lua)</summary>
		///<param name="i">An array index.</param>
		///<value>Value of the array element.</value>
		property LuaObject ^ default[int] {
			LuaObject ^ get(int i);
			void set(int i, LuaObject ^ value);
		}

		///<summary>Sets and gets a field of the table. ("tab.field in Lua)</summary>
		///<param name="key">A table key.</param>
		///<value>Value of the table field.</value>
		property LuaObject ^ default[String ^]{
			LuaObject ^ get(String ^ key);
			void set(String ^ key, LuaObject ^ value);
		}

		// Inherited via IEnumerable{KeyValuePair{LuaObject, LuaObject}}
		///<summary>Gets the enumerator for the table. (there can be only one for every table)</summary>
		///<returns>The enumerator for the table.</returns>
		virtual Collections::Generic::IEnumerator<Tools::TablePair> ^ GetEnumerator();

		// Inherited via IMetatable
		///<summary>
		///Allows object's methods to bypass the metatable.
		///</summary>
		///<value>If value is true metatable is ignored.</value>
		virtual property bool RawAccess {
			virtual bool get();
			virtual void set(bool access);
		}

		// Inherited via IMetatable
		///<summary>
		///Holds the metatable of the object and allows to change it.
		///</summary>
		///<value>A table.</value>
		property Table ^ Metatable {
			virtual Table ^ get();
			virtual void set(Table ^ table);
		}

		// Inherited via LuaObject
		///<summary>Table value is always true.</summary>
		///<returns>True.</returns>
		bool ToBoolean() override;

		// Inherited via Object
		///<summary>Converts object to the string representation.</summary>
		///<returns>String.</returns>
		String ^ ToString() override;

		// Inherited via IEquatable{LuaObject}
		///<summary>Checks two objects for equality.</summary>
		///<param name="obj">The second object for the equality comparision.</param>
		///<returns>The result of equality.</returns>
		virtual bool Equals(LuaObject ^ obj);

		// Inherited via Object
		///<summary>Checks two objects for equality.</summary>
		///<param name="obj">The second object for the equality comparision.</param>
		///<returns>The result of equality.</returns>
		bool Equals(Object ^ obj) override;

		// Inherited via IComparable{LuaObject}
		///<summary>Compairs two Lua objects.</summary>
		///<param name="obj">The second object for the comparision.</param>
		///<returns>The result of equality.</returns>
		virtual int CompareTo(LuaObject ^ obj);

#ifdef EXTRA

		bool operator ==(LuaObject ^ table);
		bool operator !=(LuaObject ^ table);
		bool operator <(LuaObject ^ table);
		bool operator >(LuaObject ^ table);
		bool operator <=(LuaObject ^ table);
		bool operator >=(LuaObject ^ table);

#endif // EXTRA

		// Inherited via IEnumerable
		///<summary>Gets the enumerator for the table. (there can be only one for every table)</summary>
		///<returns>The enumerator for the table.</returns>
		virtual System::Collections::IEnumerator ^ GetObjectEnumerator() = System::Collections::IEnumerable::GetEnumerator;

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>Counts fields in table.</summary>
		///<value>The amount of fields in the table.</value>
		virtual property int Count {
			virtual int get();
		}

		// Inherited via ICollection{KeyValuePair{LuaObject, LuaObject}}
		///<summary>Lua table is always not readonly object.</summary>
		///<value>False.</value>
		virtual property bool IsReadOnly {
			virtual bool get();
		}

		// Inherited via ICollection{KeyValuePair{LuaObject, LuaObject}}
		///<summary>Adds a new key-value pair to the table.</summary>
		///<param name="item">A new field.</param>
		virtual void Add(Tools::TablePair item);

		// Inherited via ICollection{KeyValuePair{LuaObject, LuaObject}}
		///<summary>Removes all fields from the table and leaves it empty.</summary>
		virtual void Clear();

		// Inherited via ICollection<KeyValuePair{LuaObject, LuaObject}}
		///<summary>Checks whether the table contains a key-value pair.</summary>
		///<param name="item">A key-value pair.</param>
		///<result>If only table contains the item then true.</result>
		virtual bool Contains(Tools::TablePair item);

		// Inherited via ICollection<KeyValuePair{LuaObject, LuaObject}}
		///<summary>Copies the table fields to an array from the given array index.</summary>
		///<param name="array">An array where the data will be copied to.</param>
		///<param name="arrayIndex">An array index which data will be copied from.</param>
		virtual void CopyTo(array<Tools::TablePair> ^ array, int arrayIndex);

		// Inherited via ICollection<KeyValuePair{LuaObject, LuaObject}}
		///<summary>Removes a key-value pair from the table.</summary>
		///<param name="item">A key-value pair.</param>
		///<returns>True if successfully removed.</returns>
		virtual bool Remove(Tools::TablePair item);

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>Removes a field from the table with key. ("tab[key] = nil" in Lua)</summary>
		///<param name="key">A key of the field that will be removed from table.</param>
		///<returns>True if successfully removed.</returns>
		virtual bool Remove(LuaObject ^ key);

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>Gets the collection of all keys in the table which values are not nil.</summary>
		///<value>All keys in the table.</value>
		virtual property Collections::Generic::ICollection<LuaObject ^> ^ Keys {
			virtual Collections::Generic::ICollection<LuaObject ^> ^ get();
		}

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>Gets the collection of all values in the table which are not nil.</summary>
		///<value>All values in the table.</value>
		virtual property Collections::Generic::ICollection<LuaObject ^> ^ Values {
			virtual Collections::Generic::ICollection<LuaObject ^> ^ get() override;
		}

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>Checks whether the table contains a field.</summary>
		///<param name="key">A field key.</param>
		///<result>If only table contains the item then true.</result>
		virtual bool ContainsKey(LuaObject ^ key);

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>Adds a field to the table. ("tab[key] = value" in Lua)</summary>
		///<param name="key">A field key.</param>
		///<param name="value">A value for the field.</param>
		virtual void Add(LuaObject ^ key, LuaObject ^ value);

		// Inherited via IDictionary{LuaObject, LuaObject}
		///<summary>
		///Gets the value from the table with key "<paramref name="key" />" and returns true if the value is not nil.
		///</summary>
		///<param name="key">The key to access a field.</param>
		///<param name="value">The value of the field.</param>
		///<returns>True if the value is not nil.s</returns>
		virtual bool TryGetValue(LuaObject ^ key, LuaObject ^% value);

	};

}