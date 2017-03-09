/*
  GPL
  (c) 2017, thorsten.johannvorderbrueggen@t-online.de

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


#define GET_USERNAME 0x01
#define GET_USERDIR  0x02
#define GET_USERINFO 0x03


#define CHECK_BUF() do {						\
		if (buf == NULL) {					\
			baa_error_msg(_("buffer for user-name == NULL")); \
			return -1;					\
		}							\
	} while(0)


static int
get_user_infos(uid_t uid, char *buf, size_t size, unsigned char what)
{
	struct passwd *pwd = getpwuid(uid);
	if (pwd == NULL) {
		baa_error_msg(_("struct passwd == NULL"));
		return -1;
	}

	size_t len = 0;
	switch(what) {
	case GET_USERNAME:
		CHECK_BUF();
		len = strlen(pwd->pw_name);
		if (len > --size) {
			baa_error_msg(_("len(%d) > size(%d)"));
			return -1;
		}

		strncpy(buf, pwd->pw_name, size);
#ifdef __DEBUG__
		baa_info_msg(_("copied user-name %s to buffer(:%s)"), pwd->pw_name, buf);
#endif
		break;
	case GET_USERDIR:
		CHECK_BUF();
		len = strlen(pwd->pw_dir);
		if (len > --size) {
			baa_error_msg(_("len(%d) > size(%d)"));
			return -1;
		}

		strncpy(buf, pwd->pw_dir, size);
#ifdef __DEBUG__
		baa_info_msg(_("copied name of home dir %s to buffer(:%s)"),
			pwd->pw_dir, buf);
#endif
		break;
	case GET_USERINFO:
		CHECK_BUF();
		len = strlen(pwd->pw_gecos);
		if (len > --size) {
			baa_error_msg(_("len(%d) > size(%d)"));
			return -1;
		}

		strncpy(buf, pwd->pw_gecos, size);
#ifdef __DEBUG__
		baa_info_msg(_("copied user info %s to buffer(:%s)"),
			pwd->pw_gecos, buf);
#endif
		break;
	default:
		baa_error_msg(_("unknown type: %d"), what);
	}

	return 0;
}

BAALUE_EXPORT int
get_username(uid_t uid, char *buf, size_t size)
{
	return get_user_infos(uid, buf, size, GET_USERNAME);
}

BAALUE_EXPORT int
get_homedir(uid_t uid, char *buf, size_t size)
{
	return get_user_infos(uid, buf, size, GET_USERDIR);
}

BAALUE_EXPORT int
get_userinfo(uid_t uid, char *buf, size_t size)
{
	return get_user_infos(uid, buf, size, GET_USERINFO);
}

BAALUE_EXPORT uid_t
get_userid(const char *name)
{
	if (name == NULL) {
		baa_error_msg(_("user-name == NULL"));
		return -1;
	}

	struct passwd *pwd = getpwnam(name);
	if (pwd == NULL) {
		baa_error_msg(_("struct passwd == NULL"));
		return -1;
	}

	return pwd->pw_uid;
}

BAALUE_EXPORT gid_t
get_groupid_of_user(const char *name)
{
	if (name == NULL) {
		baa_error_msg(_("user-name == NULL"));
		return -1;
	}

	struct passwd *pwd = getpwnam(name);
	if (pwd == NULL) {
		baa_error_msg(_("struct passwd == NULL"));
		return -1;
	}

	return pwd->pw_gid;
}
