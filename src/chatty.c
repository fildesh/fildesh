/**
 * \file chatty.c
 * Async socket example.
 *
 * It seems like an aio_read() will read
 * all it can until an aio_suspend() is called.
 * The aio_suspend() will return when some amount
 * of data has been read.
 **/
#define LACE_POSIX_SOURCE

#include <aio.h>
#include "lace.h"
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
  int argi = init_sysCx (&argc, &argv);
  DeclLegit( good );
  int istat = 0;
  const char* host = "127.0.0.1";
  const char* service = "1337";

  struct addrinfo crit;
  struct addrinfo* list = 0;
  struct addrinfo* addr = 0;
  struct aiocb aio[1];

  Zeroize( crit );
  Zeroize( *aio );

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

  DoLegitP( istat == 0, "getaddrinfo()" )
    istat = getaddrinfo (host, service, &crit, &list);

  DoLegit( 0 )
  {
    addr = list;
    if (addr->ai_next)
    {
      DBog0( "Host could bind to multiple!" );
    }
    if (argi < argc && eql_cstr (argv[argi], "-connect"))
    {
      int sock = -1;
      LaceX* in = open_fd_LaceXF(0);

      OFile of[] = {DEFAULT_OFile};

      oput_cstr_OFile (of, "hi");

      /* Wait for parent proc to be ready.*/
      slurp_LaceX(in);
      close_LaceX(in);

      DoLegitP( sock >= 0, "socket()" )
        sock = socket (addr->ai_family,
                       addr->ai_socktype,
                       addr->ai_protocol);
      DoLegitP( istat >= 0, "connect()" )
        istat = connect (sock, addr->ai_addr, addr->ai_addrlen);

      DoLegitP( istat == 0, "aio_write()" )
      {
        aio->aio_fildes = sock;
        aio->aio_buf = of->buf.s;
        aio->aio_nbytes = of->off;
        istat = aio_write (aio);
      }
      DoLegitP( istat == 0, "aio_suspend()" )
      {
        do {
          const struct aiocb* tmp = aio;
          istat = aio_suspend (&tmp, 1, 0);
        } while (istat != 0 && errno == EINTR);
      }

      DoLegitLine( "aio_return()" )
        aio_return (aio) == (ssize_t) aio->aio_nbytes;

      if (sock >= 0)  close (sock);
      lose_OFile (of);
    }
    else
    {
      OSPc ospc[] = {DEFAULT_OSPc};
      int sock = -1;
      int sock1 = -1;
      struct sockaddr_storage client_addr;
      socklen_t client_addr_nbytes = sizeof (client_addr);
      DeclTable( byte, msg );
      ssize_t nbytes = 0;

      ospc->cmd = cons1_AlphaTab (exename_of_sysCx ());
      PushTable( ospc->args, cons1_AlphaTab ("-connect") );

      stdxpipe_OSPc (ospc);

      DoLegitLine( "spawn()" )
        spawn_OSPc (ospc);

      DoLegitP( sock >= 0, "socket()" )
      {
        EnsizeTable( msg, 1024 );

        sock = socket (addr->ai_family,
                       addr->ai_socktype,
                       addr->ai_protocol);
      }
      DoLegitP( istat == 0, "bind()" )
        istat = bind (sock, addr->ai_addr, addr->ai_addrlen);

      DoLegitP( istat == 0, "listen()" )
        istat = listen (sock, SOMAXCONN);

      DoLegitP( sock1 >= 0, "accept()" )
      {
        /* Tell spawned process that we are listening.*/
        close_OFile (ospc->of);

        sock1 = accept (sock,
                        (struct sockaddr*) &client_addr,
                        &client_addr_nbytes);
      }

      DoLegitP( istat == 0, "aio_read()" )
      {
        aio->aio_fildes = sock1;
        aio->aio_buf = msg.s;
        aio->aio_nbytes = msg.sz;

        istat = aio_read (aio);
      }

      DoLegitP( istat == 0, "aio_suspend()" )
      {
        do {
          const struct aiocb* tmp = aio;
          istat = aio_suspend (&tmp, 1, 0);
        } while (istat < 0 && errno == EINTR);
      }

      DoLegitP( nbytes > 0, "aio_return()" )
        nbytes = aio_return (aio);

      DoLegit( 0 )
      {
        msg.sz = nbytes;
        fputs ("got:", stdout);
        fwrite (msg.s, sizeof(char), msg.sz, stdout);
        fputc ('\n', stdout);
      }

      LoseTable( msg );
      lose_OSPc (ospc);

      if (sock1 >= 0)  close (sock1);
      if (sock >= 0)  close (sock);
    }
  }

  if (list)
    freeaddrinfo (list);

  Zeroize( *aio );
  lose_sysCx ();
  return good ? 0 : 1;
}

