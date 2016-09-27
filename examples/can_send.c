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

#include "libbaalue.h"

static char *program_name;
static sigset_t mask;


static void
cleanup(void)
{
#ifdef __DEBUG__
	baa_info_msg(_("finalize cleanup"));
#endif

	baa_info_msg(_("cheers %s"), getenv("USER"));
}

static void
show_some_infos(void)
{
	baa_show_package_name();
	baa_show_version_info();
}

static void
__attribute__((noreturn)) usage(int status)
{
	putchar('\n');
	baa_info_msg("Usage: %s [options]              ", program_name);
	baa_info_msg("Options:                                       ");
	baa_info_msg("       -i vcan0     -> interface               ");
	baa_info_msg("       -a 0x123     -> can addr                ");
	baa_info_msg("       -t 100       -> cylic time 100ms        ");
	baa_info_msg("       -h           -> show this help          ");
	putchar('\n');

        show_some_infos();
	exit(status);
}

static void *
signal_handler(void *args)
{
	(void) args;

	int sig = EINVAL;
	int err = -1;
	for (;;) {
		err = sigwait(&mask, &sig);
		if (err != 0)
			baa_error_exit("sigwait() != 0 in %s", __FUNCTION__);

		switch(sig) {
		case SIGTERM:
			baa_info_msg("catched signal \"%s\" (%d) -> exit now ",
				     strsignal(sig), sig);
			exit(EXIT_SUCCESS);
			break;
		case SIGHUP:
			baa_info_msg("signal \"%s\" (%d) -> ignore it",
				     strsignal(sig), sig);
			break;
		default:
			baa_error_msg("unhandled signal \"%s\" (%d)",
				      strsignal(sig), sig);
		}
	}

	return NULL;
}

static void *
print_error_frames(void *arg)
{
	char *ifname = (char *) arg;

	int ifname_s = baa_can_raw_socket(ifname, NULL, 0);
	if (ifname_s == -1) {
                baa_error_msg("could not init can_node %s", ifname);
		return NULL;
	}

	struct can_filter rfilter;
	rfilter.can_id   = 0x00;
	rfilter.can_mask = CAN_SFF_MASK;
	setsockopt(ifname_s, SOL_CAN_RAW, CAN_RAW_FILTER,
		   &rfilter, sizeof(rfilter));

	/* set all hardware related error mask bits */
	if (baa_set_hw_error_mask(ifname_s) == -1) {
		baa_error_msg("could not set error mask");
		return NULL;
	}

	struct can_frame frame;
	struct timeval tv;
	ssize_t recv_nbytes = -1;
	for (;;) {
		memset(&frame, 0, sizeof(frame));
		memset(&tv, 0, sizeof(tv));

		recv_nbytes = read(ifname_s, &frame, sizeof(struct can_frame));
		if (recv_nbytes == -1) {
			baa_errno_msg("read in print_error_frames");
			continue;
		}

		if (ioctl(ifname_s, SIOCGSTAMP, &tv) == -1)
			baa_errno_msg("ioctl in print_error_frames");

		if (frame.can_id & CAN_ERR_FLAG) {
			baa_info_msg("\n!-----! An ERROR flag is set !-----!");

			baa_info_msg("%s: received can frame -> \"frame.id:0x%x\" / \
\"frame.can_dlc:%d\" @ \"time:%s",
			       __FUNCTION__, frame.can_id, frame.can_dlc,
			       ctime((const time_t *) &tv.tv_sec));

			int i;
			for(i = 0; i < frame.can_dlc; i++)
				baa_info_msg("frame.data[%d] -> 0x%.2x", i, frame.data[i]);
		} else {
			baa_error_msg("something went wrong ... \
should never see that!");
		}
	}

	/* should never reached */
	close(ifname_s);

	return NULL;
}

static void
send_cyclic_telegram(char *ifname, unsigned long can_addr,
		     unsigned long cycle_time_ms)
{
	const unsigned char default_ms = 100;

	int fds = baa_can_raw_socket(ifname, NULL, 0);
	if (fds == -1) {
		baa_error_msg("could not init can_node %s for CAN_RAW", ifname);
		exit(EXIT_FAILURE);
	}

	struct can_frame frame;
	memset(&frame, 0, sizeof(frame));

	frame.can_id  = can_addr;
	frame.can_dlc = 2;

	struct timespec t;
	memset(&t, 0, sizeof(struct timespec));

	t.tv_sec = 0;
	if (cycle_time_ms == 0) {
		baa_info_msg("no cyclic time defined, use default value:%dms",
			     default_ms);
		t.tv_nsec = default_ms * 1000000;
	} else {
		t.tv_nsec = cycle_time_ms * 1000000;
		baa_ts_norm(&t);
	}

	ssize_t send_nbytes = -1;
	for (;;) {
		send_nbytes = write(fds, &frame, sizeof(struct can_frame));
		if (send_nbytes == -1) {
			baa_errno_msg("could not write frame");
			/* stop sending for some time -> user recognize it */
			sleep(4);
			continue;
		}

		if (frame.data[1] == 0xfe)
			frame.data[1] = 0;
		else
			frame.data[1] += 1;

		clock_nanosleep(CLOCK_MONOTONIC, 0, &t, NULL);
	}

	/* should never reached */
	close(fds);
}


/*
 * Description:
 *
 * This examples show how to use the linux can socket interface for a simple
 * can sender and receiver using CAN-RAW as protocol.
 *
 * -> can_send.c process is the sender (server)
 *    (./can_send -i vcan0 -a 0x100)
 * -> can_recv is the receiver (client)
 *    (./can_recv -i vcan0 -f 0x00F,0x10A)
 */
int
main(int argc, char *argv[])
{
	pthread_t tid_signal_handler;
	pthread_t tid_error_handler;

	unsigned long can_addr = 0;
	unsigned long cycle_time_ms = 0;
	char *ifname = NULL;

	int c;
	while ((c = getopt(argc, argv, "hei:a:t:")) != -1) {
		switch (c) {
		case 'i':
			ifname = optarg;
			break;
		case 'a':
			can_addr = strtoul(optarg, NULL, 16);
			break;
		case 't':
			cycle_time_ms = strtoul(optarg, NULL, 10);
			break;
		case 'h':
			usage(EXIT_SUCCESS);
			break;
		default:
			baa_info_msg("ERROR -> no valid argument");
			usage(EXIT_FAILURE);
		}
	}

	if (ifname == NULL)
		usage(EXIT_FAILURE);
	else
		baa_info_msg("will use %s as canif", ifname);

	if (can_addr == 0)
		usage(EXIT_FAILURE);
	else
		baa_info_msg("will use addr 0x%x", (int) can_addr);

	baa_set_program_name(&program_name, argv[0]);

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	/*
	 * signal handling -> a thread for signal handling
	 */
	sigfillset(&mask);
	err = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	if (err != 0)
		baa_th_error_exit(err, "could not set sigmask");

	err = pthread_create(&tid_signal_handler, NULL, signal_handler, 0);
	if (err != 0)
		baa_th_error_exit(err, "could not create sighandler pthread");

	/*
	 * error frame handler -> thread to receive hardware related error frames
	 */
	int ret = pthread_create(&tid_error_handler, NULL,
				 print_error_frames, ifname);
	if (ret != 0)
		baa_th_error_exit(err, "could not create error-handler pthread");

	send_cyclic_telegram(ifname, can_addr, cycle_time_ms);

	/* should never reached */
	baa_error_msg("something went wrong, you should never reach this");

	(void) pthread_join(tid_error_handler, NULL);
	(void) pthread_join(tid_signal_handler, NULL);

	exit(EXIT_SUCCESS);
}
