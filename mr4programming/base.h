#ifndef BASE_H_
#define BASE_H_

// CONTEXT CRACKING FOR COMPILERS
#if defined(_MSC_VER)
# define COMPILER_CL (1)
#elif defined(__clang__)
# define COMPILER_CLANG (1)
#elif defined(__GNUC__) 
# define COMPILER_GCC (1)
#elif defined(__TINYC__)
# define COMPILER_TCC (1)
#else
# error Unknown compiler
#endif

// OS CONTEXT CRACING
#if defined(__linux__)
# define OS_LINUX (1)
#elif defined(_WIN32)
# define OS_WINDOWS (1)
#else
# error Unknown operating system
#endif


// ZERO OUT COMPILERS
#if !defined(COMPILER_CL)
# define COMPILER_CL (0)
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG (0)
#endif
#if !defined(COMPILER_GCC) 
# define COMPILER_GCC (0)
#endif
#if !defined(COMPILER_TCC)
# define COMPILER_TCC (0)
#endif
// ZERO OUT OS'es
#if !defined(OS_LINUX)
# define OS_LINUX (0)
#endif
#if !defined(OS_WINDOWS)
# define OS_WINDOWS (0)
#endif


/* Helper macros */
#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define IntFromPtr(p) (unsigned long long)((char*)p - (char*)0)
#define PtrFromInt(n) (void*)((char*)0 + (n))

#define Member(T,m) (((T*)0)->m)
#define OffsetOfMember(T,m) IntFromPtr(&Member(T,m))

#endif /* BASE_H_ */
