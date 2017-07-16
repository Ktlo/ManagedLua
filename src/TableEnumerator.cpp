#include "stdafx.h"
#include "TableEnumerator.h"

#include "Lua.h"
#include "Table.h"

namespace ManagedLua {

	namespace Tools {

		TableEnumerator::TableEnumerator(Table ^ t) : table(t) {
			lua_State * L = t->L->L;
			lua_pushnil(L);
			lua_pushnil(L);
		}

		TablePair TableEnumerator::Current::get() {
			return TablePair(table->L->Stack[-2], table->L->Stack[-1]);
		}

		Object ^ TableEnumerator::ObjectCurrent::get() {
			return TablePair(table->L->Stack[-2], table->L->Stack[-1]);
		}

		bool TableEnumerator::MoveNext() {
			lua_State * L = table->L->L;
			lua_pop(L, 1);
			return lua_next(L, 1);
		}

		void TableEnumerator::Reset() {
			lua_State * L = table->L->L;
			lua_settop(L, 1);
			lua_pushnil(L);
			lua_pushnil(L);
		}

		TableEnumerator::~TableEnumerator() {
			lua_settop(table->L->L, 1);
		}

	}

}