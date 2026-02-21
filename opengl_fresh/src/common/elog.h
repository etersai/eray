#ifndef ELOG_H_
#define ELOG_H_

// elog_ aka C victorinox.
// Sometimes i just want to quickly print some value.
// Basically save yourself some typing wrapper.
// fflush intentional.

#ifndef ELOGDEF
#define ELOGDEF static inline
#endif /* ELOGDEF */
#ifndef ELOG_TARGET
#define ELOG_TARGET stderr
#endif /* ELOG_TARGET */
#ifndef ELOG_TAG
#define ELOG_TAG "[elog]: "
#endif /* ELOG_TAG */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

ELOGDEF void elog_fmt(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(ELOG_TARGET, ELOG_TAG);
    vfprintf(ELOG_TARGET, fmt, args);
    fprintf(ELOG_TARGET, "\n");
    va_end(args);
    fflush(ELOG_TARGET);
}

#define elog_zu(val)  elog_fmt("%zu", (val)) 
#define elog_d(val)   elog_fmt("%d", (val))
#define elog_f(val)   elog_fmt("%f", (val))
#define elog_s(s)     elog_fmt("%s", (s))
#define elog_c(c)     elog_fmt("%c", (c))
#define elog_u(val)   elog_fmt("%u", (val))
#define elog_llu(val) elog_fmt("%llu", (val))
#define elog_lld(val) elog_fmt("%lld", (val))
#define elog_p(p)     elog_fmt("%p", (p))
#define elog_lu(val)  elog_fmt("%lu", (val))
#define elog_ld(val)  elog_fmt("%ld", (val))

#define elog_pfx_zu(prefix, zu)   elog_fmt("%s%zu", prefix, zu)
#define elog_pfx_d(prefix, d)     elog_fmt("%s%d", prefix, d)
#define elog_pfx_f(prefix, f)     elog_fmt("%s%f",  prefix, f)
#define elog_pfx_u(prefix, u)     elog_fmt("%s%u",  prefix, u)
#define elog_pfx_llu(prefix, llu) elog_fmt("%s%llu", prefix, llu)
#define elog_pfx_lld(prefix, lld) elog_fmt("%s%lld", prefix, lld)
#define elog_pfx_p(prefix, p)     elog_fmt("%s%p",  prefix, p)
#define elog_pfx_lu(prefix, lu)   elog_fmt("%s%lu",  prefix, lu)
#define elog_pfx_ld(prefix, ld)   elog_fmt("%s%ld",  prefix, ld)

#define elog_bytes_as_kib(bytes) do { elog_f((bytes)*0.0009765625); } while (0)
#define elog_bytes_as_mib(bytes) do { elog_f((bytes)*9.5367431640625e-7); } while (0)
#define elog_bytes_as_gib(bytes) do { elog_f((bytes)*9.3132257461548e-10); } while (0)

#define elog_perma_assert(x) do { if (!x) { elog_fmt("Perma assertion: %s, %s:%d", #x, __FILE__, __LINE__); abort(); }} while (0)

ELOGDEF void elog_abort(const char* str)
{
    elog_s(str);
    abort();
}

#endif /* ELOG_H_ */
