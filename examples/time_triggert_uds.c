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

/*
 * functions -> the "real" work
 */
static void
function_0(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_1(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_2(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_3(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_4(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_5(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_6(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_7(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_8(void)
{
	BAA_PRINT_LOCATION();
}

static void
function_9(void)
{
	BAA_PRINT_LOCATION();
}


/*
 * thread/fiber -> schedule entity
 */
static void
fiber_0(void)
{
	function_0();
}

static void
fiber_1(void)
{
	function_1();
}

static void
fiber_2(void)
{
	function_2();
}

static void
fiber_3(void)
{
	function_8();
	function_9();
}

static void
fiber_4(void)
{
	function_3();
	function_4();
	function_5();
	function_6();
	function_7();
}


/*
 * schedule table (expect 2 cores -> like Allwinner A20)
 *
 *  Core 0     Core 1
 * ---------------------
 *  fiber_0   fiber_1
 *  fiber_3   fiber_2
 *            fiber_4
 *
 * fiber_0 (prio 90) running on cpu 1 calls function_0 with cyclic time of 1ms
 * fiber_1 (prio 89) running on cpu 0 calls function_1 with cyclic time of 10ms
 * fiber_2 (prio 88) running on cpu 1 calls function_2,
 * fiber_3 (prio 87) running on cpu 0 calls function_8 and
 *                                          function_9 with cyclic time of 100ms
 * fiber_4 (prio 86) running on cpu 1 calls function_3,
 *                                          function_4,
 *                                          function_5,
 *                                          function_6 and
 *                                          function_7 with cyclic time of 100ms
 */
size_t num_fiber_elements = 5;
fiber_element_t fiber_array[] =
{
	{
		.func = fiber_0,
		.sched_param = { .sched_priority = 90,
		},
		.policy = SCHED_FIFO,
		.cpu = 1,
		.dt = MS_TO_NS(1),
	},
	{
		.func = fiber_1,
		.sched_param = { .sched_priority = 89,
		},
		.policy = SCHED_RR,
		.dt = MS_TO_NS(10),
		.cpu = 0
	},
	{
		.func = fiber_2,
		.sched_param = { .sched_priority = 88,
		},
		.policy = SCHED_FIFO,
		.dt = MS_TO_NS(50),
		.cpu = 1
	},
	{
		.func = fiber_3,
		.sched_param = { .sched_priority = 87,
		},
		.policy = SCHED_RR,
		.dt = MS_TO_NS(100),
		.cpu = 0,
		.safe_stack_fac = 8
	},
	{
		.func = fiber_4,
		.sched_param = { .sched_priority = 86,
		},
		.policy = SCHED_BATCH,
		.dt = MS_TO_NS(999),
		.cpu = 1
	}
};


/*
 * the other parts -> not time-triggert related
 */

static char *program_name;
static sigset_t mask;

static int kdo_socket = -1;
static char *kdo_file;      /* to uds server */
static char *kdo_ret_file;  /* from uds server */

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
	baa_info_msg("        -h                       show help     ");
	baa_info_msg("        -f [server socket]                     ");
	baa_info_msg("        -d [socket dir]                        ");
	putchar('\n');
	baa_info_msg("Examples:                                      ");
	baa_info_msg("%s -d /var/tmp -f time_triggert_uds_server.socket",
		     program_name);
	baa_info_msg("%s -d /tmp -f time_triggert_uds_server.unix    ",
		     program_name);
	putchar('\n');
	baa_info_msg("Notes: The kernel must support at least PREEMPT");
	baa_info_msg("       For realtime RT-PREEMPT is mandatory    ");
	baa_info_msg("       See \"uname -a\"                        ");
	putchar('\n');

	show_some_infos();
	exit(status);
}

static void
cleanup(void)
{
#ifdef __DEBUG__
	baa_info_msg(_("finalize cleanup"));
#endif
	if (kdo_socket != -1)
		baa_wrap_close(kdo_socket);

	if (unlink(kdo_ret_file) != 0)
		baa_error_msg(_("could not remove %s"), kdo_ret_file);

	baa_info_msg(_("cheers %s"), getenv("USER"));
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
 * This examples show how to use a unix domain socket to configure a threads in
 * an unrelated process. The predefined thread baa_schedule_server_th does the
 * "magic". It controlls via the protocol typ PTYPE_SCHED_PROPS the
 * policy (SCHED_FIFO/...), the cpu affinity and the schedule priority for
 * a kernel-tid (see also time_triggert_udp_server.c)
 *
 * -> this process hold the schedule table (the client)
 *    (./time_triggert_uds -d /var/tmp -f lt-time_triggert_uds_server.socket )
 * -> this process is the unix domain server
 *    (sudo ./time_triggert_udp_server)
 */
int main(int argc, char *argv[])
{
	pthread_t tid_signal_handler;
	char *file = NULL;
	char *dir = NULL;

	baa_set_program_name(&program_name, argv[0]);

	int c;
	while ((c = getopt(argc, argv, "hf:d:")) != -1) {
		switch (c) {
		case 'f':
			file = optarg;
			break;
		case 'd':
			dir = optarg;
			break;
		case 'h':
			usage(EXIT_SUCCESS);
			break;
		default:
			baa_error_msg("ERROR: no valid argument");
			usage(EXIT_FAILURE);
		}
	}

	if (argc != 5) {
		baa_error_msg("not enough arguments");
		usage(EXIT_FAILURE);
	}

	int err = atexit(cleanup);
	if (err != 0)
		exit(EXIT_FAILURE);

	if (!baa_check_for_rtpreempt())
		usage(EXIT_FAILURE);

        /*
	 * signale handling -> a thread for signal handling
	 */
	sigfillset(&mask);
	err = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	if (err != 0)
		baa_th_error_exit(err, "could not set sigmask");

	err = pthread_create(&tid_signal_handler, NULL, signal_handler, 0);
	if (err != 0)
		baa_th_error_exit(err, "could not create pthread");

        /*
	 * connect to unix domain server
	 */
	kdo_file = baa_create_uds_name_string(file, dir);
	if (kdo_file == NULL)
		usage(EXIT_FAILURE);

	char *tmp_dir = getenv("TMPDIR");
	if (tmp_dir == NULL)
		tmp_dir = TMP_DIR;

	kdo_socket = baa_uds_dgram_client(program_name, VAR_RUN_DIR, &kdo_ret_file);
	if (kdo_socket == -1)
		baa_info_exit("could not create socket");


	err = baa_build_scheduler(fiber_array, num_fiber_elements);
	if (err != 0)
		baa_info_exit("could not build schedule table");

	if (!baa_is_fiber_config_valid(fiber_array, num_fiber_elements))
		baa_info_exit("actual fiber config is not valid");

	err = baa_set_schedule_props_via_server(fiber_array, num_fiber_elements,
						kdo_socket, kdo_file);
	if (err != 0)
		baa_info_exit("could not set schedule properties");

	fflush(stdout);

#ifdef __DEBUG__
	sleep(45); /* wait to check process attributes */
#endif
	err = baa_start_scheduler(fiber_array, num_fiber_elements);
	if (err != 0)
		baa_info_exit("could not start scheduler");

	(void) pthread_join(tid_signal_handler, NULL);

	exit(EXIT_SUCCESS);
}
