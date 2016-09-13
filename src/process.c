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

BAALUE_EXPORT void
baa_get_num_cpu(int *cpu_conf, int *cpu_onln)
{
	*cpu_conf = get_nprocs_conf();
	*cpu_onln = get_nprocs();
}

BAALUE_EXPORT int
baa_get_sched_policy(pid_t pid)
{
	return sched_getscheduler(pid);
}

BAALUE_EXPORT int
baa_get_sched_priority(pid_t pid)
{
	struct sched_param sp;
	memset(&sp, 0, sizeof(struct sched_param));

	int ret = sched_getparam(pid, &sp);
	if (ret == -1)
		return -1;
	else
		return sp.sched_priority;
}

BAALUE_EXPORT long
baa_get_sched_time_slice_ms(pid_t pid)
{
	struct timespec t;
	memset(&t, 0, sizeof(t));

	int ret = sched_rr_get_interval(pid, &t);
	if (ret == -1)
		return -1;

#ifdef __DEBUG__
	baa_info_msg(_("times slice sec: %d ns: %ld"), t.tv_sec ,t.tv_nsec);
#endif

	while (t.tv_nsec >= NSEC_PER_SEC) {
		t.tv_nsec -= NSEC_PER_SEC;
		t.tv_sec++;
	}

	return NS_TO_MS(t.tv_nsec);
}

BAALUE_EXPORT void
baa_print_cpu_affinity(pid_t pid, size_t max)
{
	cpu_set_t set;
	CPU_ZERO(&set);

	int ret = sched_getaffinity(pid, sizeof(cpu_set_t), &set);
	if (ret == -1) {
		baa_error_msg("could not get affinity for pid %ld", (long) pid);
	} else {
		size_t i;
		for (i = 0; i < max; i++) {
			int cpu;

			cpu = CPU_ISSET(i, &set);
			baa_info_msg(_("pid %d cpu=%d is %s"), pid, i,
				cpu ? "set" : "unset");
		}
	}
}

BAALUE_EXPORT void
baa_print_sched_policy(pid_t pid)
{
	int policy = baa_get_sched_policy(pid);
	if (policy == -1) {
		baa_error_msg(_("could not get schedule policy for pid %ld"),
			(long) pid);
	} else {
		switch(policy) {
		case SCHED_FIFO:
			baa_info_msg(_("scheduler SCHED_FIFO for pid %d"),
				(long) pid);
			break;
		case SCHED_RR:
			baa_info_msg(_("scheduler SCHED_RR for pid %d"),
				(long) pid);
			break;
		case SCHED_OTHER:
			baa_info_msg(_("scheduler SCHED_OTHER for pid %d"),
				(long) pid);
			break;
		case SCHED_IDLE:
			baa_info_msg(_("scheduler SCHED_IDLE for pid %d"),
				(long) pid);
			break;
		case SCHED_BATCH:
			baa_info_msg(_("scheduler SCHED_BATCH for pid %d"),
				(long) pid);
			break;
		default:
			baa_info_msg(_("unknown scheduler %d for %ld"),
				policy, (long) pid);
		}
	}
}

BAALUE_EXPORT void
baa_print_sched_priority(pid_t pid)
{
	baa_info_msg(_("priority  for pid %ld is %d"),
		(long) pid, baa_get_sched_priority(pid));
}

BAALUE_EXPORT void
baa_print_priority_range(int policy)
{
	int min, max;

	switch(policy) {
	case SCHED_FIFO:
		min = sched_get_priority_min(policy);
		max = sched_get_priority_max(policy);
		baa_info_msg(_("priorty range for SCHED_FIFO is %d - %d"), min, max);
		break;
	case SCHED_RR:
		min = sched_get_priority_min(policy);
		max = sched_get_priority_max(policy);
		baa_info_msg(_("priorty range for SCHED_RR is %d - %d"), min, max);
		break;
	default:
		baa_info_msg(_("priority range makes no sense for policy %d (must be 0)"),
			policy);
	}
}

BAALUE_EXPORT void
baa_print_sched_time_slice_ms(pid_t pid)
{
	baa_info_msg(_("time slice for pid %ld in ms %ld"),
		(long) pid, baa_get_sched_time_slice_ms(pid));
}

BAALUE_EXPORT void
baa_print_num_cpu()
{
	int cpu_conf = 0, cpu_onln = 0;

	baa_get_num_cpu(&cpu_conf, &cpu_onln);

	baa_info_msg(_("number configured cpu's %d"), cpu_conf);
	baa_info_msg(_("number online cpu's %d"), cpu_onln);
}
