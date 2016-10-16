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

#ifndef _LIBBAALUE_H_
#define _LIBBAALUE_H_

#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE

/* more or less common inc */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <wait.h>
#include <limits.h>
#include <linux/limits.h>
#include <sysexits.h>
#include <ctype.h>
#include <pthread.h>
#include <bsd/stdlib.h>
#include <sys/utsname.h>
#include <sched.h>
#include <time.h>
#include <sys/times.h>
#include <sys/sysinfo.h>
#include <semaphore.h>
#include <sys/syscall.h>

/* inotify inc */
#include <sys/inotify.h>

/* more or less network related inc */
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netpacket/packet.h>

/* libconfig */
#include <libconfig.h>

/* autotools generated config */
#include <config.h>

/* getopt and locale realted inc */
#include <getopt.h>
#include <libintl.h>
#include <locale.h>
#include "gettext.h"

/* libgit2 */
#include <git2.h>
#include <git2/clone.h>

/* libcurl */
#include <curl/curl.h>

/* can/lin */
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>

/* libarchive -> actually not needed */
/* #include <archive.h>
   #include <archive_entry.h> */

/* libressl -> actually not needed */
/* #include <openssl/sha.h> */

/* libcap-ng */
#include <cap-ng.h>

/* reboot/halt */
#include <sys/reboot.h>
#include <linux/reboot.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * common defines
 * -------------
 */

#define MAXLINE 254
#define DUMMY_STRING "dummy"
#define FILE_EMPTY -2

#ifndef __DEBUG__
#define VAR_RUN_DIR "/var/run"
#else
#define VAR_RUN_DIR "/tmp"
#endif

#define TMP_DIR "/tmp"
#define UDS_NAME_ADD "socket"
// max num of bytes per uds msg
#define MAX_LEN_MSG 100

/*
 * supported protocol type:
 * - PTYPE_SCHED_PROPS -> set scheduling propertys:
 *	num_unpacked = baa_unpack(buf, "cLLLL",
 *				  &protocol_type,  <- TODO: use this info?
 *				  &fiber_element.kernel_tid,
 *				  &fiber_element.policy,
 *				  &fiber_element.cpu,
 *				  &fiber_element.sched_param.sched_priority);
 * client:                            server:
 * |__ send one fiber-element __|
 *                                    |__ rcv and unpack __|
 *                                    |__ send PTYPE_RCV_ACK __|
 * |__ recv PTYPE_RCV_ACK __|
 *                                    |__ set values __|
 *                                    |__ send PTYPE_CMD_ACK __|
 * |__ recv PTYPE_CMD_ACK __|
 *
 * - PTYPE_DEVICE_MGMT -> start device managment functions:
 *      - t.b.d.
 * - PTYPE_DEVICE_MGMT_HALT -> independed subtype of device managment
 * - PTYPE_DEVICE_MGMT_REBOOT -> independed subtype of device managment
 */
#define PTYPE_SCHED_PROPS        0x00
#define PTYPE_DEVICE_MGMT        0x01
#define PTYPE_DEVICE_MGMT_HALT   0x02
#define PTYPE_DEVICE_MGMT_REBOOT 0x03
/*
 * PTYPE_ERROR -> unspecified error (default reaction -> continue)
 * PTYPE_RESET -> continue with next or leave thread/function
 */
#define PTYPE_RESET              0xEE
#define PTYPE_ERROR              0xEF
/*
 * PTYPE_RCV_ACK -> received the message
 * PTYPE_CMD_ACK -> triggered the cmd
 * PTYPE_VERIFY_ACK -> finished and verified the cmd
 */
#define PTYPE_CMD_ACK            0xFD
#define PTYPE_VERIFY_ACK         0xFE
#define PTYPE_RCV_ACK            0xFF

/* can_*_socket flags */
#define BIND_TO_SINGLE_CANIF 0x00
#define BIND_TO_ANY_CANIF 0x01

/*
 * common types
 * -------------
 */
/* shortcut for old signal api (baa_signal_old()) */
typedef	void sigfunc(int);

/* one function as part of the scheduling table */
typedef struct {
	pthread_t tid;
	pid_t kernel_tid;
	void (*func) (void);
	struct sched_param sched_param;
	struct timespec t;
	int policy;
	int cpu;
	int dt;
	int dt_sec;
	unsigned int safe_stack_fac;
	unsigned int safe_heap_fac;
} fiber_element_t;

/*
 * common macros
 * -------------
 */
#define NSEC_PER_SEC 1000000000
#define NS_TO_MS(val) (val / 1000000)
#define MS_TO_NS(val) (val * 1000000)

#define _(string) gettext(string)

#define BAA_PRINT_LOCATION() do {		      \
		baa_info_msg(_("Your're in %s of %s"),    \
			     __FUNCTION__, __FILE__);     \
	} while (0)


/*
 * helper.c
 * ========
 */
sigfunc *
baa_signal_old(int signo, sigfunc *func);

void
baa_set_program_name(char **program_name, char *kdo_arg);

void
baa_show_package_name(void);

void
baa_show_version_info(void);

ssize_t
baa_read_line(int fd, void *buf, size_t n_bytes);

int
baa_set_cloexec(int fd);

int
baa_become_daemon(void);

int
baa_lock_region(int fd);

char *
baa_create_file_with_pid(const char *name, const char *dir);

int
baa_create_psem(char *name, sem_t **sem);

int
baa_open_psem(char *name, sem_t **sem);

void
baa_unlink_psem(char *name);

void
baa_close_psem(sem_t **sem);

bool
baa_check_for_rtpreempt(void);

void
baa_show_clock_resolution(clockid_t clock_type);

int
baa_drop_capability(int hold_capability);

char *
baa_strupr(char* str);

char *
baa_strlwr(char* str);


/*
 * network.c
 * =========
 */

int
baa_uds_dgram_server(const char *name, const char *dir, char **socket_f);

int
baa_uds_stream_server(const char *name, const char *dir, char **socket_f);

int
baa_uds_dgram_client(const char *name, const char *dir, char **socket_f);

int
baa_uds_stream_client(const char *name, const char *dir, char **socket_f);

int
baa_unlink_uds(int sfd);

char *
baa_create_uds_name_string(const char *file, const char *dir);

int
baa_inet_dgram_client(const char *host, const char *service);

int
baa_inet_stream_client(const char *host, const char *service);

int
baa_inet_dgram_server(const char *service);

int
baa_inet_stream_server(const char *service);

void *
baa_daytime_server_th(void *args);


/*
 * process.c
 * =========
 */
void
baa_get_num_cpu(int *cpu_conf, int *cpu_onln);

int
baa_get_sched_policy(pid_t pid);

int
baa_get_sched_priority(pid_t pid);

long
baa_get_sched_time_slice_ms(pid_t pid);

void
baa_print_cpu_affinity(pid_t pid, size_t max);

void
baa_print_sched_policy(pid_t pid);

void
baa_print_sched_priority(pid_t pid);

void
baa_print_priority_range(int policy);

void
baa_print_sched_time_slice_ms(pid_t pid);

void
baa_print_num_cpu(void);


/*
 * error.c
 * =======
 */

/*
 * +---------------------+------------+------------+--------------------------+
 * |     function        | use errno? | terminate? | log_level (man 3 syslog) |
 * +---------------------+------------+------------+--------------------------+
 * | baa_error_exit      |     yes    |   exit()   |         LOG_ERR          |
 * | baa_info_exit       |     no     |   exit()   |         LOG_ERR          |
 * | baa_dump_exit       |     yes    |  abort()   |         LOG_ERR          |
 * | baa_error_msg       |     no     |    no      |         LOG_ERR          |
 * | baa_errno_msg       |     yes    |    no      |         LOG_ERR          |
 * | baa_info_msg        |     no     |    no      |         LOG_INFO         |
 * | baa_debug_msg       |     yes    |    no      |         LOG_DEBUG        |
 * +---------------------+------------+------------+--------------------------+
 * | baa_th_error_msg    | errno_val  |    no      |         LOG_ERR          |
 * | baa_th_error_exit   | errno_val  |   exit()   |         LOG_ERR          |
 * | baa_th_dump_exit    | errno_val  |  abourt()  |         LOG_ERR          |
 * +---------------------+------------+------------+--------------------------+
 */

/* print error message and exit */
void
__attribute__((noreturn)) baa_error_exit(const char *fmt, ...);

/* print info message and exit */
void
__attribute__((noreturn)) baa_info_exit(const char *fmt, ...);

/* print error message and dump/exit */
void
__attribute__((noreturn)) baa_dump_exit(const char *fmt, ...);

/* print error message */
void
baa_error_msg(const char *fmt, ...);

/* print error message */
void
baa_errno_msg(const char *fmt, ...);

/* print info message */
void
baa_info_msg(const char *fmt, ...);

/* print debug message */
void
baa_debug_msg(const char *fmt, ...);

/* print error message with errno = errno_val */
void
baa_th_error_msg(int errno_val, const char *fmt, ...);

/* print error message with errno = errno_val and dump/exit */
void
__attribute__((noreturn)) baa_th_dump_exit(int errno_val, const char *fmt, ...);

/* print error message with errno = errno_val and exit */
void
__attribute__((noreturn)) baa_th_error_exit(int errno_val, const char *fmt, ...);

/* enable/disable logging via syslog */
void
baa_enable_syslog(bool use_it, const char *name);


/*
 * wrapper.c
 * =========
 */

/*
 * wrapper functions around libc/syscalls
 *
 * usecase: - check all useful errors
 *          - in most cases exit due to failures (save state)
 */
void
baa_wrap_close(int fd);


/*
 * serialize.c
 * ===========
 */
int
baa_pack(unsigned char *buf, char *fmt, ...);

int
baa_unpack(unsigned char *buf, char *fmt, ...);


/*
 * sh_mq.c
 * =======
 */

// TODO: t.b.d


/*
 * sh_mem.c
 * ========
 */
int
baa_shmem_server(char *name, size_t size, void **mmap_seg);

int
baa_shmem_client(char *name, size_t size, void **mmap_seg);

void
baa_unlink_mmap_seg(char *name);


/*
 * fiber.c
 * =======
 */

void
baa_sigint_handler_sched(int sig);

void *
baa_fiber(void *arg);

int
baa_build_scheduler(fiber_element_t fiber_array[], int count);

int
baa_set_schedule_props(fiber_element_t fiber_array[], int count);

int
baa_start_scheduler(fiber_element_t fiber_array[], int count);

void
baa_ts_norm(struct timespec *t);

bool
baa_is_fiber_config_valid(fiber_element_t fiber_array[], int count);

/* set fiber element array via dedicated server (unix domain socket) */
int
baa_set_schedule_props_via_server(fiber_element_t fiber_array[], int count,
				  int sfd, char *kdo_f);

/* the server thread for baa_set_schedule_props_via_server */
void *
baa_schedule_server_th(void *args);


/*
 * can_lin.c
 * =========
 */

/* get a can socket -> CAN_RAW */
int
baa_can_raw_socket(char *ifname, struct sockaddr_can **addr, unsigned char flags);
/* get a can socket -> CAN_BCM */
int
baa_can_bcm_socket(char *ifname, struct sockaddr_can **addr, unsigned char flags);

/* set all hardware related error mask bits */
int
baa_set_hw_error_mask(int fd_s);

/* set can-id filter list to socket */
int
baa_set_flist(int fds, char *flist);


/*
 * device_mgmt.c
 * =============
 */

void
baa_reboot(void);

void
baa_halt(void);

/* device managment thread -> shutdown device ... */
void *
baa_device_mgmt_th(void *args);

int
baa_reboot_device(int sfd);

int
baa_halt_device(int sfd);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
