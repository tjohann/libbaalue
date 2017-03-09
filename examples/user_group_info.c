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

#include "libbaalue.h"

static char *program_name;

static void
show_some_infos(void)
{
	baa_show_package_name();
	baa_show_version_info();
}

/*
 * Description:
 *
 * This examples show how to use the provided user/group functions
 *
 */
int main(int argc, char *argv[])
{
	baa_set_program_name(&program_name, argv[0]);
	show_some_infos();

	char *user = getenv("USER");
	if (user == NULL)
		baa_error_exit(_("could not get user name from environment"));

	putchar('\n');
	baa_info_msg("-------------------------------------------");
	baa_info_msg("hello user %s ", user);
	baa_info_msg("-------------------------------------------");

	uid_t uid = get_userid(user);
	if (uid == -1)
		baa_error_msg("could not get user id -> ignore it");
	else
		baa_info_msg("- your user id is: %d", uid);

	gid_t gid = get_groupid_of_user(user);
	if (gid == -1)
		baa_error_msg("could not get group id -> ignore it");
	else
		baa_info_msg("- your group id is: %d", gid);

	char buf[BAA_MAXLINE];
	int err = get_homedir(uid, buf, BAA_MAXLINE);
	if (err == -1)
		baa_error_msg("could not net home dir -> ignore it");
	else
		baa_info_msg("- your home dir: %s", buf);

	err = get_userinfo(uid, buf, BAA_MAXLINE);
	if (err == -1)
		baa_error_msg("could not net user infos -> ignore it");
	else
		baa_info_msg("- your additional user entrys: %s", buf);

	err = get_groupname(gid, buf, BAA_MAXLINE);
	if (err == -1)
		baa_error_msg("could not get name of main group -> ignore it");
	else
		baa_info_msg("- your main group is named: %s", buf);


	baa_info_msg("-------------------------------------------");
	putchar('\n');
	baa_info_msg(_("cheers %s"), user);

	exit(EXIT_SUCCESS);
}
