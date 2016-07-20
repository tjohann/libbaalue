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

// buffer 10 connection
#define BACKLOG 10

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

	if ((unlink(str) == -1) && (errno != ENOENT))
		baa_error_exit(_("couln't unlink %s"), str);

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	if (strlen(str) > sizeof(addr.sun_path)) {
		baa_info_msg(_("strlen(str) > sizeof(add.sun_path) in %s"),
			__FUNCTION__);
		return -1;
	}

#ifdef __DEBUG__
	info_msg(_("use %s as socket"), str);
#endif
	sfd = socket(AF_UNIX, type, 0);
	if (sfd == -1)
		baa_error_exit(_("could not open socket in %s"), __FUNCTION__);

	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, str, sizeof(addr.sun_path) - 1);

	if (flags & USE_BIND)
		if (bind(sfd, (struct sockaddr *) &addr, SUN_LEN(&addr)) < 0)
			baa_error_exit(_("could not bind socket"));

	if (flags & USE_CONNECT)
		if (connect(sfd, (struct sockaddr *) &addr,
			    sizeof(struct sockaddr_un)) < 0)
			baa_error_exit(_("could not connect socket"));

	sfd_f = malloc(n + 1);
	if (sfd_f == NULL)
		baa_error_exit(_("sfd_f == NULL %s"), str);

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

	if (listen(sfd, BACKLOG) == -1)
		baa_error_exit(_("listen in %s"), __FUNCTION__);

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

	if (getsockname(sfd, (struct sockaddr *) &addr, &len ) == -1)
		baa_error_exit(_("could not get sun_path in %s"), __FUNCTION__);

	if (unlink(addr.sun_path) == -1) {
		if (errno == ENOENT)
			baa_info_msg(_("no such file %s"), addr.sun_path);
		else
			baa_error_exit(_("could not unlink %s in %s"),
				addr.sun_path, __FUNCTION__);
	}

	return 0;
}

BAALUE_EXPORT char *
baa_get_uds_name_s(const char *file, const char *dir)
{
	if ((file == NULL) || (dir == NULL ))
		return NULL;

	char tmp_s[MAXLINE];
	memset(tmp_s, 0, MAXLINE);

	int n = snprintf(tmp_s, MAXLINE,"%s/%s", dir, file);

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	if (strlen(tmp_s) > sizeof(addr.sun_path)) {
		baa_info_msg(_("strlen(tmp_s) > sizeof(add.sun_path) in %s"),
			__FUNCTION__);
		return NULL;
	}

	char *str = malloc(n + 1);
	if (str == NULL)
		baa_error_exit(_("str == NULL %s"), tmp_s);

	memset(str, 0, n + 1);
	strncpy(str, tmp_s, n);

#ifdef __DEBUG__
	baa_info_msg(_("assembled name %s"), str);
#endif
	return str;
}


/*
 * TODO ....
 */
static int
connect_inet_socket(const char *host, const char *service, int type)
{
	struct addrinfo *result_sav = NULL;
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

	result_sav = result;

	int sfd = -1;
	do {
                sfd = socket(result->ai_family,
			     result->ai_socktype,
			     result->ai_protocol);
                if (sfd == -1)
                        continue;

		/* take the first valid socket */
		if (connect(sfd, result->ai_addr, result->ai_addrlen) == 0)
			break;

                if (close(sfd) == -1) {
			fprintf(stderr, "close\n");
			return -1;
		}

        } while ((result = result->ai_next) != NULL);

	freeaddrinfo(result_sav);

	if (result == NULL) {
		fprintf(stderr, "something went wrong\n");
		return -1;
	}

	return sfd;
}
