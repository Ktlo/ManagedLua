/**********************************************************\
| Project: "ManagedLua.dll"                                |
| File: "config.h"                                         |
|##########################################################|
|   Contains preprocessor derectives, libraries to include |
| and disables some VisualC++ compile errors.              |
|##########################################################|
| Author: Ktlo                                             |
| Date: 04.03.2017 23:32                                   |
\**********************************************************/

#ifndef __LIB_CONFIG
#define __LIB_CONFIG

#pragma warning( disable : 4996 )
#pragma warning( disable : 4302 )
#pragma warning( disable : 4244 )
#pragma comment( lib, "lua53.lib" )

//The main and absolute namespace.
#define BRIGHT_CUBE_BEGIN namespace bright_cube {
#define BRIGHT_CUBE_END }

//Exports
#define CLRLUA_LIB __declspec( dllexport )

//For maybe-inline methods
#define INLINE inline

//COM Invisible
#define HIDE 

#if _DEBUG
#define DEBUG _DEBUG
#endif

//Extra methods for 32-bit support in 64-bit program assembly
#define EXTRA 1

#endif // __LIB_CONFIG