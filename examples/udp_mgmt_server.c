/*
  GPL
  (c) 2015-2016, thorsten.johannvorderbrueggen@t-online.de

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
#include <signal.h>
#include <grp.h>

static char *program_name;
static sigset_t mask;

/* 8101-8114   Unassigned */
const char mgmt_port[] = "8101";

static void
cleanup(void)
{
#ifdef __DEBUG__
	baa_info_msg("finalize cleanup");
#endif

	baa_info_msg("cheers %s", getenv("USER"));
}

static void
show_some_infos(void)
{
	baa_show_package_name();
	baa_show_version_info();
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

/*
 * Description:
 *
 * This examples show how to use a datagram socket
 *
 * -> the server process is udp_mgmt_server.c
 *    (sudo ./udp_mgmt_server )
 * -> the client process is udp_mgmt_client.c
 *    (./udp_mgmt_client -s baalue_master)
 */
int main(int argc, char *argv[])
{
	pthread_t tid_signal_handler;
	pthread_t tid_mgmt_handler;

	baa_set_program_name(&program_name, argv[0]);

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	/*
	 * we dont need root rights
	err = baa_drop_capability(...);
	if (err == -1)
		exit(EXIT_FAILURE);
	*/
	show_some_infos();

	/*
	 * signal handling -> a thread for signal handling
	 */
	sigfillset(&mask);
	err = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	if (err != 0)
		baa_th_error_exit(err, "could not set sigmask");

	err = pthread_create(&tid_signal_handler, NULL, signal_handler, 0);
	if (err != 0)
		baa_th_error_exit(err, "could not create pthread");

        /*
	 * setup datagram server
	 */
	int fds = -1;
	fds = baa_inet_dgram_server(mgmt_port);
	if (fds == -1)
		baa_error_exit("could not setup server");

	/*
	 * start the thread to handle device managment server
	 */
	err = pthread_create(&tid_mgmt_handler, NULL,
			     baa_device_mgmt_th, (void *) &fds);
	if (err != 0)
		baa_th_error_exit(err, "could not create pthread");

	(void) pthread_join(tid_mgmt_handler, NULL);
        (void) pthread_join(tid_signal_handler, NULL);

	exit(EXIT_SUCCESS);
}
