/*
  GPL
  (c) 2016, thorsten.johannvorderbrueggen@t-online.de

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

#include <libbaalue.h>
#include "libbaalue-private.h"

#define USE_PID 0x01
#define USE_BIND 0x02
#define USE_CONNECT 0x04
#define USE_LISTEN 0x08

// buffer 10 connection
#define BACKLOG 10


/*
 * unix domain socket
 */
static int
uds_socket(const char *name, const char *dir, char **socket_f, int type,
	   unsigned char flags)
{
	if (name == NULL || dir == NULL)
		return -1;

	char *sfd_f = NULL;
	int sfd = -1;

	int n = -1;
	char str[MAXLINE];
	memset(str, 0, MAXLINE);
	if (flags & USE_PID)
		n = snprintf(str, MAXLINE,"%s/%s.%ld", dir, name,
			     (long) getpid());
	else
		n = snprintf(str, MAXLINE,"%s/%s.%s", dir, name,
			     UDS_NAME_ADD);

	if ((unlink(str) == -1) && (errno != ENOENT)) {
		baa_errno_msg(_("couln't unlink %s"), str);
		return -1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	if (strlen(str) > sizeof(addr.sun_path)) {
		baa_error_msg(_("strlen(str) > sizeof(add.sun_path) in %s"),
			      __FUNCTION__);
		return -1;
	}

#ifdef __DEBUG__
	baa_info_msg(_("use %s as socket"), str);
#endif
	sfd = socket(AF_UNIX, type, 0);
	if (sfd == -1) {
		baa_errno_msg(_("could not open socket in %s"), __FUNCTION__);
		return -1;
	}

	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, str, sizeof(addr.sun_path) - 1);

	if (flags & USE_BIND)
		if (bind(sfd, (struct sockaddr *) &addr, SUN_LEN(&addr)) < 0) {
			baa_errno_msg(_("could not bind socket"));
			return -1;
		}

	if (flags & USE_CONNECT) {
		if (connect(sfd, (struct sockaddr *) &addr,
			    sizeof(struct sockaddr_un)) < 0)
			baa_errno_msg(_("could not connect socket"));
		return -1;
	}

	sfd_f = malloc(n + 1);
	if (sfd_f == NULL) {
		baa_errno_msg(_("sfd_f == NULL %s"), str);
		return -1;
	}

	memset(sfd_f, 0, n + 1);
	strncpy(sfd_f, str, n);
	*socket_f = sfd_f;

	return sfd;
}

BAALUE_EXPORT int
baa_uds_dgram_server(const char *name, const char *dir, char **socket_f)
{
	return uds_socket(name, dir, socket_f, SOCK_DGRAM, USE_BIND);
}

BAALUE_EXPORT int
baa_uds_stream_server(const char *name, const char *dir, char **socket_f)
{
	int sfd = uds_socket(name, dir, socket_f, SOCK_STREAM, USE_BIND);

	if (listen(sfd, BACKLOG) == -1) {
		baa_errno_msg(_("listen in %s"), __FUNCTION__);
		return -1;
	}

	return sfd;
}

BAALUE_EXPORT int
baa_uds_dgram_client(const char *name, const char *dir, char **socket_f)
{
	return uds_socket(name, dir, socket_f, SOCK_DGRAM, USE_PID | USE_BIND);
}

BAALUE_EXPORT int
baa_uds_stream_client(const char *name, const char *dir, char **socket_f)
{
	return uds_socket(name, dir, socket_f, SOCK_DGRAM, USE_PID | USE_CONNECT);
}

BAALUE_EXPORT int
baa_unlink_uds(int sfd)
{
	struct sockaddr_un addr;
	socklen_t len = sizeof(struct sockaddr_un);

	if (getsockname(sfd, (struct sockaddr *) &addr, &len ) == -1) {
		baa_errno_msg(_("could not get sun_path in %s"), __FUNCTION__);
		return -1;
	}

	if (unlink(addr.sun_path) == -1) {
		if (errno == ENOENT) {
			baa_info_msg(_("no such file %s"), addr.sun_path);
		} else {
			baa_errno_msg(_("could not unlink %s in %s"),
				      addr.sun_path, __FUNCTION__);
			return -1;
		}
	}

	return 0;
}

BAALUE_EXPORT char *
baa_create_uds_name_string(const char *file, const char *dir)
{
	if ((file == NULL) || (dir == NULL ))
		return NULL;

	char tmp_str[MAXLINE];
	memset(tmp_str, 0, MAXLINE);

	int n = snprintf(tmp_str, MAXLINE,"%s/%s", dir, file);

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	if (strlen(tmp_str) > sizeof(addr.sun_path)) {
		baa_error_msg(_("strlen(tmp_str) > sizeof(add.sun_path) in %s"),
			      __FUNCTION__);
		return NULL;
	}

	char *str = malloc(n + 1);
	if (str == NULL) {
		baa_errno_msg(_("str == NULL %s"), tmp_str);
		return NULL;
	}

	memset(str, 0, n + 1);
	strncpy(str, tmp_str, n);

#ifdef __DEBUG__
	baa_info_msg(_("assembled name %s"), str);
#endif
	return str;
}


/*
 * inet socket -> client side
 */
static int
connect_inet_socket(const char *host, const char *service, int type)
{
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;

	int ret = getaddrinfo(host, service, &hints, &result);
	if (ret != 0) {
		baa_info_msg(_("getaddrinfo in %s with %s"), __FUNCTION__,
			     gai_strerror(ret));
		return -1;
	}

	int sfd = -1;
	do {
                sfd = socket(result->ai_family,
			     result->ai_socktype,
			     result->ai_protocol);
                if (sfd == -1)
                        continue;

		/* take the first valid one */
		ret = connect(sfd, result->ai_addr, result->ai_addrlen);
		if (ret == 0)
			break;

                close(sfd);

        } while ((result = result->ai_next) != NULL);

	if (result == NULL) {
		baa_error_msg(_("something went wrong"));
		close(sfd);
		return -1;
	} else {
		freeaddrinfo(result);
	}

	return sfd;
}

BAALUE_EXPORT int
baa_inet_dgram_client(const char *host, const char *service)
{
	return connect_inet_socket(host, service, SOCK_DGRAM);
}

BAALUE_EXPORT int
baa_inet_stream_client(const char *host, const char *service)
{
	return connect_inet_socket(host, service, SOCK_STREAM);
}

/*
 * inet socket -> server side
 */
static int
bind_inet_socket(const char *host, const char *service, int type,
		 unsigned char flags)
{
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = type;

	int ret = getaddrinfo(host, service, &hints, &result);
	if (ret != 0) {
		baa_info_msg(_("getaddrinfo in %s with %s"), __FUNCTION__,
			     gai_strerror(ret));
		return -1;
	}

	int sfd = -1;
	int opt_val = 1;
	do {
                sfd = socket(result->ai_family,
			     result->ai_socktype,
			     result->ai_protocol);
                if (sfd == -1)
                        continue;

		if (flags & USE_LISTEN) {
			ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,
					 &opt_val, sizeof(opt_val));
			if (ret == -1) {
				baa_errno_msg(_("setsockopt in %s"),
					      __FUNCTION__);
				goto error;
			}
		}

		/* take the first valid one */
		ret = bind(sfd, result->ai_addr, result->ai_addrlen);
		if (ret == 0)
                        break;

		close(sfd);

        } while ((result = result->ai_next) != NULL);

	if (result == NULL) {
		baa_error_msg(_("something went wrong"));
		goto error;
	} else {
		freeaddrinfo(result);
	}

	return sfd;
error:
	if (result != NULL)
		freeaddrinfo(result);

	close(sfd);

	return -1;
}

BAALUE_EXPORT int
baa_inet_dgram_server(const char *host, const char *service)
{
	return bind_inet_socket(host, service, SOCK_DGRAM, 0);
}

BAALUE_EXPORT int
baa_inet_stream_server(const char *host, const char *service)
{
	int sfd = bind_inet_socket(host, service, SOCK_DGRAM, USE_LISTEN);

	if (listen(sfd, BACKLOG) == -1) {
		baa_errno_msg(_("listen in %s"), __FUNCTION__);
		return -1;
	}

	return sfd;
}
