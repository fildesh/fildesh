/**
 * \file chatty.c
 * Async socket example.
 *
 * It seems like an aio_read() will read
 * all it can until an aio_suspend() is called.
 * The aio_suspend() will return when some amount
 * of data has been read.
 **/
#define FILDESH_POSIX_SOURCE

#include <fildesh/fildesh.h>
#include "include/fildesh/fildesh_compat_errno.h"
#include "include/fildesh/fildesh_compat_fd.h"
#include "include/fildesh/fildesh_compat_sh.h"

#include <aio.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>


int main (int argc, char** argv)
{
  int argi = 1;
  int istat = 0;
  const char* host = "127.0.0.1";
  const char* service = "1337";
  const char* message_text_data = NULL;
  size_t message_text_size = 0;
  bool connecting = false;

  struct addrinfo crit;
  struct addrinfo* list = NULL;
  struct addrinfo* addr = NULL;
  struct aiocb aio[1];

  memset(&crit, 0, sizeof(crit));
  memset(aio, 0, sizeof(*aio));

  /* crit.ai_family = AF_INET6; */
  crit.ai_family   = AF_INET;
  /* crit.ai_flags    = AI_V4MAPPED | AI_ADDRCONFIG; */
  crit.ai_socktype = SOCK_STREAM;
  crit.ai_protocol = IPPROTO_TCP;
  /* crit.ai_family   = AF_UNSPEC; */
  /* crit.ai_flags    = AI_PASSIVE; */
  /* crit.ai_socktype = SOCK_DGRAM; */
  /* crit.ai_protocol = IPPROTO_UDP; */


  if (argi >= argc) {
    message_text_data = "hi";
    message_text_size = strlen(message_text_data);
  } else if (0 == strcmp(argv[argi], "-connect")) {
    connecting = true;
  } else {
    message_text_data = argv[argi];
    message_text_size = strlen(message_text_data);
  }

  istat = getaddrinfo(host, service, &crit, &list);
  if (istat != 0) {
    fildesh_log_error("getaddrinfo()");
    return 1;
  }

  addr = list;
  if (addr->ai_next) {
    fildesh_log_warning("Host could bind to multiple!");
  }
  if (connecting) {
    char buf[1024];
    int sock = -1;
    FildeshX* in = open_fd_FildeshX(0);

    /* Wait for parent proc to be ready.*/
    slurp_FildeshX(in);
    close_FildeshX(in);

    sock = socket(addr->ai_family,
                  addr->ai_socktype,
                  addr->ai_protocol);
    if (sock < 0) {fildesh_compat_errno_trace(); return 1;}
    istat = connect(sock, addr->ai_addr, addr->ai_addrlen);
    if (istat != 0) {fildesh_compat_errno_trace(); return 1;}

    memcpy(buf, message_text_data, message_text_size);
    aio->aio_fildes = sock;
    aio->aio_buf = buf;
    aio->aio_nbytes = message_text_size;
    istat = aio_write(aio);
    if (istat != 0) {fildesh_compat_errno_trace(); return 1;}

    do {
      const struct aiocb* tmp = aio;
      istat = aio_suspend (&tmp, 1, 0);
    } while (istat != 0 && errno == EINTR);
    if (istat != 0) {fildesh_compat_errno_trace(); return 1;}

    istat = (int)aio_return(aio);
    if (istat < 0) {fildesh_compat_errno_trace(); return 1;}
    if (istat != (int)aio->aio_nbytes) {fildesh_log_warning("nbytes differs");}

    close(sock);
  } else {
    char buf[1024];
    int listen_sock = -1;
    int sock = -1;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_nbytes = sizeof(client_addr);
    ssize_t nbytes = 0;
    Fildesh_fd source_fd = -1;
    Fildesh_fd fd_to_child = -1;
    FildeshCompat_pid pid;

    istat = fildesh_compat_fd_pipe(&fd_to_child, &source_fd);
    if (istat != 0) {fildesh_compat_errno_trace(); return 1;}

    pid = fildesh_compat_fd_spawnlp(
        source_fd, 1, 2, NULL, argv[0], "-connect", NULL);
    if (pid < 0) {fildesh_compat_errno_trace(); return 126;}

    listen_sock = socket(addr->ai_family,
                         addr->ai_socktype,
                         addr->ai_protocol);
    if (listen_sock < 0) {fildesh_compat_errno_trace(); return 1;}
    istat = bind (listen_sock, addr->ai_addr, addr->ai_addrlen);
    if (istat != 0) {fildesh_compat_errno_trace(); close(listen_sock); return 1;}

    istat = listen(listen_sock, SOMAXCONN);
    if (istat != 0) {fildesh_compat_errno_trace(); close(listen_sock); return 1;}

    fildesh_compat_fd_close(fd_to_child);

    sock = accept(listen_sock,
                   (struct sockaddr*) &client_addr,
                   &client_addr_nbytes);
    if (istat != 0) {fildesh_compat_errno_trace(); close(listen_sock); return 1;}

    aio->aio_fildes = sock;
    aio->aio_buf = buf;
    aio->aio_nbytes = sizeof(buf);

    istat = aio_read(aio);
    if (istat != 0) {fildesh_compat_errno_trace();}

    if (istat == 0) do {
      const struct aiocb* tmp = aio;
      istat = aio_suspend (&tmp, 1, 0);
    } while (istat < 0 && errno == EINTR);
    if (istat != 0) {fildesh_compat_errno_trace();}

    if (istat == 0) {
      nbytes = aio_return(aio);
      if (nbytes <= 0) {fildesh_compat_errno_trace();}
    }
    if (nbytes > 0) {
      fputs("got:", stdout);
      fwrite(buf, sizeof(char), nbytes, stdout);
      fputc('\n', stdout);
    }

    if (sock >= 0) {close(sock);}
    close(listen_sock);
    fildesh_compat_sh_wait(pid);
  }

  if (list) {
    freeaddrinfo(list);
  }

  memset(aio, 0, sizeof(*aio));
  return (istat == 0 ? 0 : 1);
}

