/*
	project: ManagedLua
	file: LightUserData.h

		Contains the LightUserData class that holds 'lightuserdata' values. Class instances has different size
	depending on the bit depth of the builded library. Because 'lightuserdata' value is a pointer in Lua which
	has different size.
*/

#pragma once
#include "config.h"

#include "ManagedLua.h"
#include "LuaObject.h"

namespace ManagedLua {

	///<summary>Representation of 'lightuserdata' values in Lua.</summary>
	public ref class LightUserData sealed : public LuaObject {
	internal:
		void * val;	//value of lightuserdata
		//Gets the lightuserdata value from the Lua stack
		LightUserData(Lua ^ state, int i);
		//Push the lightuserdata value on the given stack stack (inherited via LuaObject)
		void pushValue(Lua ^ L) override;

	public:

		///<summary>Construct object from the native pointer.</summary>
		HIDE LightUserData(void * ptr);

#if _WIN64 && EXTRA

		///<summary>Create new 'lightuserdata' value from an 32-bit integer.</summary>
		///<param name="ptr">Integer value.</param>
		LightUserData(UInt32 ptr);

#endif // _WIN64 && EXTRA

		///<summary>Create new 'lightuserdata' value from an integer.</summary>
		///<param name="ptr">Integer value.</param>
		LightUserData(size_t ptr);

#if _WIN64 && EXTRA

		///<summary>Converts LightUserData object to 32-bit integer value.</summary>
		///<param name="obj">Convertible object.</param>
		///<returns>Integer value.</returns>
		static operator UInt32(LightUserData ^ obj);

#endif // _WIN64 && EXTRA

		///<summary>Converts LightUserData object to integer value.</summary>
		///<param name="obj">Convertible object.</param>
		///<returns>Integer value.</returns>
		static operator size_t(LightUserData ^ obj);

		///<summary>Converts LightUserData object to native pointer value.</summary>
		///<param name="obj">Convertible object.</param>
		///<returns>Native pointer value.</returns>
		HIDE static operator void *(LightUserData ^ obj);

		// Inherited via LuaObject
		///<summary>All Lua 'lightuserdata' values are true.</summary>
		///<returns>True.</returns>
		bool ToBoolean() override;

		// Inherited via Object
		///<summary>
		///Makes string with hexademical numer from the lightuserdata value.
		///Don't affected by metamethods because isn't fixed with any Lua state.
		///</summary>
		///<returns>String that contains number.</returns>
		String ^ ToString() override;

	};

}