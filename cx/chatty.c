
    /* It seems like an async_read() will read
     * all it can until an async_suspend() is called.
     * The async_suspend will return when some amount
     * of data has been read.
     */
#define POSIX_C_SOURCE 1

#include "def.h"
#include "fileb.h"
#include <aio.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef _MSC_VER
# define __func__ __FUNCTION__
#endif


#define DBog0(s)  dump_debug(__FILE__,__func__,__LINE__,s)
#define DBog1(s,a)  dump_debug (__FILE__,__func__,__LINE__,s,a)
#define DBog2(s,a,b)  dump_debug (__FILE__,__func__,__LINE__,s,a,b)
#define DBog3(s,a,b,c)  dump_debug (__FILE__,__func__,__LINE__,s,a,b,c)


#if 0
#define CCif(cond, inv, msg) \
    if ((inv) && !(cond)) \
    { \
        inv = false; \
        if (msg) \
        { \
            DBog2( "(%s => !(%s))", msg, #cond ); \
        } \
    } \
    else if (inv)
#endif

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


FileB LogOut;

static
    void
dump_debug (const char* file,
            const char* func,
            uint line,
            const char* fmt,
            ...)
{
    va_list args;
    int err = errno;
    FileB* f = &LogOut;

    printf_FileB (f, "%s(%u) %s: ", file, line, func);

    va_start (args, fmt);
    vprintf_FileB (f, fmt, args);
    va_end(args);

    dump_char_FileB (f, '\n');

    if (err != 0)
    {
#if 0
            /* Why no work? */
        const uint n = 2048 * sizeof(char);
        char* s;

        printf_FileB (f, "^^^ errno:%d ", err);

        s = (char*) ensure_FileB (f, n);
        s[0] = '\0';

        strerror_r (err, s, n);

        f->off += strlen (s) * sizeof(char);
        dump_char_FileB (f, '\n');
#else
        printf_FileB (f, "^^^ errno:%d %s\n", err, strerror (err));
#endif
        errno = 0;
    }
    flusho_FileB (f);
}


int main ()
{
    bool good = true;
    pid_t pid;
    int istat = 0;
    int io[2];
    const char* host = "127.0.0.1";
    const char* service = "1337";

    struct addrinfo crit;
    struct addrinfo* list = 0;
    struct addrinfo* addr = 0;
    DecloStack( struct aiocb, aio );

    memset (&crit, 0, sizeof (crit));
    memset (aio, 0, sizeof (*aio));

    init_FileB (&LogOut);
    seto_FileB (&LogOut, true);
    set_FILE_FileB (&LogOut, stderr);


        /* crit.ai_family = AF_INET6; */
    crit.ai_family   = AF_INET;
        /* crit.ai_flags    = AI_V4MAPPED | AI_ADDRCONFIG; */
    crit.ai_socktype = SOCK_STREAM;
    crit.ai_protocol = IPPROTO_TCP;
        /* crit.ai_family   = AF_UNSPEC; */
        /* crit.ai_flags    = AI_PASSIVE; */
        /* crit.ai_socktype = SOCK_DGRAM; */
        /* crit.ai_protocol = IPPROTO_UDP; */

    BInit();

    istat = getaddrinfo (host, service, &crit, &list);
    BCasc( istat == 0, good, "getaddrinfo()" );

    addr = list;
    if (addr->ai_next)
    {
        DBog0( "Host could bind to multiple!" );
    }

    istat = pipe (io);
    BCasc( istat == 0, good, "pipe()" );

    pid = fork ();
    BCasc( pid >= 0, good, "fork()" );

    if (pid == 0)
    {
        int sock = -1;
        DecloStack( FileB, f );

        BInit();

        close (io[1]);

        init_FileB (f);

        dump_cstr_FileB (f, "hi");

            /* Wait for parent proc to be ready.*/
        {
            byte tmp;
            read (io[0], &tmp, 1);
        }
        close (io[0]);

        sock = socket (addr->ai_family,
                       addr->ai_socktype,
                       addr->ai_protocol);
        BCasc( sock >= 0, good, "socket()" );

        istat = connect (sock, addr->ai_addr, addr->ai_addrlen);
        BCasc( istat >= 0, good, "connect()" );

        aio->aio_fildes = sock;
        aio->aio_buf = f->buf.s;
        aio->aio_nbytes = f->off;
        istat = aio_write (aio);
        BCasc( istat == 0, good, "aio_write()" );

        do {
            const struct aiocb* tmp = aio;
            istat = aio_suspend (&tmp, 1, 0);
        } while (istat != 0 && errno == EINTR);

        BCasc( istat == 0, good, "aio_suspend()" );
        BCasc( aio_return (aio) == (ssize_t) aio->aio_nbytes, good, "aio_return()" );

        BLose();

        if (sock >= 0)  close (sock);
        lose_FileB (f);
    }
    else
    {
        int sock = -1;
        int sock1 = -1;
        struct sockaddr_storage client_addr;
        socklen_t client_addr_nbytes = sizeof (client_addr);

        DeclTable( byte, msg );

        BInit();

        close (io[0]);

        SizeUpTable( msg, 1024 );

        sock = socket (addr->ai_family,
                       addr->ai_socktype,
                       addr->ai_protocol);
        BCasc( sock >= 0, good, "socket()" );

        istat = bind (sock, addr->ai_addr, addr->ai_addrlen);
        BCasc( istat == 0, good, "bind()" );

        istat = listen (sock, SOMAXCONN);
        BCasc( istat == 0, good, "listen()" );

        close (io[1]);
        io[1] = -1;
        sock1 = accept (sock,
                        (struct sockaddr*) &client_addr,
                        &client_addr_nbytes);

        BCasc( sock1 >= 0, good, "accept()" );

        aio->aio_fildes = sock1;
        aio->aio_buf = msg.s;
        aio->aio_nbytes = msg.sz;

        aio_read (aio);

        BCasc( sock1 >= 0, good, "aio_read()" );
        do {
            const struct aiocb* tmp = aio;
            istat = aio_suspend (&tmp, 1, 0);
        } while (istat < 0 && errno == EINTR);

        BCasc( istat == 0, good, "aio_suspend()" );

        {
            ssize_t nbytes = aio_return (aio);
            BInit();
            BCasc( nbytes > 0, good, "aio_return()" );
            msg.sz = nbytes;
            BLose();
        }

        BCasc( 1, good, 0 );

        fputs ("got:", stdout);
        fwrite (msg.s, sizeof(char), msg.sz, stdout);
        fputc ('\n', stdout);

        BLose();

        LoseTable( msg );

        if (io[1] >= 0)  close (io[1]);
        waitpid (pid, &istat, 0);

        if (sock1 >= 0)  close (sock1);
        if (sock >= 0)  close (sock);
    }

    BLose();

    if (list)
        freeaddrinfo (list);

    lose_FileB (&LogOut);
    memset (aio, 0, sizeof (*aio));
    return 0;
}

