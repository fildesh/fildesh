/**
 * \file chatty.c
 * Async socket example.
 *
 * It seems like an aio_read() will read
 * all it can until an aio_suspend() is called.
 * The aio_suspend() will return when some amount
 * of data has been read.
 **/
#define POSIX_SOURCE

#include <aio.h>
#include "cx/syscx.h"
#include "cx/def.h"
#include "cx/fileb.h"
#include "cx/ospc.h"

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>


int main (int argc, char** argv)
{
    int argi =
        (init_sysCx (&argc, &argv),
         1);
    bool good = true;
    int istat = 0;
    const char* host = "127.0.0.1";
    const char* service = "1337";

    struct addrinfo crit;
    struct addrinfo* list = 0;
    struct addrinfo* addr = 0;
    DecloStack( struct aiocb, aio );

    memset (&crit, 0, sizeof (crit));
    memset (aio, 0, sizeof (*aio));

        /* crit.ai_family = AF_INET6; */
    crit.ai_family   = AF_INET;
        /* crit.ai_flags    = AI_V4MAPPED | AI_ADDRCONFIG; */
    crit.ai_socktype = SOCK_STREAM;
    crit.ai_protocol = IPPROTO_TCP;
        /* crit.ai_family   = AF_UNSPEC; */
        /* crit.ai_flags    = AI_PASSIVE; */
        /* crit.ai_socktype = SOCK_DGRAM; */
        /* crit.ai_protocol = IPPROTO_UDP; */


    if (argi < argc && !eql_cstr (argv[argi], "-connect"))
        failout_sysCx ("I take no arguments from humans.");

    BInit();

    istat = getaddrinfo (host, service, &crit, &list);
    BCasc( istat == 0, good, "getaddrinfo()" );

    addr = list;
    if (addr->ai_next)
    {
        DBog0( "Host could bind to multiple!" );
    }

    if (argi < argc && eql_cstr (argv[argi], "-connect"))
    {
        int sock = -1;
        DecloStack1( OFileB, of, dflt_OFileB () );

        BInit();

        dump_cstr_OFileB (of, "hi");

        /* Wait for parent proc to be ready.*/
        load_XFileB (stdin_XFileB ());
        close_XFileB (stdin_XFileB ());

        sock = socket (addr->ai_family,
                       addr->ai_socktype,
                       addr->ai_protocol);
        BCasc( sock >= 0, good, "socket()" );

        istat = connect (sock, addr->ai_addr, addr->ai_addrlen);
        BCasc( istat >= 0, good, "connect()" );

        aio->aio_fildes = sock;
        aio->aio_buf = of->buf.s;
        aio->aio_nbytes = of->off;
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
        lose_OFileB (of);
    }
    else
    {
        DecloStack1( OSPc, ospc, dflt_OSPc () );
        int sock = -1;
        int sock1 = -1;
        struct sockaddr_storage client_addr;
        socklen_t client_addr_nbytes = sizeof (client_addr);
        DeclTable( byte, msg );

        BInit();

        ospc->cmd = cons1_AlphaTab (exename_of_sysCx ());
        PushTable( ospc->args, cons1_AlphaTab ("-connect") );

        stdxpipe_OSPc (ospc);
        good = spawn_OSPc (ospc);
        BCasc( istat == 0, good, "spawn()" );

        EnsizeTable( msg, 1024 );

        sock = socket (addr->ai_family,
                       addr->ai_socktype,
                       addr->ai_protocol);
        BCasc( sock >= 0, good, "socket()" );

        istat = bind (sock, addr->ai_addr, addr->ai_addrlen);
        BCasc( istat == 0, good, "bind()" );

        istat = listen (sock, SOMAXCONN);
        BCasc( istat == 0, good, "listen()" );

        /* Tell spawned process that we are listening.*/
        close_OFileB (ospc->of);

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
        lose_OSPc (ospc);

        if (sock1 >= 0)  close (sock1);
        if (sock >= 0)  close (sock);
    }
    BCasc( 1, good, 0 );

    BLose();

    if (list)
        freeaddrinfo (list);

    memset (aio, 0, sizeof (*aio));
    lose_sysCx ();
    return 0;
}

