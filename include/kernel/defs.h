#ifndef __DEFS_H__
#define __DEFS_H__

/*
 *  https://clang.llvm.org/docs/AttributeReference.html#always-inline-force-inline
 */
#define __always_inline		inline __attribute__((__always_inline__))
#define __noreturn		__attribute__((__noreturn__))
#define __aligned(x)		__attribute__((__aligned__(x)))

#endif

