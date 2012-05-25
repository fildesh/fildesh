
    /* Included from def.h */

#define Concatify(a,b) a ## b
#define ConcatifyDef(a,b)  Concatify(a,b)

#define ArraySz( a )  (sizeof(a) / sizeof(*a))

#define CastUp( T, field, p ) \
    ((T*) ((size_t) p - offsetof( T, field )))

#define EltZ( a, idx, elsz ) \
    ((void*) ((size_t) a + (size_t) ((idx) * (elsz))))

#define Elt( a, idx )  (&(a)[idx])

#define IdxEltZ( a, e, elsz ) \
    ((size_t) ((size_t) (e) - (size_t) (a)) / (elsz))

#define IdxElt( a, e ) \
    IdxEltZ( a, e, sizeof(*a) )

#define CeilQuot( a, b ) \
    (((a) + (b) - 1) / (b))

#define BSfx( a, op, b, sfx )  (a)sfx op (b)sfx

#define UFor( i, bel )  for (i = 0; i < (bel); ++i)

#define BInit() {
#define BLose() }
#define BLoop( i, bel )  uint i; for (i = 0; i < (bel); ++i) BInit()

#define Claim( x )  assert(x)
#define Claim2( a ,op, b )  assert((a) op (b))

#define AccepTok( line, tok ) \
    ((0 == strncmp ((line), (tok), strlen(tok))) \
     ? ((line) = &(line)[strlen(tok)]) \
     : 0)

#define DecloStack( T, x )  T onstack_##x; T* const restrict x = &onstack_##x



    /** Implemented in sys-cx.c **/
void
dbglog_printf3 (const char* file,
                const char* func,
                uint line,
                const char* fmt,
                ...);

#define DBog0(s)  dbglog_printf3 (__FILE__,__FUNC__,__LINE__,s)
#define DBog1(s,a)  dbglog_printf3 (__FILE__,__FUNC__,__LINE__,s,a)
#define DBog2(s,a,b)  dbglog_printf3 (__FILE__,__FUNC__,__LINE__,s,a,b)
#define DBog3(s,a,b,c)  dbglog_printf3 (__FILE__,__FUNC__,__LINE__,s,a,b,c)

    /** Cascading if statement.**/
#define BCasc(cond, inv, msg) \
    if (!(inv) || !(cond)) \
    { \
        inv = false; \
        if (msg) \
        { \
            DBog2( "(%s => !(%s))", msg, #cond ); \
        } \
    } \
    BLose() if (inv) BInit()

