/*
  GPL                                                                        
  (c) 2015, thorsten.johannvorderbrueggen@t-online.de
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifndef _LIB_BAALUE_H
#define _LIB_BAALUE_H

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
//#include <bsd/stdlib.h>

// getopt and locale realted inc
#include <getopt.h>
#include <locale.h>
#include <libintl.h>

// time related inc
#include <time.h>
#include <sys/times.h>

// inotify inc
#include <sys/inotify.h>

// for confuse
#include <confuse.h>

// more or less network related inc
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netpacket/packet.h>

// libbaalue build config
#include "config.h"


/*
 * some common defines
 */
#define MAXLINE 254

/*
 * some info functions
 */

// print lib version -> see CMakeFiles.txt
void print_lib_version(void);



/*
 * our common error handling functions
 */

// common macros
#define ERROR_MSG_AND_RETURN(name) { error_msg(name); return -1; }


// print error message and exit
void
__attribute__((noreturn)) error_exit(const char *fmt, ...);

// print error message
void
error_msg(const char *fmt, ...);

#endif
