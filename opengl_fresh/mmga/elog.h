#ifndef ELOG_H_
#define ELOG_H_

// elog_ aka C victorinox.
// Sometimes i just want to quickly print some value.
// Basically save yourself some typing wrapper.
// fflush intentional.

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef ELOGDEF
#define ELOGDEF static inline
#endif /* ELOGDEF */
#ifndef ELOG_TARGET
#define ELOG_TARGET stderr
#endif /* ELOG_TARGET */
#ifndef ELOG_TAG
#define ELOG_TAG "[elog]: "
#endif /* ELOG_TAG */

ELOGDEF void elog_zu(size_t val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%zu\n", val);
    fflush(ELOG_TARGET);
}
ELOGDEF void elog_d(int val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%d\n", val);
    fflush(ELOG_TARGET);
}
ELOGDEF void elog_f(float val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%f\n", val);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_s(const char* str)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%s\n", str);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_u(unsigned int val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%u\n", val);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_llu(unsigned long long val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%llu\n", val);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_lld(long long val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%lld\n", val);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_p(const void* p)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%p\n", p);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_lu(unsigned long val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%lu\n", val);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_ld(long val)
{
    fprintf(ELOG_TARGET, ELOG_TAG "%ld\n", val);
    fflush(ELOG_TARGET);
}

ELOGDEF void elog_abort(const char* str)
{
    elog_s(str);
    abort();
}

#define elog_bytes_as_kib(bytes) do { elog_f((bytes)*0.0009765625); } while (0)
#define elog_bytes_as_mib(bytes) do { elog_f((bytes)*9.5367431640625e-7); } while (0)
#define elog_bytes_as_gib(bytes) do { elog_f((bytes)*9.3132257461548e-10); } while (0)

#endif /* ELOG_H_ */
