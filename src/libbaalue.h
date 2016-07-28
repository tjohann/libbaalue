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

// more or less common inc
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

// inotify inc
#include <sys/inotify.h>

// more or less network related inc
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netpacket/packet.h>

// libconfig
#include <libconfig.h>

// autotools generated config
#include <config.h>

// getopt and locale realted inc
#include <getopt.h>
#include <libintl.h>
#include <locale.h>
#include "gettext.h"

// libgit2
#include <git2.h>
#include <git2/clone.h>

// libcurl
#include <curl/curl.h>

// libarchive
#include <archive.h>
#include <archive_entry.h>

// libressl
#include <openssl/sha.h>

// libcap-ng
#include <cap-ng.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * common defines
 * -------------
 */

/*
 * make sure that at least this size is avalable without page fault
 * see https://rt.wiki.kernel.org/index.php/RT_PREEMPT_HOWTO
 */
#define BASE_SAFE_SIZE 1024
#define DEF_MEM_FAC 1

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

/* supported protocol type */
#define PTYPE_SCHED_PROPS 0x00
#define PTYPE_VERIFY_ACK  0xFE
#define PTYPE_RCV_ACK     0xFF

/*
 * common types
 * -------------
 */
// shortcut for old signal api (signal_old())
typedef	void sigfunc(int);

// one function as part of the scheduling table
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

#define _(string) gettext(string)

#define BAA_PRINT_LOCATION() do {		      \
		info_msg(_("Your're in %s of %s"),    \
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
baa_show_package_name();

void
baa_show_version_info();

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
baa_check_for_rtpreempt();

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
baa_get_uds_name_s(const char *file, const char *dir);


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
baa_print_num_cpu();


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
 * | baa_error_msg       |     yes    |    no      |         LOG_ERR          |
 * | baa_info_msg        |     no     |    no      |         LOG_INFO         |
 * | baa_debug_msg       |     yes    |    no      |         LOG_DEBUG        |
 * +---------------------+------------+------------+--------------------------+
 * | baa_th_error_msg    | errno_val  |    no      |         LOG_ERR          |
 * | baa_th_error_exit   | errno_val  |   exit()   |         LOG_ERR          |
 * | baa_th_dump_exit    | errno_val  |  abourt()  |         LOG_ERR          |
 * +---------------------+------------+------------+--------------------------+
 */

// print error message and exit
void
__attribute__((noreturn)) baa_error_exit(const char *fmt, ...);

void
__attribute__((noreturn)) baa_info_exit(const char *fmt, ...);

// print error message and dump/exit
void
__attribute__((noreturn)) baa_dump_exit(const char *fmt, ...);

// print error message
void
baa_error_msg(const char *fmt, ...);

// print info message
void
baa_info_msg(const char *fmt, ...);

// print debug message
void
baa_debug_msg(const char *fmt, ...);

// print error message with errno = errno_val
void
baa_th_error_msg(int errno_val, const char *fmt, ...);

// print error message with errno = errno_val and dump/exit
void
__attribute__((noreturn)) baa_th_dump_exit(int errno_val, const char *fmt, ...);

// print error message with errno = errno_val and exit
void
__attribute__((noreturn)) baa_th_error_exit(int errno_val, const char *fmt, ...);

// enable/disable logging via syslog
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

int
baa_set_schedule_props_via_server(fiber_element_t fiber_array[], int count,
				int sfd, char *kdo_f);

bool
baa_is_fiber_config_valid(fiber_element_t fiber_array[], int count);

void *
baa_schedule_server_th(void *args);

void
baa_ts_norm(struct timespec *t);


/*
 * can_lin.c
 * =========
 */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
