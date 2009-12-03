#define HAVE_RPC_RPC_H 1
#define HAVE_RPC_PMAP_CLNT_H 1

#ifdef HAVE_BIT64

#define SIZEOF_UNSIGNED_LONG 8
#define SIZEOF_LONG 8

#define SIZEOF_UNSIGNED_INT 4
#define SIZEOF_INT 4

#else

#define SIZEOF_UNSIGNED_LONG 4
#define SIZEOF_LONG 4

#define SIZEOF_UNSIGNED_INT 4
#define SIZEOF_INT 4

#endif

#define HAVE_DECL_GETHOSTNAME 1
