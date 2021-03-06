/*
  GPL
  (c) 2016-2020, thorsten.johannvorderbrueggen@t-online.de

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

#define USE_PID            BAA_BIT0
#define USE_BIND           BAA_BIT1
#define USE_CONNECT        BAA_BIT2
#define USE_LISTEN         BAA_BIT3

/* buffer 10 connection in one socket */
#define BACKLOG 10


/*
 * set socket snd/rcv timeouts
 */
static int
set_timeout(int sfd, int optval, int nsec)
{
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));

	tv.tv_sec = nsec;
	tv.tv_usec = 0;

	int ret = setsockopt(sfd, SOL_SOCKET, optval, &tv, sizeof(tv));
	if (ret == -1) {
		baa_errno_msg(_("could not set timeout"));
		return -1;
	}

#ifdef __DEBUG__
	baa_info_msg("set timeout to %d secs", tv.tv_sec);
#endif
	return 0;
}

BAALUE_LOCAL int
set_snd_timeout(int sfd)
{
	return set_timeout(sfd, SO_SNDTIMEO, SEND_TIMEOUT);
}

BAALUE_LOCAL int
set_rcv_timeout(int sfd)
{
	return set_timeout(sfd, SO_RCVTIMEO, RECV_TIMEOUT);
}

/*
 * alarm handler wich only returns for connect timeouts
 */
BAALUE_LOCAL void
handle_alarm(int sigio)
{
#ifdef __DEBUG__
	baa_info_msg("an connect timeout happens");
#endif
	return;
}

/*
 * unix domain socket
 */
static int
uds_socket(const char *name, const char *dir, char **socket_f, int type,
	   unsigned char flags)
{
	if (name == NULL)
		return -1;

	char *sfd_f = NULL;
	int sfd = -1;

	int n = -1;
	char str[BAA_MAXLINE];
	memset(str, 0, BAA_MAXLINE);

	if (dir == NULL) {
		if (flags & USE_PID)
			n = snprintf(str, BAA_MAXLINE,"%s.%ld", name,
				     (long) getpid());
		else
			n = snprintf(str, BAA_MAXLINE,"%s.%s", name,
				     UDS_NAME_ADD);
	} else {
		if (flags & USE_PID)
			n = snprintf(str, BAA_MAXLINE,"%s/%s.%ld", dir, name,
				     (long) getpid());
		else
			n = snprintf(str, BAA_MAXLINE,"%s/%s.%s", dir, name,
				     UDS_NAME_ADD);
	}

	if ((unlink(str) == -1) && (errno != ENOENT)) {
		baa_errno_msg(_("couln't unlink %s"), str);
		return -1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	if (strlen(str) > (sizeof(addr.sun_path) -1 )) {
		baa_error_msg(_("strlen(str) > (sizeof(add.sun_path) -1)  in %s"),
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
	strncpy(addr.sun_path, str, sizeof(addr.sun_path) - 1); /* len is okay */

	if (flags & USE_BIND)
		if (bind(sfd, (struct sockaddr *) &addr, SUN_LEN(&addr)) < 0) {
			baa_errno_msg(_("could not bind socket"));
			return -1;
		}

	if (flags & USE_CONNECT) {
		unsigned int nsec = CONNECT_TIMEOUT;
		sigfunc *old_handler = baa_signal(SIGALRM, handle_alarm);
		if (alarm(nsec) != 0)
			baa_info_msg(_("could not set alarm timer (%d sec)"), nsec);

		if (connect(sfd, (struct sockaddr *) &addr,
				sizeof(struct sockaddr_un)) < 0) {
			baa_errno_msg(_("could not connect socket"));
			return -1;
		}

		alarm(0);
		baa_signal(SIGALRM, old_handler);
	}

	(void) set_snd_timeout(sfd);
	(void) set_rcv_timeout(sfd);

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
	if (sfd == -1)
		return -1;

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

	char tmp_str[BAA_MAXLINE];
	memset(tmp_str, 0, BAA_MAXLINE);

	int n = snprintf(tmp_str, BAA_MAXLINE,"%s/%s", dir, file);

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
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;

	struct addrinfo *result = NULL;
	int ret = getaddrinfo(host, service, &hints, &result);
	if (ret != 0) {
		baa_info_msg(_("getaddrinfo in %s with %s"), __FUNCTION__,
			gai_strerror(ret));
		return -1;
	}

	unsigned int nsec = CONNECT_TIMEOUT;
	sigfunc *old_handler = baa_signal(SIGALRM, handle_alarm );
	if (alarm(nsec) != 0)
		baa_info_msg(_("could not set alarm timer (%d sec)"), nsec);

	int sfd = -1;
	struct addrinfo *rp = NULL;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
#ifdef __DEBUG__
		baa_info_msg(_("in function %s and go through addrinfo list"),
			__FUNCTION__);
#endif
                sfd = socket(result->ai_family,
			     result->ai_socktype,
			     result->ai_protocol);
                if (sfd == -1)
                        continue;

		(void) set_rcv_timeout(sfd);
		(void) set_snd_timeout(sfd);

		/* take the first valid one */
		ret = connect(sfd, result->ai_addr, result->ai_addrlen);
		if (ret == 0)
			break;

                close(sfd);
        }

	alarm(0);
	baa_signal(SIGALRM, old_handler);

	freeaddrinfo(result);

	return (rp == NULL) ? -1 : sfd;
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
bind_inet_socket(const char *service, int type, unsigned char flags)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = type;

	struct addrinfo *result = NULL;
	int ret = getaddrinfo(NULL, service, &hints, &result);
	if (ret != 0) {
		baa_info_msg(_("getaddrinfo in %s with %s"), __FUNCTION__,
			     gai_strerror(ret));
		return -1;
	}

	int sfd = -1;
	int opt_val = 1;
	struct addrinfo *rp = NULL;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
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

				freeaddrinfo(result);
				close(sfd);
				return -1;
			}
		}

		(void) set_snd_timeout(sfd);
		(void) set_rcv_timeout(sfd);

		/* take the first valid one */
		ret = bind(sfd, result->ai_addr, result->ai_addrlen);
		if (ret == 0)
                        break;

		/* close only if bind fails! */
		close(sfd);
	}

	freeaddrinfo(result);

	return (rp == NULL) ? -1 : sfd;
}

BAALUE_EXPORT int
baa_inet_dgram_server(const char *service)
{
	return bind_inet_socket(service, SOCK_DGRAM, 0);
}

BAALUE_EXPORT int
baa_inet_stream_server(const char *service)
{
	int sfd = bind_inet_socket(service, SOCK_STREAM, USE_LISTEN);
	if (sfd == -1)
		return -1;

	if (listen(sfd, BACKLOG) == -1) {
		baa_errno_msg(_("listen in %s"), __FUNCTION__);
		return -1;
	}

	return sfd;
}

BAALUE_EXPORT void *
baa_daytime_server_th(void *args)
{
	int sfd = *((int *) args);
	int confd = -1;

	char buf[BAA_MAXLINE];
	memset(&buf, 0, sizeof(buf));

	time_t ticks;
	ssize_t len;

	for (;;) {
		confd = accept(sfd, (struct sockaddr *) NULL, NULL);
		if (confd == -1)
			continue;

		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
		len = strlen(buf);

		if (write(confd, buf, len) != len)
			baa_errno_msg(_("could not write %s to socket"), buf);
	}

	return NULL;
}
