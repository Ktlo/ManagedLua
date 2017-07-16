#pragma once

#include "config.h"
#include "ManagedLua.h"

using namespace System;

extern const char * __objects;
extern const char * __classes;
extern const char * __namespa;

System::Object ^ __convert_stack_object_lua_(ManagedLua::Lua ^ state, int i);
void __push_object_lua_(lua_State * L, System::Object ^ obj);
int __prop_index_lua_(lua_State * L);
int __prop_newindex_lua_(lua_State * L);
void __propmeta_lua_(lua_State * L, int n, int i);
int __first_indexer_lua_(Type ^ T, Object ^ obj, lua_State * L);
int __first_newindexer_lua_(Type ^ T, Object ^ obj, lua_State * L);

int __register_object_lua_(lua_State * L, System::Object ^ obj);
void __get_object_metatable_lua_(lua_State * L);
bool __is_object_lua_(lua_State * L, int i);
int __object_gc_lua_(lua_State * L);
int __object_tostring_lua_(lua_State * L);
int __object_eq_lua_(lua_State * L);
int __object_index_lua_(lua_State * L);
int __object_method_lua_(lua_State * L);
int __object_newindex_lua_(lua_State * L);
int __object_len_lua_(lua_State * L);

int __register_class_lua_(lua_State * L, System::Type ^ T);
void __get_class_metatable_lua_(lua_State * L);
bool __is_class_lua_(lua_State * L, int i);
int __class_gc_lua_(lua_State * L);
int __class_tostring_lua_(lua_State * L);
int __class_call_lua_(lua_State * L);
int __class_index_lua_(lua_State * L);
int __class_method_lua_(lua_State * L);
int __class_newindex_lua_(lua_State * L);

void __register_namespa_lua_(lua_State * L, const char * ns, size_t len);
void __get_namespa_metatable_lua_(lua_State * L);
bool __is_namespa_lua_(lua_State * L, int i);
int __namespa_tostring_lua_(lua_State * L);
int __namespa_index_lua_(lua_State * L);
int __namespa_newindex_lua_(lua_State * L);