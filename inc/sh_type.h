#ifndef _SH_TYPE_H_
#define _SH_TYPE_H_

typedef unsigned char      SH_U8;

typedef unsigned short     SH_U16;

typedef unsigned int       SH_U32;

#ifdef SH_SUPPORT_LONGLONG
typedef unsigned long long SH_U64;
#endif

typedef char               SH_S8;

typedef short              SH_S16;

typedef int                SH_S32;

#ifdef SH_SUPPORT_LONGLONG
typedef long long          SH_S64;
#endif

typedef unsigned char      SH_BOOL;

typedef float              FLOAT;

typedef double             DOUBLE;

#ifndef VOID
typedef void               VOID;
#endif

#ifndef CHAR
typedef char               CHAR;
#endif

#ifdef LINUX
typedef unsigned int       DWORD;
#endif

typedef void*              HANDLE;

#define TRUE                              1
#define FALSE                             0
//#define NULL                              (void*)0

#ifdef SH_SUPPORT_64BIT
#define SH_ADDR            SH_U64
#else
#define SH_ADDR            SH_U32
#endif

typedef enum
{
    E_NOERR,
    E_PARAM,
    E_NULL,
    E_NOMEM,
    E_USERORPASS,
    E_OPENFILE,
    E_CLOSEFILE,
    E_READFILE,
    E_WRITEFILE,
    E_BIND,
    E_LISTEN,
    E_CONNECT,
    E_ACCEPT,
    E_SEND,
    E_RECV,
    E_NOTSURPOT,
    E_OTHER
}EERRCODE;

typedef enum
{
    E_U8,
    E_S8,
    E_U16,
    E_S16,
    E_U32,
    E_S32,
    E_FLOAT,
    E_DOUBLE
}EDATATYPE;

#endif

