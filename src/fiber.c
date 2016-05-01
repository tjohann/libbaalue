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

// to sync all threads
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define CLEAN_SCHED_PROPS_CLIENT() do {				\
		num_packed = 0, num_read = 0, num_send = 0;	\
		len = sizeof(struct sockaddr_un);		\
		memset(buf, 0, MAX_LEN_MSG);			\
	} while(0)

#define CLEAN_SCHED_PROPS_SERVER() do {					\
		num_unpacked = 0, num_read = 0, num_send = 0;		\
		len = sizeof(struct sockaddr_un);			\
		memset(&addr, 0, len);					\
		memset(buf, 0, MAX_LEN_MSG);				\
		memset(&fiber_element, 0, sizeof(fiber_element));	\
	} while(0)

#define PRINT_SCHED_INFOS_REQUIRED() do {				\
		baa_info_msg("--------_BEGIN_-----------");		\
		baa_info_msg(_("show REQUIRED values"));		\
		baa_info_msg(_("all below for thread-id %d"),		\
			(int) fiber->kernel_tid);			\
		baa_info_msg(_("cpu %d"), fiber->cpu);			\
		baa_info_msg(_("policy %d"), fiber->policy);		\
		baa_info_msg(_("priority %d"),				\
			fiber->sched_param.sched_priority);		\
		baa_info_msg("---------_END_------------");		\
	} while (0)

#define PRINT_SCHED_INFOS_CONFIGURED() do {				\
		baa_info_msg("--------_BEGIN_-----------");		\
		baa_info_msg(_("show CONFIGURED values"));		\
		baa_print_sched_time_slice_ms(fiber_element.kernel_tid); \
		baa_print_sched_priority(fiber_element.kernel_tid);	\
		baa_print_sched_policy(fiber_element.kernel_tid);	\
		baa_info_msg("---------_END_------------");		\
	} while (0)

void
baa_sigint_handler_sched(int sig)
{
	(void) sig;

	/*
	 * only for int of clock_nanosleep;
	 * the rest is up for the application
	 */
	return;
}

static void
stack_prefault(size_t size)
{
        unsigned char dummy[size * BASE_SAFE_SIZE];
        memset(dummy, 0, size * BASE_SAFE_SIZE);
}

static int
heap_prefault(size_t size)
{
        unsigned char *dummy;
	size_t len = size * BASE_SAFE_SIZE;

	dummy = malloc(len);
	if (dummy == NULL)
		baa_error_exit(_("malloc in %s"), __FUNCTION__);

	memset(dummy, 0, len);

	return 0;
}

static inline
void ts_norm(struct timespec *t)
{
	while (t->tv_nsec >= NSEC_PER_SEC) {
		t->tv_nsec -= NSEC_PER_SEC;
		t->tv_sec++;
	}
}

void *
baa_fiber(void *arg)
{
	fiber_element_t *fiber = (fiber_element_t *) arg;
	pid_t kernel_tid = (pid_t) syscall(SYS_gettid);

	fiber->kernel_tid = kernel_tid;
	if (fiber->safe_stack_fac == 0)
		stack_prefault(DEF_MEM_FAC);
	else
		stack_prefault(fiber->safe_stack_fac);

	if (fiber->safe_heap_fac == 0)
		heap_prefault(DEF_MEM_FAC);
	else
		heap_prefault(fiber->safe_heap_fac);

	// wait until every fiber is up and ready
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);

	clock_gettime(CLOCK_MONOTONIC, &fiber->t);
	fiber->t.tv_sec += 1;
	fiber->t.tv_nsec = 0;

	int s = 0;
	for (;;) {
		s = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &fiber->t, NULL);
		if (s == EINTR)
			baa_info_msg(_("interrupted by signal SIGINT handler in %s"),
				__FUNCTION__);

		// call the function group
		fiber->func();

		fiber->t.tv_nsec += fiber->dt;
		ts_norm(&fiber->t);
	}

	return NULL;
}

int
baa_build_scheduler(fiber_element_t fiber_array[], int count)
{
	int ret = -1;

	for (int i = 0; i < count; i++) {
		ret = pthread_create(&fiber_array[i].tid,
					 NULL,
					 baa_fiber,
					 (void *) &fiber_array[i]);
		if (ret != 0) {
			baa_th_error_msg(ret, _("pthread_create in %s"), __FUNCTION__);
			return -1;
		}
	}

	// make sure everthing is ready
	sleep (1);

	return 0;
}

int
baa_set_schedule_props(fiber_element_t fiber_array[], int count)
{
	cpu_set_t set;
	fiber_element_t *fiber = NULL;

	for (int i = 0; i < count; i++) {
		fiber = &fiber_array[i];

#ifdef __DEBUG__
		PRINT_SCHED_INFOS_REQUIRED();
#endif
		CPU_ZERO(&set);
		CPU_SET(fiber->cpu, &set);

		if (CPU_ISSET(fiber->cpu, &set) == false)
			baa_error_exit(_("could not set fiber to CPU %d"), fiber->cpu);

		if (sched_setaffinity(fiber->kernel_tid, sizeof(cpu_set_t),
				      &set) == -1) {
			if (errno == EINVAL)
				baa_error_exit(_("no physical cpu %d"), fiber->cpu);
			else
				baa_error_exit(_("sched_setaffinity() == -1 "));
		}

		if (sched_setscheduler(fiber->kernel_tid, fiber->policy,
				       &fiber->sched_param))
			baa_error_exit("could not set scheduling policy %d",
				fiber->policy);
	}

	return 0;
}

int
baa_start_scheduler(fiber_element_t fiber_array[], int count)
{
	int ret = -1;

	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);

	for (int i = 0; i < count; i++) {
		ret = pthread_join(fiber_array[i].tid, NULL);
		if (ret != 0) {
			baa_error_msg(_("pthread_join in %s"), __FUNCTION__);
			return -1;
		}
	}

	return 0;
}

int
baa_set_schedule_props_via_server(fiber_element_t fiber_array[], int count,
				int sfd, char *kdo_f)
{
	unsigned char ptype_rcv_ack;
	socklen_t len;
	ssize_t num_read, num_send, num_packed;
	fiber_element_t *fiber = NULL;

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, kdo_f, sizeof(addr.sun_path) - 1);

	unsigned char buf[MAX_LEN_MSG];
	memset(buf, 0, sizeof(buf));

	for (int i = 0; i < count; i++) {
		CLEAN_SCHED_PROPS_CLIENT();

		fiber = &fiber_array[i];

		num_packed = baa_pack(buf, "cLLLL",
				PTYPE_SCHED_PROPS,
				(int) fiber->kernel_tid,
				fiber->policy,
				fiber->cpu,
				fiber->sched_param.sched_priority);

		if (num_packed > MAX_LEN_MSG) {
			baa_info_msg(_("message is to longer (%d) than %d"),
				num_packed, MAX_LEN_MSG);
			return -1;
		}

		num_send = sendto(sfd, buf, num_packed, 0,
				  (struct sockaddr *) &addr, len);
		if (num_send != num_packed)
			baa_error_exit(_("num_send != num_packed in %s"), __FUNCTION__);
#ifdef __DEBUG__
		baa_info_msg(_("send/pack %ld/%d bytes (protocol type %d) to %s"),
			(long) num_send, num_packed, PTYPE_SCHED_PROPS, addr.sun_path);
#endif
		num_read = recvfrom(sfd, &ptype_rcv_ack, 1, 0,
				    (struct sockaddr *) &addr, &len);
		if (num_read == -1)
			baa_error_exit(_("num_read == -1 in %s"), __FUNCTION__);

		if (ptype_rcv_ack != PTYPE_RCV_ACK) {
			baa_info_msg(_("wrong answer %d"), ptype_rcv_ack);
			return -1;
		}
	}

	return 0;
}

bool
baa_is_fiber_config_valid(fiber_element_t fiber_array[], int count)
{
	if (fiber_array == NULL || count == 0) {
		baa_info_msg(_("fiber_array == NULL || count == 0"));
		return false;
	}

	fiber_element_t *fiber = NULL;

	int max_prio = -1, min_prio = -1;
	int cpu_conf = -1, cpu_onln = -1;

	for (int i = 0; i < count; i++) {
		fiber = &fiber_array[i];
		switch (fiber->policy) {
		case SCHED_RR:
			min_prio = sched_get_priority_min(SCHED_RR);
			max_prio = sched_get_priority_max(SCHED_RR);

			if (fiber->sched_param.sched_priority > max_prio) {
				baa_info_msg(_("priority > %d"), max_prio);
				return false;
			}
			if (fiber->sched_param.sched_priority < min_prio) {
				baa_info_msg(_("priority < %d"), min_prio);
				return false;
			}
			break;
		case SCHED_FIFO:
			min_prio = sched_get_priority_min(SCHED_FIFO);
			max_prio = sched_get_priority_max(SCHED_FIFO);
			if (fiber->sched_param.sched_priority > max_prio) {
				baa_info_msg(_("priority > %d"), max_prio);
				return false;
			}
			if (fiber->sched_param.sched_priority < min_prio) {
				baa_info_msg(_("priority < %d"), min_prio);
				return false;
			}
			break;
		case SCHED_OTHER:
			if (fiber->sched_param.sched_priority != 0) {
				baa_info_msg(_("priority %d != 0"),
					fiber->sched_param.sched_priority);
				return false;
			}
			break;
		case SCHED_IDLE:
			if (fiber->sched_param.sched_priority != 0) {
				baa_info_msg(_("priority %d != 0"),
					fiber->sched_param.sched_priority);
				return false;
			}
			break;
		case SCHED_BATCH:
			if (fiber->sched_param.sched_priority != 0) {
				baa_info_msg(_("priority %d != 0"),
					fiber->sched_param.sched_priority);
				return false;
			}
			break;
		default:
			baa_info_msg(_("policy %d invalid"), fiber->policy);
			return false;
		}

		baa_get_num_cpu(&cpu_conf, &cpu_onln);
		if (fiber->cpu > cpu_conf) {
			baa_info_msg(_("cpu affinity > configured cpu (%d > %d)"),
				fiber->cpu, cpu_conf);
			return false;
		}

		/* end of checks*/
	}
	return true;
}

void *
baa_schedule_server_th(void *args)
{
	unsigned char buf[MAX_LEN_MSG];

	fiber_element_t fiber_element;
	ssize_t num_unpacked, num_read, num_send;

	struct sockaddr_un addr;
	socklen_t len;

	int kdo_s = *((int *) args);

	unsigned char protocol_type = 0x00;
	const unsigned char ptype_rcv_ack = PTYPE_RCV_ACK;

	int err = pthread_detach(pthread_self());
	if (err != 0)
		baa_th_error_msg(err, _("pthread_detach in %s"), __FUNCTION__);

	for(;;) {
		CLEAN_SCHED_PROPS_SERVER();

		num_read = recvfrom(kdo_s, buf, MAX_LEN_MSG, 0,
				    (struct sockaddr *) &addr, &len);
		if (num_read == -1)
			baa_error_exit(_("num_read == -1 in %s"), __FUNCTION__);

		if (buf[0] == PTYPE_SCHED_PROPS) {
			num_unpacked = baa_unpack(buf, "cLLLL",
						&protocol_type,
						&fiber_element.kernel_tid,
						&fiber_element.policy,
						&fiber_element.cpu,
						&fiber_element.sched_param.sched_priority);

			if (num_unpacked > MAX_LEN_MSG) {
				baa_info_msg(_("message is to longer (%d) than %d"),
					num_unpacked, MAX_LEN_MSG);
				continue;
			}

			err = baa_set_schedule_props(&fiber_element, 1);
			if (err != 0)
				baa_info_exit(_("Couldn't set schedule properties"));

			num_send = sendto(kdo_s, &ptype_rcv_ack, 1, 0,
					  (struct sockaddr *) &addr, len);
			if (num_send != 1)
				baa_error_exit(_("num_send == -1 in %s"), __FUNCTION__);

			PRINT_SCHED_INFOS_CONFIGURED();
		} else {
			baa_info_msg(_("protocol type %d not supported"), buf[0]);
		}

#ifdef __DEBUG__
		baa_info_msg(_("received/unpack %ld/%d bytes (protocol type %d) from %s"),
			(long) num_read, num_unpacked, protocol_type, addr.sun_path);
#endif
	}

	return NULL;
}
