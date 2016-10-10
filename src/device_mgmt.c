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

#include <sys/reboot.h>
#include <linux/reboot.h>

#define CLEAN_DEVICE_MGMT_PROPS() do {					\
		num_read = 0;						\
		len = sizeof(struct sockaddr_storage);			\
		memset(&addr, 0, len);					\
		memset(buf, 0, MAX_LEN_MSG);				\
	} while(0)


BAALUE_EXPORT void *
baa_device_mgmt_th(void *args)
{
	unsigned char buf[MAX_LEN_MSG];

	struct sockaddr_storage addr;
	socklen_t len;
	ssize_t num_read;

	int kdo_s = *((int *) args);

	for (;;) {
		CLEAN_DEVICE_MGMT_PROPS();

		num_read = recvfrom(kdo_s, buf, MAX_LEN_MSG, 0,
				    (struct sockaddr *) &addr, &len);
		if (num_read == -1) {
			baa_errno_msg(_("num_read == -1 in %s"), __FUNCTION__);
			continue;
		}

		switch(buf[0]) {
		case PTYPE_DEVICE_MGMT:
#ifdef __DEBUG__
			baa_info_msg(_("protocol type -> PTYPE_DEVICE_MGMT"));
#endif
			/*
			 * TODO: implement HALT & REBOOT
			 */
			break;
		case PTYPE_DEVICE_MGMT_HALT:
#ifdef __DEBUG__
			baa_info_msg(_("protocol type -> PTYPE_DEVICE_MGMT_HALT"));
#endif
			sync();
			reboot(LINUX_REBOOT_CMD_HALT);
			break;
		case PTYPE_DEVICE_MGMT_REBOOT:
#ifdef __DEBUG__
			baa_info_msg(_("protocol type -> PTYPE_DEVICE_MGMT_HALT"));
#endif
			sync();
			reboot(LINUX_REBOOT_CMD_RESTART);

			break;
		default:
			baa_error_msg(_("protocol type %d not supported"), buf[0]);
		}
	}

	return NULL;
}
