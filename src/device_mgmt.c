/*
  GPL
  (c) 2016-2018, thorsten.johannvorderbrueggen@t-online.de

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

#define CLEAN_DEVICE_MGMT_PROPS() do {					\
		num_read = 0;						\
		num_send = 0;						\
		len = sizeof(struct sockaddr_storage);			\
		memset(&addr, 0, len);					\
		memset(buf, 0, BAA_MAX_LEN_MSG);			\
	} while(0)


/*
 * ----------- client side functions/threads -----------
 */


/*
 * simple_device_cmd's are single byte commands like REBOOT/HALT or PING
 */
static int
simple_device_cmd(int sfd, unsigned char cmd)
{
#ifdef __DEBUG__
	baa_info_msg(_("command: %d"), cmd);
#endif
	ssize_t n = send(sfd, &cmd, 1, 0);
	if (n != 1) {
		baa_errno_msg(_("num_send != 1 in %s"), __FUNCTION__);
		return -1;
	}

	unsigned char ptype_rcv = 0x00;
	n = recv(sfd, &ptype_rcv, 1, 0);
	if (n == -1) {
		baa_errno_msg(_("num_read == -1 in %s"), __FUNCTION__);
		return -1;
	}
#ifdef __DEBUG__
	baa_info_msg(_("received protocol type %d"), ptype_rcv);
#endif
	switch (ptype_rcv) {
	case PTYPE_ERROR:
	case PTYPE_RESET:
		baa_error_msg(_("get PTYPE_ERROR/RESET from server"));
		return -1;
		break;
	case PTYPE_RCV_ACK:
		break;
	default:
		baa_error_msg(_("unknown PTYPE %d, try next one"),
			      ptype_rcv);
		return -1;
	}

	return 0;
}

BAALUE_EXPORT int
baa_reboot_device(int sfd)
{
	return simple_device_cmd(sfd, PTYPE_DEVICE_MGMT_REBOOT);
}

BAALUE_EXPORT int
baa_halt_device(int sfd)
{
	return simple_device_cmd(sfd, PTYPE_DEVICE_MGMT_HALT);
}

BAALUE_EXPORT int
baa_ping_device(int sfd)
{
	return simple_device_cmd(sfd, PTYPE_DEVICE_MGMT_PING);
}


/*
 * ----------- server side functions/threads -----------
 */

BAALUE_EXPORT void
baa_reboot(void)
{
	/*
	 * the normal way
	  sync();
	  reboot(LINUX_REBOOT_CMD_HALT);
	*/

	/* use runit to reboot device */
	system("init 6");
}

BAALUE_EXPORT void
baa_halt(void)
{
	/*
	 * the normal way
	 sync();
	 reboot(LINUX_REBOOT_CMD_RESTART);
	*/

	/* use runit to halt device */
	system("init 0");
}

BAALUE_EXPORT void *
baa_device_mgmt_th(void *args)
{
	unsigned char buf[BAA_MAX_LEN_MSG];

	ssize_t num_read, num_send;

	struct sockaddr_storage addr;
	socklen_t len;

	int kdo_s = *((int *) args);

	const unsigned char ptype_rcv_ack = PTYPE_RCV_ACK;
	for (;;) {
		CLEAN_DEVICE_MGMT_PROPS();

		num_read = recvfrom(kdo_s, buf, BAA_MAX_LEN_MSG, 0,
				    (struct sockaddr *) &addr, &len);
		if (num_read == -1) {
#ifdef __DEBUG__
			if (errno == EWOULDBLOCK)
				baa_info_msg(_("socket_timeout in %s"),
					__FUNCTION__);
			else
				baa_info_msg(_("num_read == -1 in %s"),
					__FUNCTION__);
#endif
			continue;
		}

		switch(buf[0]) {
		case PTYPE_DEVICE_MGMT:
#ifdef __DEBUG__
			baa_info_msg(_("protocol type -> PTYPE_DEVICE_MGMT"));
#endif
			/*
			 * TODO:
			 * - uptime
			 * - cpu load
			 * - mem usage
			 * - ...
			 */

			SEND_RCV_ACK();
			break;
		case PTYPE_DEVICE_MGMT_HALT:
#ifdef __DEBUG__
			baa_info_msg(_("protocol type -> PTYPE_DEVICE_MGMT_HALT"));
#endif
			SEND_RCV_ACK();
			baa_halt();
			break;
		case PTYPE_DEVICE_MGMT_REBOOT:
#ifdef __DEBUG__
			baa_info_msg(_("protocol type -> PTYPE_DEVICE_MGMT_REBOOT"));
#endif
			SEND_RCV_ACK();
			baa_reboot();
			break;
		case PTYPE_DEVICE_MGMT_PING:
#ifdef __DEBUG__
			baa_info_msg(_("protocol type -> PTYPE_DEVICE_MGMT_PING"));
#endif
			SEND_RCV_ACK();
			break;
		default:
			baa_error_msg(_("protocol type %d not supported"), buf[0]);
		}
	}

	return NULL;
}
