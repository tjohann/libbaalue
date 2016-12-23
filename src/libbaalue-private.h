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

#ifndef _LIBBAALUE_PRIVATE_H_
#define _LIBBAALUE_PRIVATE_H_

#include <libbaalue.h>

#define BAALUE_EXPORT __attribute__ ((visibility ("default")))
#define BAALUE_LOCAL  __attribute__ ((visibility ("hidden")))

#define UDS_NAME_ADD "socket"

/*
 *  send an 'i have received ACK'
 */
#define SEND_RCV_ACK() do {						\
	num_send = sendto(kdo_s, &ptype_rcv_ack, 1, 0,			\
			  (struct sockaddr *) &addr, len);		\
	if (num_send != 1) 						\
		baa_errno_msg(_("num_send == -1 in %s"), __FUNCTION__);	\
	} while(0)


/*
 *  send an 'i have started the command ACK'
 */
#define SEND_CMD_ACK() do {						\
	num_send = sendto(kdo_s, &ptype_cmd_ack, 1, 0,			\
			  (struct sockaddr *) &addr, len);		\
	if (num_send != 1) 						\
		baa_errno_msg(_("num_send == -1 in %s"), __FUNCTION__);	\
	} while(0)

/*
 *  send an 'an error occured -> continue ...'
 */
#define SEND_ERROR() do {						\
	num_send = sendto(kdo_s, &ptype_error, 1, 0,			\
			  (struct sockaddr *) &addr, len);		\
	if (num_send != 1) 						\
		baa_errno_msg(_("num_send == -1 in %s"), __FUNCTION__);	\
	} while(0)

#endif
