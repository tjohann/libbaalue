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


BAALUE_EXPORT int
baa_get_groupname(gid_t gid, char *buf, size_t size)
{
	if (buf == NULL) {
		baa_error_msg(_("buffer for group name == NULL"));
		return -1;
	}

	struct group *grp = getgrgid(gid);
	if (grp == NULL) {
		baa_error_msg(_("struct group == NULL"));
		return -1;
	}

	size_t len = strlen(grp->gr_name);
	if (len > --size) {
		baa_error_msg(_("len(%d) > size(%d)"));
		return -1;
	}

	strncpy(buf, grp->gr_name, size);
#ifdef __DEBUG__
	baa_info_msg(_("copied group name %s to buffer %s"), grp->gr_name, buf);
#endif
	return 0;
}

BAALUE_EXPORT gid_t
baa_get_groupid(const char *name)
{
	if (name == NULL) {
		baa_error_msg(_("name == NULL"));
		return -1;
	}

	struct group *grp = getgrnam(name);
	if (grp == NULL) {
		baa_error_msg(_("struct group == NULL"));
		return -1;
	}

	return grp->gr_gid;
}
