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


/*
 * thread/fiber -> schedule entity
 */
static void
fiber_0 (void)
{
	function_0();
}

static void
fiber_1 (void)
{
	function_1();
	function_2();
	function_3();
	function_4();
}


/*
 * schedule table (expect 2 cores -> like Allwinner A20)
 *
 *  Core 0     Core 1
 * ---------------------
 *  fiber_0   fiber_1
 *
 * fiber_0 (prio 89) running on cpu 1 calls function_0 with cyclic time of 10ms
 * fiber_1 (prio 90) running on cpu 0 calls function_1,
 *                                          function_2,
 *                                          function_3 and
 *                                          function_4 with cyclic time of 100ms
 */
size_t num_fiber_elements = 2;
fiber_element_t fiber_array[] =
{
	{
		.func = fiber_0,
		.sched_param = { .sched_priority = 89,
		},
		.cpu = 0,
		.policy = SCHED_FIFO,
		.dt = MS_TO_NS(10),
	},
	{
		.func = fiber_1,
		.sched_param = { .sched_priority = 90,
		},
		.cpu = 1,
		.policy = SCHED_RR,
		.dt = MS_TO_NS(100),
	}
};


/*
 * the other parts -> not time-triggert related
 */

static char *program_name;
static sigset_t mask;


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
 * This examples show how to use of my simple time triggert infrastructure
 *
 * -> this process hold the schedule table (the client)
 *    (sudo ./time_triggert_simple)
 */
int main(int argc, char *argv[])
{
	pthread_t tid_signal_handler;

	baa_set_program_name(&program_name, argv[0]);

	int c;
	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		case 'h':
			usage(EXIT_SUCCESS);
			break;
		default:
			baa_error_msg("ERROR: no valid argument");
			usage(EXIT_FAILURE);
		}
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
	 * start schedule table
	 */
	err = baa_drop_capability(CAP_SYS_NICE);
	if (err == -1)
		exit(EXIT_FAILURE);

	err = baa_build_scheduler(fiber_array, num_fiber_elements);
	if (err != 0)
		baa_error_exit("could not build schedule table");

	if (!baa_is_fiber_config_valid(fiber_array, num_fiber_elements))
		baa_error_exit("actual fiber config is not valid");

	err = baa_set_schedule_props(fiber_array, num_fiber_elements);
	if (err != 0)
		baa_error_exit("could not set schedule properties");

	fflush(stdout);

	err = baa_start_scheduler(fiber_array, num_fiber_elements);
	if (err != 0)
		baa_error_exit("could not start scheduler");

	(void) pthread_join(tid_signal_handler, NULL);

	exit(EXIT_SUCCESS);
}
