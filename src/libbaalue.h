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
 * common types
 * -------------
 */


/*
 * common macros
 * -------------
 */
#define _(string) gettext(string)

#define PRINT_LOCATION() do {			      \
		info_msg(_("Your're in %s of %s"),    \
			 __FUNCTION__, __FILE__);     \
	} while (0)


/*
 * helper.c
 * ========
 */


/*
 * network.c
 * =========
 */



/*
 * error.c
 * =======
 */

/*
 * +-------------------------+------------+------------+--------------------------+
 * |     function            | use errno? | terminate? | log_level (man 3 syslog) |
 * +-------------------------+------------+------------+--------------------------+
 * | baa_error_exit          |     yes    |   exit()   |         LOG_ERR          |
 * | baa_info_exit           |     no     |   exit()   |         LOG_ERR          |
 * | baa_dump_exit           |     yes    |  abort()   |         LOG_ERR          |
 * | baa_error_msg           |     yes    |    no      |         LOG_ERR          |
 * | baa_info_msg            |     no     |    no      |         LOG_INFO         |
 * | baa_debug_msg           |     yes    |    no      |         LOG_DEBUG        |
 * +---------------------+------------+------------+--------------------------+
 * | baa_th_error_msg        | errno_val  |    no      |         LOG_ERR          |
 * | baa_th_error_exit       | errno_val  |   exit()   |         LOG_ERR          |
 * | baa_th_dump_exit        | errno_val  |  abourt()  |         LOG_ERR          |
 * +-------------------------+------------+------------+--------------------------+
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


#endif
