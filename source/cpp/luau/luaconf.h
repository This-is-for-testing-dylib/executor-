// Modified luaconf.h to avoid macro redefinition issues

#pragma once

#include <stddef.h>

// Basic configuration
#define lua_assert(x)((void)0)
#define luai_apicheck(L, e)((void)0)

// Lack of C++ exceptions for some compilers/warning level combinations
#if !defined(LUA_USE_LONGJMP) && !defined(LUA_USE_CXEXCEPT)
#define LUA_USE_LONGJMP 1
#endif

// Macro environment
#if defined(LUA_USE_CXEXCEPT)
#include <exception>

struct lua_cexception
{
int dummy;
};

#define LUAI_THROW(L) throw lua_cexception()
#define LUAI_TRY(L,c,a) try { a } catch(lua_cexception&) { c }
#elif defined(LUA_USE_LONGJMP)
#include <setjmp.h>

// Note: set used in conjunction with try/catch macros in ldo.c
#define LUAI_THROW(L) longjmp((L)->global->errorjmp, 1)
#define LUAI_TRY(L,c,a) if (setjmp((L)->global->errorjmp) == 0) { a } else { c }
#else
#error "choose exception model"
#endif

// Export control for library objects
// LUAI_FUNC is a workhorse - defines visibility, linkage for all module exports
// LUAI_DDEC is a rare variant for inline functions defined in headers that have to be exported
// LUAI_DDEF is the definition part of LUAI_DDEC
// LUA_API is used for Lua API functions/objects

// These will be overridden by build system definitions
#ifndef LUA_API
#define LUA_API extern
#endif

#ifndef LUAI_FUNC
// Avoid redefining LUAI_FUNC if already defined by build system
#define LUAI_FUNC extern
#endif

#ifndef LUAI_DDEC
#define LUAI_DDEC extern
#endif

#ifndef LUAI_DDEF
#define LUAI_DDEF
#endif

// Type sizes
#define LUAI_MAXSHORTLEN 40

// Minimum Lua stack available to a C function
#define LUA_MINSTACK20

// Maximum recursion depth when parsing expressions
#define LUAI_MAXPARSE500

// Maximum number of upvalues for a function prototype
#define LUA_MAXUPVALUES60

// Buffer size used for on-stack string operations
#define LUA_BUFFERSIZE 512

// Compatibility
#define LUA_COMPAT_DEBUGLIBNAME 1 // compatibility with old debug library name
