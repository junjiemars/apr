/* ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#ifndef APR_NETWORK_IO_H
#define APR_NETWORK_IO_H

#include "apr_general.h"
#include "apr_file_io.h"
#include "apr_errno.h"
#if APR_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MAX_SECS_TO_LINGER
#define MAX_SECS_TO_LINGER 30
#endif

#ifndef APRMAXHOSTLEN
#define APRMAXHOSTLEN 256
#endif

#ifndef APR_ANYADDR
#define APR_ANYADDR "0.0.0.0"
#endif

/* Socket option definitions */
#define APR_SO_LINGER        1
#define APR_SO_KEEPALIVE     2
#define APR_SO_DEBUG         4
#define APR_SO_NONBLOCK      8
#define APR_SO_REUSEADDR     16
#define APR_SO_TIMEOUT       32
#define APR_SO_SNDBUF        64
#define APR_SO_RCVBUF        128

#define APR_POLLIN    0x001 
#define APR_POLLPRI   0x002
#define APR_POLLOUT   0x004
#define APR_POLLERR   0x010
#define APR_POLLHUP   0x020
#define APR_POLLNVAL  0x040

typedef enum {APR_SHUTDOWN_READ, APR_SHUTDOWN_WRITE, 
	      APR_SHUTDOWN_READWRITE} ap_shutdown_how_e;

/* We need to make sure we always have an in_addr type, so APR will just
 * define it ourselves, if the platform doesn't provide it.
 */
#if !defined(APR_HAVE_IN_ADDR)
struct in_addr {
    ap_uint32_t  s_addr;
}
#endif

/* I guess not everybody uses inet_addr.  This defines ap_inet_addr
 * appropriately.
 */

#if APR_HAVE_INET_ADDR
#define ap_inet_addr    inet_addr
#elif APR_HAVE_INET_NETWORK        /* only DGUX, as far as I know */
#define ap_inet_addr    inet_network
#endif

typedef struct ap_socket_t     ap_socket_t;
typedef struct ap_pollfd_t     ap_pollfd_t;
typedef struct ap_hdtr_t       ap_hdtr_t;
typedef struct in_addr      ap_in_addr;

#if APR_HAS_SENDFILE
/* Define flags passed in on ap_sendfile() */
#define APR_SENDFILE_DISCONNECT_SOCKET      1

/* A structure to encapsulate headers and trailers for ap_sendfile */
struct ap_hdtr_t {
    struct iovec* headers;
    int numheaders;
    struct iovec* trailers;
    int numtrailers;
};
#endif

/* function definitions */

/*

=head1 ap_status_t ap_create_tcp_socket(ap_socket_t **new, ap_pool_t *cont)

B<Create a socket for tcp communication.>

    arg 1) The new socket that has been setup. 
    arg 2) The pool to use

=cut
 */
ap_status_t ap_create_tcp_socket(ap_socket_t **new, ap_pool_t *cont);

/*

=head1 ap_status_t ap_shutdown(ap_socket_t *thesocket, ap_shutdown_how_e how)

B<Shutdown either reading, writing, or both sides of a tcp socket.>

    arg 1) The socket to close 
    arg 2) How to shutdown the socket.  One of:
              APR_SHUTDOWN_READ      -- no longer allow read requests
              APR_SHUTDOWN_WRITE     -- no longer allow write requests
              APR_SHUTDOWN_READWRITE -- no longer allow read or write requests 

B<NOTE>:  This does not actually close the socket descriptor, it just
          controls which calls are still valid on the socket.

=cut
 */
ap_status_t ap_shutdown(ap_socket_t *ithesocket, ap_shutdown_how_e how);

/*

=head1 ap_status_t ap_close_socket(ap_socket_t *thesocket)

B<Close a tcp socket.>

    arg 1) The socket to close 

=cut
 */
ap_status_t ap_close_socket(ap_socket_t *thesocket);

/*

=head1 ap_status_t ap_bind(ap_socket_t *sock)

B<Bind the socket to it's assocaited port>

    arg 1) The socket to bind 

B<NOTE>:  This is where we will find out if there is any other process
          using the selected port.

=cut
 */
ap_status_t ap_bind(ap_socket_t *sock);

/*

=head1 ap_status_t ap_listen(ap_socket_t *sock, ap_int32_t backlog)

B<Listen to a bound socketi for connections.> 

    arg 1) The socket to listen on 
    arg 2) The number of outstanding connections allowed in the sockets
          listen queue.  If this value is less than zero, the listen
          queue size is set to zero.  

=cut
 */
ap_status_t ap_listen(ap_socket_t *sock, ap_int32_t backlog);

/*

=head1 ap_status_t ap_accept(ap_socket_t **new, ap_socket_t *sock, ap_pool_t *connection_pool)

B<Accept a new connection request>

    arg 1) A copy of the socket that is connected to the socket that
           made the connection request.  This is the socket which should
           be used for all future communication.
    arg 2) The socket we are listening on.
    arg 3) The pool for the new socket.

=cut
 */
ap_status_t ap_accept(ap_socket_t **new, const ap_socket_t *sock, 
                      ap_pool_t *connection_pool);

/*

=head1 ap_status_t ap_connect(ap_socket_t *sock, char *hostname)

B<Issue a connection request to a socket either on the same machine or a different one.> 

    arg 1) The socket we wish to use for our side of the connection 
    arg 2) The hostname of the machine we wish to connect to.  If NULL,
           APR assumes that the sockaddr_in in the apr_socket is completely
           filled out.

=cut
 */
ap_status_t ap_connect(ap_socket_t *sock, char *hostname);

/*

=head1 ap_status_t ap_get_remote_hostname(char **name, ap_socket_t *sock)

B<Get name of the machine we are currently connected to.> 

    arg 1) A buffer to store the hostname in.
    arg 2) The socket to examine.

=cut
 */
ap_status_t ap_get_remote_hostname(char **name, ap_socket_t *sock);

/*

=head1 ap_status_t ap_gethostname(char *buf, ap_int32_t len, ap_pool_t *cont)

B<Get name of the current machine> 

    arg 1) A buffer to store the hostname in.
    arg 2) The maximum length of the hostname that can be stored in the
           buffer provided. 
    arg 3) The pool to use.

=cut
 */
ap_status_t ap_gethostname(char *buf, int len, ap_pool_t *cont);

/*

=head1 ap_status_t ap_get_socketdata(void **data, char *key, ap_socket_t *sock)

B<Return the pool associated with the current socket.>

    arg 1) The currently open socket.
    arg 2) The user data associated with the socket.

=cut
 */
ap_status_t ap_get_socketdata(void **data, char *key, ap_socket_t *sock);

/*

=head1 ap_status_t ap_set_socketdata(ap_socket_t *sock, void *data, char *key, ap_status_t (*cleanup) (void *))

B<Set the pool associated with the current socket.>

    arg 1) The currently open socket.
    arg 2) The user data to associate with the socket.
    arg 3) The key to associate with the data.
    arg 4) The cleanup to call when the socket is destroyed.

=cut
 */
ap_status_t ap_set_socketdata(ap_socket_t *sock, void *data, char *key,
                              ap_status_t (*cleanup) (void*));

/*

=head1 ap_status_t ap_send(ap_socket_t *sock, const char *buf, ap_ssize_t *len)

B<Send data over a network.>

    arg 1) The socket to send the data over.
    arg 2) The buffer which contains the data to be sent. 
    arg 3) On entry, the number of bytes to send; on exit, the number
           of bytes sent.

B<NOTE>:  This functions acts like a blocking write by default.  To change 
          this behavior, use ap_setsocketopt with the APR_SO_TIMEOUT option.
  
   It is possible for both bytes to be sent and an error to be returned.
  
   APR_EINTR is never returned.

=cut
 */
ap_status_t ap_send(ap_socket_t *sock, const char *buf, ap_ssize_t *len);

/*

=head1 ap_status_t ap_sendv(ap_socket_t *sock, const struct iovec *vec, ap_int32_t nvec, ap_int32_t *len)

B<Send multiple packets of data over a network.>

    arg 1) The socket to send the data over.
    arg 2) The array of iovec structs containing the data to send 
    arg 3) The number of iovec structs in the array
    arg 4) Receives the number of bytes actually written

B<NOTE>:  This functions acts like a blocking write by default.  To change 
          this behavior, use ap_setsocketopt with the APR_SO_TIMEOUT option.
          The number of bytes actually sent is stored in argument 3.
  
   It is possible for both bytes to be sent and an error to be returned.
  
   APR_EINTR is never returned.

=cut
 */
ap_status_t ap_sendv(ap_socket_t *sock, const struct iovec *vec, 
                     ap_int32_t nvec, ap_int32_t *len);

#if APR_HAS_SENDFILE
/*

=head1 ap_status_t ap_sendfile(ap_socket_t *sock, ap_file_t *file, ap_hdtr_t *hdtr, ap_off_t *offset, ap_size_t *len, ap_int32_t flags)

B<Send a file from an open file descriptor to a socket, along with optional headers and trailers>

    arg 1) The socket to which we're writing
    arg 2) The open file from which to read
    arg 3) A structure containing the headers and trailers to send
    arg 4) Offset into the file where we should begin writing
    arg 5) Number of bytes to send 
    arg 6) OS-specific flags to pass to sendfile()

B<NOTE>:  This functions acts like a blocking write by default.  To change 
          this behavior, use ap_setsocketopt with the APR_SO_TIMEOUT option.
          The number of bytes actually sent is stored in argument 5.

=cut
 */
ap_status_t ap_sendfile(ap_socket_t *sock, ap_file_t *file, ap_hdtr_t *hdtr, 
                        ap_off_t *offset, ap_size_t *len, ap_int32_t flags);
#endif

/*

=head1 ap_status_t ap_recv(ap_socket_t *sock, char *buf, ap_ssize_t *len)

B<Read data from a network.>

    arg 1) The socket to read the data from.
    arg 2) The buffer to store the data in. 
    arg 3) On entry, the number of bytes to receive; on exit, the number
           of bytes received.

B<NOTE>:  This functions acts like a blocking read by default.  To change 
          this behavior, use ap_setsocketopt with the APR_SO_TIMEOUT option.
          The number of bytes actually sent is stored in argument 3.
  
   It is possible for both bytes to be received and an APR_EOF or
   other error to be returned.
  
   APR_EINTR is never returned.

=cut
 */
ap_status_t ap_recv(ap_socket_t *sock, char *buf, ap_ssize_t *len);

/*

=head1 ap_status_t ap_setsocketopt(ap_socket_t *sock, ap_int32_t opt, ap_int32_t on)

B<Setup socket options for the specified socket> 

    arg 1) The socket to set up.
    arg 2) The option we would like to configure.  One of:
              APR_SO_DEBUG      --  turn on debugging information 
              APR_SO_KEEPALIVE  --  keep connections active
              APR_SO_LINGER     --  lingers on close if data is present
              APR_SO_NONBLOCK   --  Turns blocking on/off for socket
              APR_SO_REUSEADDR  --  The rules used in validating addresses
                                    supplied to bind should allow reuse
                                    of local addresses.
              APR_SO_TIMEOUT    --  Set the timeout value in microseconds.
                                    values < 0 mean wait forever.  0 means
                                    don't wait at all.
              APR_SO_SNDBUF     --  Set the SendBufferSize
              APR_SO_RCVBUF     --  Set the ReceiveBufferSize
    arg 3) Are we turning the option on or off.

=cut
 */
ap_status_t ap_setsocketopt(ap_socket_t *sock, ap_int32_t opt, ap_int32_t on);

/*

=head1 ap_status_t ap_set_local_port(ap_socket_t *sock, ap_uint32_t port)
      Assocaite a local port with a socket.

    arg 1) The socket to set
    arg 2) The local port this socket will be dealing with.

B<NOTE>:  This does not bind the two together, it is just telling apr 
          that this socket is going to use this port if possible.  If
          the port is already used, we won't find out about it here.

=cut
 */
ap_status_t ap_set_local_port(ap_socket_t *sock, ap_uint32_t port);

/*

=head1 ap_status_t ap_set_remote_port(ap_socket_t *sock, ap_uint32_t port)

B<Assocaite a remote port with a socket.>

    arg 1) The socket to enquire about.
    arg 2) The local port this socket will be dealing with.

B<NOTE>:  This does not make a connection to the remote port, it is just 
          telling apr which port ap_connect() should attempt to connect to.

=cut
 */
ap_status_t ap_set_remote_port(ap_socket_t *sock, ap_uint32_t port);

/*

=head1 ap_status_t ap_get_local_port(ap_uint32_t *port, ap_socket_t *sock)

B<Return the local port with a socket.>

    arg 1) The local port this socket is associated with.
    arg 2) The socket to enquire about.

=cut
 */
ap_status_t ap_get_local_port(ap_uint32_t *port, ap_socket_t *sock);

/*

=head1 ap_status_t ap_get_remote_port(ap_uint32_t *port, ap_socket_t *sock)

B<Return the remote port associated with a socket.>

    arg 1) The remote port this socket is associated with.
    arg 2) The socket to enquire about.

=cut
 */
ap_status_t ap_get_remote_port(ap_uint32_t *port, ap_socket_t *sock);

/*

=head1 ap_status_t ap_set_local_ipaddr(ap_socket_t *sock, cont char *addr)

B<Assocaite a local socket addr with an apr socket.>

    arg 1) The socket to use 
    arg 2) The IP address to attach to the socket.
           Use APR_ANYADDR to use any IP addr on the machine.

B<NOTE>:  This does not bind the two together, it is just telling apr 
          that this socket is going to use this address if possible. 

=cut
 */
ap_status_t ap_set_local_ipaddr(ap_socket_t *sock, const char *addr);

/*

=head1 ap_status_t ap_set_remote_ipaddr(ap_socket_t *sock, cont char *addr)

B<Assocaite a remote socket addr with an apr socket.>

    arg 1) The socket to use 
    arg 2) The IP address to attach to the socket.

B<NOTE>:  This does not make a connection to the remote address, it is just
          telling apr which address ap_connect() should attempt to connect to.

=cut
 */
ap_status_t ap_set_remote_ipaddr(ap_socket_t *sock, const char *addr);

/*

=head1 ap_status_t ap_get_local_ipaddr(char **addr, const ap_socket_t *sock)

B<Return the local IP address associated with an apr socket.>

    arg 1) The local IP address associated with the socket.
    arg 2) The socket to use 

=cut
 */
ap_status_t ap_get_local_ipaddr(char **addr, const ap_socket_t *sock);

/*

=head1 ap_status_t ap_get_remote_ipaddr(char **addr, const ap_socket_t *sock)

B<Return the remote IP address associated with an apr socket.>

    arg 1) The remote IP address associated with the socket.
    arg 2) The socket to use 

=cut
 */
ap_status_t ap_get_remote_ipaddr(char **addr, const ap_socket_t *sock);

/*

=head1 ap_status_t ap_get_local_name(struct sockaddr_in **name, const ap_socket_t *sock)

B<Return the local socket name as a BSD style struct sockaddr_in.>

    arg 1) The local name associated with the socket.
    arg 2) The socket to use 

=cut
 */
ap_status_t ap_get_local_name(struct sockaddr_in **name, const ap_socket_t *sock);

/*

=head1 ap_status_t ap_get_remote_name(struct sockaddr_in **name, const ap_socket_t *sock)

B<Return the remote socket name as a BSD style struct sockaddr_in.>

    arg 1) The remote name associated with the socket.
    arg 2) The socket to use 

=cut
 */
ap_status_t ap_get_remote_name(struct sockaddr_in **name, const ap_socket_t *sock);

/*

=head1 ap_status_t ap_setup_poll(ap_pollfd_t **new, ap_int32_t num, ap_pool_t *cont)

B<Setup the memory required for poll to operate properly.>

    arg 1) The poll structure to be used. 
    arg 2) The number of socket descriptors to be polled.
    arg 3) The pool to operate on.

=cut
 */
ap_status_t ap_setup_poll(ap_pollfd_t **new, ap_int32_t num, 
                          ap_pool_t *cont);

/*

=head1 ap_status_t ap_poll(ap_pollfd_t *aprset, ap_int32_t *nsds, ap_interval_time_t timeout)

B<Poll the sockets in the poll structure.>

    arg 1) The poll structure we will be using. 
    arg 2) The number of sockets we are polling. 
    arg 3) The amount of time in microseconds to wait.  This is a maximum, not 
           a minimum.  If a socket is signalled, we will wake up before this
           time.  A negative number means wait until a socket is signalled.

B<NOTE>:  The number of sockets signalled is returned in the second argument. 

          This is a blocking call, and it will not return until either a 
          socket has been signalled, or the timeout has expired. 

=cut
 */
ap_status_t ap_poll(ap_pollfd_t *aprset, ap_int32_t *nsds, ap_interval_time_t timeout);

/*

=head1 ap_status_t ap_add_poll_socket(ap_pollfd_t *aprset, ap_socket_t *sock, ap_int16_t event)

B<Add a socket to the poll structure.> 

    arg 1) The poll structure we will be using. 
    arg 2) The socket to add to the current poll structure. 
    arg 3) The events to look for when we do the poll.  One of:
              APR_POLLIN    -- signal if read will not block
              APR_POLLPRI   -- signal if prioirty data is availble to be read
              APR_POLLOUT   -- signal if write will not block

=cut
 */
ap_status_t ap_add_poll_socket(ap_pollfd_t *aprset, ap_socket_t *socket, 
                               ap_int16_t event);

/*

=head1 ap_status_t ap_mask_poll_socket(ap_pollfd_t *aprset, ap_socket_t *sock, ap_int16_t events)

B<Modfify a socket in the poll structure with mask.> 

    arg 1) The poll structure we will be using. 
    arg 2) The socket to modify in poll structure. 
    arg 3) The events to stop looking for during the poll.  One of:
              APR_POLLIN    -- signal if read will not block
              APR_POLLPRI   -- signal if prioirty data is availble to be read
              APR_POLLOUT   -- signal if write will not block

=cut
 */
ap_status_t ap_mask_poll_socket(ap_pollfd_t *aprset, ap_socket_t *sock,
                                  ap_int16_t events);
/*

=head1 ap_status_t ap_remove_poll_socket(ap_pollfd_t *aprset, ap_socket_t *sock)

B<Remove a socket from the poll structure.> 

    arg 1) The poll structure we will be using. 
    arg 2) The socket to remove from the current poll structure. 

=cut
 */
ap_status_t ap_remove_poll_socket(ap_pollfd_t *aprset, ap_socket_t *sock);

/*

=head1 ap_status_t ap_clear_poll_sockets(ap_pollfd_t *aprset, ap_int16_t events)

B<Remove all sockets from the poll structure.> 

    arg 1) The poll structure we will be using. 
    arg 2) The events to clear from all sockets.  One of:
              APR_POLLIN    -- signal if read will not block
              APR_POLLPRI   -- signal if prioirty data is availble to be read
              APR_POLLOUT   -- signal if write will not block

=cut
 */
ap_status_t ap_clear_poll_sockets(ap_pollfd_t *aprset, ap_int16_t events);

/*

=head1 ap_status_t ap_get_revents(ap_int_16_t *event, ap_socket_t *sock, ap_pollfd_t *aprset)

B<Get the return events for the specified socket.> 

    arg 1) The returned events for the socket.  One of:
              APR_POLLIN    -- Data is available to be read 
              APR_POLLPRI   -- Prioirty data is availble to be read
              APR_POLLOUT   -- Write will succeed
              APR_POLLERR   -- An error occurred on the socket
              APR_POLLHUP   -- The connection has been terminated
              APR_POLLNVAL  -- This is an invalid socket to poll on.
                               Socket not open.
    arg 2) The socket we wish to get information about. 
    arg 3) The poll structure we will be using. 

=cut
 */
ap_status_t ap_get_revents(ap_int16_t *event, ap_socket_t *sock, 
                           ap_pollfd_t *aprset);

/*

=head1 ap_status_t ap_get_polldata(ap_pollfd_t *pollfd, char *key, void *data)

B<Return the pool associated with the current poll.>

    arg 1) The currently open pollfd.
    arg 2) The key to use for retreiving data associated with a poll struct.
    arg 3) The user data associated with the pollfd.

=cut
 */
ap_status_t ap_get_polldata(ap_pollfd_t *pollfd, char *key, void *data);

/*

=head1 ap_status_t ap_set_polldata(ap_pollfd_t *pollfd, void *data, char *key, ap_status_t (*cleanup) (void *))

B<Return the pool associated with the current poll.>

    arg 1) The currently open pollfd.
    arg 2) The user data to associate with the pollfd.

=cut
 */
ap_status_t ap_set_polldata(ap_pollfd_t *pollfd, void *data, char *key,
                            ap_status_t (*cleanup) (void *));

/*

=head1 ap_status_t ap_socket_from_file(ap_socket_t **newsock, ap_file_t *file)

B<Convert a File type to a socket so that it can be used in a poll operation.>

    arg 1) the newly created socket which represents a file.
    arg 2) the file to mask as a socket.

B<NOTE>: This is not available on all platforms.  Platforms that have the
         ability to poll files for data to be read/written/exceptions will
         have the APR_FILES_AS_SOCKETS macro defined as true.

=cut
 */
ap_status_t ap_socket_from_file(ap_socket_t **newsock, ap_file_t *file);


/*  accessor functions   */

#ifdef __cplusplus
}
#endif

#endif  /* ! APR_NETWORK_IO_H */

