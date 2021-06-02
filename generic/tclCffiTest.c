/*
 * a set of functions to test arguments and returns in dyncall.
 */

#include <tcl.h>
#include <stddef.h>
#include <stdint.h>		/* uintptr_t */
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef __WIN32__
#include <windows.h>
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#define CFFI_STDCALL __stdcall
#else
#define CFFI_STDCALL
#endif

#define FNSTR2NUM(name_, fmt_, type_)           \
    EXTERN type_ name_ (char *s) {              \
        type_ val;                              \
        sscanf(s, "%" #fmt_, &val);             \
        return val;                             \
    }

#define FNINOUT(token_, type_)                                   \
    EXTERN type_ token_##_out(type_ in, type_ *out)              \
    {                                                            \
        *out = in + 1;                                           \
        return in + 2;                                           \
    }                                                            \
    EXTERN type_ token_##_byref(type_ *inP) { return *inP + 2; } \
    EXTERN type_ token_##_inout(type_ *inoutP)                   \
    {                                                            \
        *inoutP = *inoutP + 1;                                   \
        return *inoutP + 1;                                      \
    }

#define FNNUMERICARRAY(token_, type_)                       \
    EXTERN type_ token_##_array_in(int n, const type_ *arr) \
    {                                                       \
        type_ sum;                                          \
        int i;                                              \
        for (i = 0, sum = 0; i < n; ++i)                    \
            sum += arr[i];                                  \
        return sum;                                         \
    }                                                       \
    EXTERN void token_##_array_out(int n, type_ *arr)       \
    {                                                       \
        int i;                                              \
        for (i = 0; i < n; ++i)                             \
            arr[i] = (type_)i;                              \
    }                                                       \
    EXTERN void token_##_array_inout(int n, type_ *arr)     \
    {                                                       \
        int i;                                              \
        for (i = 0; i < n; ++i)                             \
            arr[i] = arr[i] + 1;                            \
    }

#define FNSTRINGS(token_, type_)                     \
    EXTERN int token_##_out(type_ *in, type_ *out)   \
    {                                                \
        int i;                                       \
        for (i = 0; in[i]; ++i)                      \
            out[i] = in[i];                          \
        out[i] = in[i];                              \
        return i;                                    \
    }                                                \
    EXTERN int token_##_reverse_inout(type_ *inout)  \
    {                                                \
        int i, len;                                  \
        type_ ch;                                    \
        for (len = 0; inout[len]; ++len)             \
            ;                                        \
        for (i = 0; i < len / 2; ++i) {              \
            ch                 = inout[i];           \
            inout[i]           = inout[len - i - 1]; \
            inout[len - i - 1] = ch;                 \
        }                                            \
        return len;                                  \
    }


typedef struct InnerTestStruct {
    char c[15];
} InnerTestStruct;

typedef struct TestStruct {
    signed char c;
    int i;
    short shrt;
    unsigned int uint;
    unsigned short ushrt;
    long l;
    unsigned char uc;
    unsigned long ul;
    char c3[13];
    long long ll;
    Tcl_UniChar unic[13];
    unsigned long long ull;
    unsigned char b[3];
    float f;
    InnerTestStruct s;
    double d;
} TestStruct;

EXTERN int noargs () { return 42; }
EXTERN int onearg (int arga) { return -arga; }
EXTERN int twoargs (int arga, int argb) { return arga + argb; }

DLLEXPORT double CFFI_STDCALL stdcalltest(double arga, double argb) {
  /* Division so order of argument errors will be caught */
    return arga / argb;
}

EXTERN void schar_to_void(signed char a) { return ; }
EXTERN void uchar_to_void(unsigned char a) { return ; }
EXTERN void short_to_void(signed short a) { return ; }
EXTERN void ushort_to_void(unsigned short a) { return ; }
EXTERN void int_to_void(signed int a) { return ; }
EXTERN void uint_to_void(unsigned int a) { return ; }
EXTERN void long_to_void(signed long a) { return ; }
EXTERN void ulong_to_void(unsigned long a) { return ; }
EXTERN void longlong_to_void(signed long long a) { return ; }
EXTERN void ulonglong_to_void(unsigned long long a) { return ; }
EXTERN void float_to_void(float a) { return ; }
EXTERN void double_to_void(double a) { return ; }
EXTERN void pointer_to_void(void *a) { return ; }
EXTERN void string_to_void(char *s) { return; }
EXTERN void unistring_to_void(Tcl_UniChar *s) { return; }
EXTERN void chars_to_void(char s[]) { return; }
EXTERN void unichars_to_void(Tcl_UniChar s[]) { return; }
EXTERN void binary_to_void(unsigned char *b) { return; }
EXTERN void bytes_to_void(unsigned char *b[]) { return; }


EXTERN signed char schar_to_schar(signed char a) { return a; }
EXTERN signed char uchar_to_schar(unsigned char a) { return (signed char) a; }
EXTERN signed char short_to_schar(signed short a) { return (signed char) a; }
EXTERN signed char ushort_to_schar(unsigned short a) { return (signed char) a; }
EXTERN signed char int_to_schar(signed int a) { return (signed char) a; }
EXTERN signed char uint_to_schar(unsigned int a) { return (signed char) a; }
EXTERN signed char long_to_schar(signed long a) { return (signed char) a; }
EXTERN signed char ulong_to_schar(unsigned long a) { return (signed char) a; }
EXTERN signed char longlong_to_schar(signed long long a) { return (signed char) a; }
EXTERN signed char ulonglong_to_schar(unsigned long long a) { return (signed char) a; }
EXTERN signed char float_to_schar(float a) { return (signed char) a; }
EXTERN signed char double_to_schar(double a) { return (signed char) a; }
EXTERN signed char pointer_to_schar(void *a) { return (signed char)(uintptr_t)a; }
FNSTR2NUM(string_to_schar, hhd, signed char)
FNINOUT(schar, signed char)
FNNUMERICARRAY(schar, signed char)

EXTERN unsigned char schar_to_uchar(signed char a) { return (unsigned char) a; }
EXTERN unsigned char uchar_to_uchar(unsigned char a) { return a; }
EXTERN unsigned char short_to_uchar(signed short a) { return (unsigned char) a; }
EXTERN unsigned char ushort_to_uchar(unsigned short a) { return (unsigned char) a; }
EXTERN unsigned char int_to_uchar(signed int a) { return (unsigned char) a; }
EXTERN unsigned char uint_to_uchar(unsigned int a) { return (unsigned char) a; }
EXTERN unsigned char long_to_uchar(signed long a) { return (unsigned char) a; }
EXTERN unsigned char ulong_to_uchar(unsigned long a) { return (unsigned char) a; }
EXTERN unsigned char longlong_to_uchar(signed long long a) { return (unsigned char) a; }
EXTERN unsigned char ulonglong_to_uchar(unsigned long long a) { return (unsigned char) a; }
EXTERN unsigned char float_to_uchar(float a) { return (unsigned char) a; }
EXTERN unsigned char double_to_uchar(double a) { return (unsigned char) a; }
EXTERN unsigned char pointer_to_uchar(void *a) { return (unsigned char)(uintptr_t)a; }
FNSTR2NUM(string_to_uchar, hhu, unsigned char)
FNINOUT(uchar, unsigned char)
FNNUMERICARRAY(uchar, unsigned char)

EXTERN signed short schar_to_short(signed char a) { return (signed short) a; }
EXTERN signed short uchar_to_short(unsigned char a) { return (signed short) a; }
EXTERN signed short short_to_short(signed short a) { return a; }
EXTERN signed short ushort_to_short(unsigned short a) { return (signed short) a; }
EXTERN signed short int_to_short(signed int a) { return (signed short) a; }
EXTERN signed short uint_to_short(unsigned int a) { return (signed short) a; }
EXTERN signed short long_to_short(signed long a) { return (signed short) a; }
EXTERN signed short ulong_to_short(unsigned long a) { return (signed short) a; }
EXTERN signed short longlong_to_short(signed long long a) { return (signed short) a; }
EXTERN signed short ulonglong_to_short(unsigned long long a) { return (signed short) a; }
EXTERN signed short float_to_short(float a) { return (signed short) a; }
EXTERN signed short double_to_short(double a) { return (signed short) a; }
EXTERN signed short pointer_to_short(void *a) { return (signed short)(uintptr_t)a; }
FNSTR2NUM(string_to_short, hd, short)
FNINOUT(short, short)
FNNUMERICARRAY(short, short)

EXTERN unsigned short schar_to_ushort(signed char a) { return (unsigned short) a; }
EXTERN unsigned short uchar_to_ushort(unsigned char a) { return (unsigned short) a; }
EXTERN unsigned short short_to_ushort(signed short a) { return (unsigned short) a; }
EXTERN unsigned short ushort_to_ushort(unsigned short a) { return a; }
EXTERN unsigned short int_to_ushort(signed int a) { return (unsigned short) a; }
EXTERN unsigned short uint_to_ushort(unsigned int a) { return (unsigned short) a; }
EXTERN unsigned short long_to_ushort(signed long a) { return (unsigned short) a; }
EXTERN unsigned short ulong_to_ushort(unsigned long a) { return (unsigned short) a; }
EXTERN unsigned short longlong_to_ushort(signed long long a) { return (unsigned short) a; }
EXTERN unsigned short ulonglong_to_ushort(unsigned long long a) { return (unsigned short) a; }
EXTERN unsigned short float_to_ushort(float a) { return (unsigned short) a; }
EXTERN unsigned short double_to_ushort(double a) { return (unsigned short) a; }
EXTERN unsigned short pointer_to_ushort(void *a) { return (unsigned short)(uintptr_t)a; }
FNSTR2NUM(string_to_ushort, hu, unsigned short)
FNINOUT(ushort, unsigned short)
FNNUMERICARRAY(ushort, unsigned short)

EXTERN signed int schar_to_int(signed char a) { return (signed int) a; }
EXTERN signed int uchar_to_int(unsigned char a) { return (signed int) a; }
EXTERN signed int short_to_int(signed short a) { return (signed int) a; }
EXTERN signed int ushort_to_int(unsigned short a) { return (signed int) a; }
EXTERN signed int int_to_int(signed int a) { return a; }
EXTERN signed int uint_to_int(unsigned int a) { return (signed int) a; }
EXTERN signed int long_to_int(signed long a) { return (signed int) a; }
EXTERN signed int ulong_to_int(unsigned long a) { return (signed int) a; }
EXTERN signed int longlong_to_int(signed long long a) { return (signed int) a; }
EXTERN signed int ulonglong_to_int(unsigned long long a) { return (signed int) a; }
EXTERN signed int float_to_int(float a) { return (signed int) a; }
EXTERN signed int double_to_int(double a) { return (signed int) a; }
EXTERN signed int pointer_to_int(void *a) { return (signed int)(uintptr_t)a; }
FNSTR2NUM(string_to_int, d, int)
FNINOUT(int, int)
FNNUMERICARRAY(int, int)

EXTERN unsigned int schar_to_uint(signed char a) { return (unsigned int) a; }
EXTERN unsigned int uchar_to_uint(unsigned char a) { return (unsigned int) a; }
EXTERN unsigned int short_to_uint(signed short a) { return (unsigned int) a; }
EXTERN unsigned int ushort_to_uint(unsigned short a) { return (unsigned int) a; }
EXTERN unsigned int int_to_uint(signed int a) { return (unsigned int) a; }
EXTERN unsigned int uint_to_uint(unsigned int a) { return a; }
EXTERN unsigned int long_to_uint(signed long a) { return (unsigned int) a; }
EXTERN unsigned int ulong_to_uint(unsigned long a) { return (unsigned int) a; }
EXTERN unsigned int longlong_to_uint(signed long long a) { return (unsigned int) a; }
EXTERN unsigned int ulonglong_to_uint(unsigned long long a) { return (unsigned int) a; }
EXTERN unsigned int float_to_uint(float a) { return (unsigned int) a; }
EXTERN unsigned int double_to_uint(double a) { return (unsigned int) a; }
EXTERN unsigned int pointer_to_uint(void *a) { return (unsigned int)(uintptr_t)a; }
FNSTR2NUM(string_to_uint, u, unsigned int)
FNINOUT(uint, unsigned int)
FNNUMERICARRAY(uint, unsigned int)

EXTERN signed long schar_to_long(signed char a) { return (signed long) a; }
EXTERN signed long uchar_to_long(unsigned char a) { return (signed long) a; }
EXTERN signed long short_to_long(signed short a) { return (signed long) a; }
EXTERN signed long ushort_to_long(unsigned short a) { return (signed long) a; }
EXTERN signed long int_to_long(signed int a) { return (signed long) a; }
EXTERN signed long uint_to_long(unsigned int a) { return (signed long) a; }
EXTERN signed long long_to_long(signed long a) { return a; }
EXTERN signed long ulong_to_long(unsigned long a) { return (signed long) a; }
EXTERN signed long longlong_to_long(signed long long a) { return (signed long) a; }
EXTERN signed long ulonglong_to_long(unsigned long long a) { return (signed long) a; }
EXTERN signed long float_to_long(float a) { return (signed long) a; }
EXTERN signed long double_to_long(double a) { return (signed long) a; }
EXTERN signed long pointer_to_long(void *a) { return (signed long)(uintptr_t)a; }
FNSTR2NUM(string_to_long, ld, long)
FNINOUT(long, long)
FNNUMERICARRAY(long, long)

EXTERN unsigned long schar_to_ulong(signed char a) { return (unsigned long) a; }
EXTERN unsigned long uchar_to_ulong(unsigned char a) { return (unsigned long) a; }
EXTERN unsigned long short_to_ulong(signed short a) { return (unsigned long) a; }
EXTERN unsigned long ushort_to_ulong(unsigned short a) { return (unsigned long) a; }
EXTERN unsigned long int_to_ulong(signed int a) { return (unsigned long) a; }
EXTERN unsigned long uint_to_ulong(unsigned int a) { return (unsigned long) a; }
EXTERN unsigned long long_to_ulong(signed long a) { return (unsigned long) a; }
EXTERN unsigned long ulong_to_ulong(unsigned long a) { return a; }
EXTERN unsigned long longlong_to_ulong(signed long long a) { return (unsigned long) a; }
EXTERN unsigned long ulonglong_to_ulong(unsigned long long a) { return (unsigned long) a; }
EXTERN unsigned long float_to_ulong(float a) { return (unsigned long) a; }
EXTERN unsigned long double_to_ulong(double a) { return (unsigned long) a; }
EXTERN unsigned long pointer_to_ulong(void *a) { return (unsigned long)(uintptr_t)a; }
FNSTR2NUM(string_to_ulong, lu, unsigned long)
FNINOUT(ulong, unsigned long)
FNNUMERICARRAY(ulong, unsigned long)

EXTERN signed long long schar_to_longlong(signed char a) { return (signed long long) a; }
EXTERN signed long long uchar_to_longlong(unsigned char a) { return (signed long long) a; }
EXTERN signed long long short_to_longlong(signed short a) { return (signed long long) a; }
EXTERN signed long long ushort_to_longlong(unsigned short a) { return (signed long long) a; }
EXTERN signed long long int_to_longlong(signed int a) { return (signed long long) a; }
EXTERN signed long long uint_to_longlong(unsigned int a) { return (signed long long) a; }
EXTERN signed long long long_to_longlong(signed long a) { return (signed long long) a; }
EXTERN signed long long ulong_to_longlong(unsigned long a) { return (signed long long) a; }
EXTERN signed long long longlong_to_longlong(signed long long a) { return a; }
EXTERN signed long long ulonglong_to_longlong(unsigned long long a) { return (signed long long) a; }
EXTERN signed long long float_to_longlong(float a) { return (signed long long) a; }
EXTERN signed long long double_to_longlong(double a) { return (signed long long) a; }
EXTERN signed long long pointer_to_longlong(void *a) { return (signed long long)(uintptr_t)a; }
FNSTR2NUM(string_to_longlong, lld, long long)
FNINOUT(longlong, long long)
FNNUMERICARRAY(longlong, long long)

EXTERN unsigned long long schar_to_ulonglong(signed char a) { return (unsigned long long) a; }
EXTERN unsigned long long uchar_to_ulonglong(unsigned char a) { return (unsigned long long) a; }
EXTERN unsigned long long short_to_ulonglong(signed short a) { return (unsigned long long) a; }
EXTERN unsigned long long ushort_to_ulonglong(unsigned short a) { return (unsigned long long) a; }
EXTERN unsigned long long int_to_ulonglong(signed int a) { return (unsigned long long) a; }
EXTERN unsigned long long uint_to_ulonglong(unsigned int a) { return (unsigned long long) a; }
EXTERN unsigned long long long_to_ulonglong(signed long a) { return (unsigned long long) a; }
EXTERN unsigned long long ulong_to_ulonglong(unsigned long a) { return (unsigned long long) a; }
EXTERN unsigned long long longlong_to_ulonglong(signed long long a) { return (unsigned long long) a; }
EXTERN unsigned long long ulonglong_to_ulonglong(unsigned long long a) { return a; }
EXTERN unsigned long long float_to_ulonglong(float a) { return (unsigned long long) a; }
EXTERN unsigned long long double_to_ulonglong(double a) { return (unsigned long long) a; }
EXTERN unsigned long long pointer_to_ulonglong(void *a) { return (signed long long)(uintptr_t)a; }
FNSTR2NUM(string_to_ulonglong, llu, unsigned long long)
FNINOUT(ulonglong, unsigned long long)
FNNUMERICARRAY(ulonglong, unsigned long long)

EXTERN float schar_to_float(signed char a) { return (float) a; }
EXTERN float uchar_to_float(unsigned char a) { return (float) a; }
EXTERN float short_to_float(signed short a) { return (float) a; }
EXTERN float ushort_to_float(unsigned short a) { return (float) a; }
EXTERN float int_to_float(signed int a) { return (float) a; }
EXTERN float uint_to_float(unsigned int a) { return (float) a; }
EXTERN float long_to_float(signed long a) { return (float) a; }
EXTERN float ulong_to_float(unsigned long a) { return (float) a; }
EXTERN float longlong_to_float(signed long long a) { return (float) a; }
EXTERN float ulonglong_to_float(unsigned long long a) { return (float) a; }
EXTERN float float_to_float(float a) { return a; }
EXTERN float double_to_float(double a) { return (float) a; }
EXTERN float pointer_to_float(void *a) { return (float)(uintptr_t)a; }
FNSTR2NUM(string_to_float, f, float)
FNINOUT(float, float)
FNNUMERICARRAY(float, float)

EXTERN double schar_to_double(signed char a) { return (double) a; }
EXTERN double uchar_to_double(unsigned char a) { return (double) a; }
EXTERN double short_to_double(signed short a) { return (double) a; }
EXTERN double ushort_to_double(unsigned short a) { return (double) a; }
EXTERN double int_to_double(signed int a) { return (double) a; }
EXTERN double uint_to_double(unsigned int a) { return (double) a; }
EXTERN double long_to_double(signed long a) { return (double) a; }
EXTERN double ulong_to_double(unsigned long a) { return (double) a; }
EXTERN double longlong_to_double(signed long long a) { return (double) a; }
EXTERN double ulonglong_to_double(unsigned long long a) { return (double) a; }
EXTERN double float_to_double(float a) { return (double) a; }
EXTERN double double_to_double(double a) { return a; }
EXTERN double pointer_to_double(void *a) { return (double)(uintptr_t)a; }
FNSTR2NUM(string_to_double, lf, double)
FNINOUT(double, double)
FNNUMERICARRAY(double, double)

EXTERN int pointer_in(int *pint) { return *pint; }
EXTERN void pointer_out(int **ppint)
{
    int *pint = ckalloc(sizeof(int));
    *pint = 99;
    *ppint = pint;
}
EXTERN void pointer_incr(char **pp) { *pp = *pp + 1; }
EXTERN void *pointer_byref(void **pp) { return *pp; }
EXTERN void pointer_noop(void *p) {}
EXTERN void *pointer_to_pointer(void *p) { return p; }
EXTERN void *pointer_add(void *p, int n) {
    return n + (char *)p;
}
EXTERN void *pointer_errno(void *p) {
    errno = EINVAL;
    return p;
}
#ifdef _WIN32
EXTERN void *pointer_lasterror(void *p)
{
    SetLastError(ERROR_INVALID_DATA);
    return p;
}
#endif

FNSTRINGS(string, char)
FNSTRINGS(unistring, Tcl_UniChar)
FNSTRINGS(binary, unsigned char)

EXTERN int jis0208_out(int bufsize, char *out)
{
    /* "8c" is the U+543e in jis0208 */
    int i, len;
    for (i = 0, len = 0; i < bufsize - 3; i += 2) {
        out[i]     = '8';
        out[i + 1] = 'c';
        ++len;
    }
    out[i] = '\0';
    out[i+1] = '\0';
    return len;
}

EXTERN int jis0208_in(char *in)
{
    /* "8c" is the U+543e in jis0208 */
    int len = 0;
    while (*in) {
        if (*in++ != '8')
            break;
        if (*in++ != 'c')
            break;
        ++len;
    }
    return len;
}

/* Caller responsible to ensure adequate memory! */
EXTERN int jis0208_inout(char *in)
{
    int len = (int) strlen(in);
    if (in[len+1] != 0)
        Tcl_Panic("jis0208_inout assumption false.");
    memmove(in + len, in, len + 2); /* Two null bytes */
    return len;
}

EXTERN int bytes_out(int n, unsigned char *in, unsigned char *out)
{
    int i;
    for (i = 0; i < n; ++i)
        out[i] = in[i];
    return i;
}

EXTERN void bytes_inout(int n, unsigned char *inout)
{
    int i;
    unsigned char ch;
    for (i = 0; i < n / 2; ++i) {
        ch       = inout[i];
        inout[i] = inout[n - i - 1];
        inout[n - i - 1] = ch;
    }
}

EXTERN void get_array_int(int *a, int n) {
    int i;
    for (i = 0; i < n; ++i)
        a[i] = 2*a[i];
}

EXTERN int getTestStructSize() { return (int)sizeof(TestStruct); }
EXTERN int getTestStruct(TestStruct *tsP)
{
    int off;
    Tcl_Obj *objP;
    Tcl_UniChar *unichars;
    int len;

#define OFF(type, field) (type) offsetof(TestStruct, field)
    tsP->c = OFF(signed char, c);
    tsP->i = OFF(int, i);
    tsP->shrt = OFF(short,shrt);
    tsP->uint = OFF(unsigned int,uint);
    tsP->ushrt = OFF(unsigned short,ushrt);
    tsP->l = OFF(long,l);
    tsP->uc = OFF(unsigned char,uc);
    tsP->ul = OFF(unsigned long,ul);
    snprintf(tsP->c3, sizeof(tsP->c3), "%d", (int) offsetof(TestStruct, c3));
    tsP->ll = OFF(long long, ll);
    objP = Tcl_ObjPrintf("%d", (int) offsetof(TestStruct, unic));
    unichars = Tcl_GetUnicodeFromObj(objP, &len);
    memmove(tsP->unic, unichars, (len+1)*sizeof(Tcl_UniChar));
    Tcl_DecrRefCount(objP);
    tsP->ull = OFF(unsigned long long, ull);
    tsP->b[0] = OFF(unsigned char, b);
    tsP->b[1] = tsP->b[0]+1;
    tsP->b[2] = tsP->b[0]+2;
    off          = OFF(int, f);
    tsP->f       = (float)off; /* Two step because direct assign causes MSVC 15.9.20 to crash */
    snprintf(tsP->s.c, sizeof(tsP->s.c), "%d", (int) offsetof(TestStruct, s));
    off          = OFF(int, d);
    tsP->d       = off; /* Two step because direct assign causes MSVC 15.9.20 to crash */
    return sizeof(*tsP);
}

/* Increments every field value by 1 */
EXTERN void incrTestStruct(TestStruct *tsP)
{
    int i;
    Tcl_Obj *objP;
    Tcl_UniChar *unichars;
    int len;

#define SET(fld)                 \
    do {                         \
        tsP->fld = tsP->fld + 1; \
    } while (0)

    SET(c);
    SET(i);
    SET(shrt);
    SET(uint);
    SET(ushrt);
    SET(l);
    SET(uc);
    SET(ul);

    sscanf(tsP->c3, "%d", &i);
    snprintf(tsP->c3, sizeof(tsP->c3), "%d", i + 1);

    SET(ll);

    /* unic */
    objP = Tcl_NewUnicodeObj(tsP->unic, -1);
    Tcl_GetIntFromObj(NULL, objP, &i);
    Tcl_DecrRefCount(objP);
    objP     = Tcl_NewIntObj(i);
    unichars = Tcl_GetUnicodeFromObj(objP, &len);
    memmove(tsP->unic, unichars, (len+1)*sizeof(Tcl_UniChar));
    Tcl_DecrRefCount(objP);

    SET(ull);

    for (i = 0; i < sizeof(tsP->b); ++i)
        tsP->b[i] = tsP->b[i] + 1;

    SET(f);

    sscanf(tsP->s.c, "%d", &i);
    snprintf(tsP->s.c, sizeof(tsP->s.c), "%d", i + 1);

    SET(d);
}

struct SimpleStruct {
    unsigned char c;
    long long ll;
    short s;
};
EXTERN int structFillArray (int n, struct SimpleStruct *out)
{
    int i;
    int val = 0;
    for (i = 0; i < n; ++i) {
        out[i].c  = ++val;
        out[i].ll = ++val;
        out[i].s  = ++val;
    }
    return val;
}
EXTERN int structCheck(struct SimpleStruct *in, signed char c, long long ll, short s)
{
    return in->c == c && in->ll == ll && in->s == s;
}

EXTERN void getEinvalString(char *bufP)
{
    strcpy(bufP, strerror(EINVAL));
}

EXTERN int setErrno(int i)
{
    errno = EINVAL;
    return i;
}

#ifdef _WIN32
EXTERN int setWinError(int i)
{
    SetLastError(ERROR_INVALID_DATA);
    return i;
}
#endif
