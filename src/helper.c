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
baa_show_version_info()
{
	struct utsname u;

	baa_info_msg(_("Show content of version realted info: "));
	baa_info_msg(_("------------------------------------- "));
	baa_info_msg(_("Package version: %s                   "),
		PACKAGE_VERSION);

	if (uname(&u) == 0) {
		baa_info_msg(_("------------------------------------- "));
		baa_info_msg(_("System name: %s                       "),
			u.sysname);
		baa_info_msg(_("Kernel release: %s                    "),
			u.release);
		baa_info_msg(_("Version: %s                           "),
			u.version);
		baa_info_msg( _("------------------------------------ "));
		baa_info_msg(_("Architecture: %s                      "),
			u.machine);
	}

	baa_info_msg(_("------------------------------------- "));
}

BAALUE_EXPORT void
baa_show_package_name()
{
	baa_info_msg(_("Show content of package realted info: "));
	baa_info_msg(_("------------------------------------- "));
	baa_info_msg(_("Package name is %s                    "),
		PACKAGE_STRING);
	baa_info_msg(_("------------------------------------- "));
}

BAALUE_EXPORT void
baa_set_program_name(char **program_name, char *kdo_arg)
{
	char *whoami = NULL;
	char *tmp_str = NULL;
	size_t len = 0;

	if (kdo_arg == NULL)
		kdo_arg = "dummy";

	if ((whoami = strrchr(kdo_arg, '/')) == NULL)
		whoami = kdo_arg;
	else
		whoami++;

	len = strlen(whoami) + 1;
	tmp_str = malloc(len);
	/*
	 * this function is called at the beginning, so when malloc could not
	 * allocate the memory a exit is okay
	 */
	if (tmp_str == NULL)
		baa_error_exit(_("malloc in %s"), __FUNCTION__);

	memset(tmp_str, 0, len);
	strncat(tmp_str, whoami, len);

	*program_name = tmp_str;
}

BAALUE_EXPORT sigfunc *
baa_signal_old(int signo, sigfunc *func)
{
	struct sigaction actual, old_actual;

	memset(&actual, 0, sizeof(struct sigaction));
	memset(&old_actual, 0, sizeof(struct sigaction));

	actual.sa_handler = func;
	sigemptyset(&actual.sa_mask);
	actual.sa_flags = 0;

	if (signo == SIGALRM)
		; // do nothing -> we need it for timeout handling
	else
		actual.sa_flags |= SA_RESTART;

	if (sigaction(signo, &actual, &old_actual) < 0) {
		baa_errno_msg("signal_old -> sigaction()");
		return SIG_ERR;
	}

	return old_actual.sa_handler;
}

BAALUE_EXPORT ssize_t
baa_read_line(int fd, void *buf, size_t n_bytes)
{
	ssize_t n = 0;
	size_t count = 0;
	char *tmp_buf = buf;
	char ch = 0;

read_cmd:
	n = read(fd, &ch, 1);
	if (n == -1) {
		if (errno == EINTR)
			goto read_cmd;
		else {
			baa_errno_msg("read_line() -> read");
			return -1;
		}
	} else {
		if (n == 0 && count == 0)
			return BAA_FILE_EMPTY;

		if (count < n_bytes - 1) {
			count++;
			*tmp_buf++ = ch;
		}

		if ((ch != '\n') && (n != 0))   // NEWLINE or EOF
			goto read_cmd;
	}
	*tmp_buf = '\0';

	return count;
}

BAALUE_EXPORT int
baa_set_cloexec(int fd)
{
	int err = fcntl(fd, F_GETFD, 0);
	if (err == -1) {
		baa_errno_msg(_("fcntl() at line %d"), __LINE__);
		return -1;
	}

	int new = err | FD_CLOEXEC;

	err = fcntl(fd, F_SETFD, new);
	if (err == -1) {
		baa_errno_msg(_("fcntl() at line %d"), __LINE__);
		return -1;
	}

	return 0;
}

BAALUE_EXPORT int
baa_become_daemon(void)
{
	/* umask(0); */

	pid_t pid = fork();
	if (pid == -1) {
		baa_errno_msg(_("fork() at line %d"), __LINE__);
		return -1;
	}
	if (pid)
		_exit(EXIT_SUCCESS);

	if (setsid() == -1)
		baa_errno_msg(_("setsid() at line %d"), __LINE__);

	baa_signal_old(SIGHUP, SIG_IGN);

	pid = fork();
	if (pid == -1) {
		baa_errno_msg(_("fork() at line %d"), __LINE__);
		return -1;
	}
	if (pid)
		_exit(EXIT_SUCCESS);

	if (chdir("/") == -1) {
		baa_errno_msg(_("chdir() at line %d"), __LINE__);
		return -1;
	}

	int max_fd = sysconf(_SC_OPEN_MAX);
        if (max_fd == -1)
		max_fd = 64;  // should be enough

	int i;
        for (i = 0; i < max_fd; i++)
		close(i);

	int fd0 = open("/dev/null", O_RDONLY);
	int fd1 = open("/dev/null", O_RDWR);
	int fd2 = open("/dev/null", O_RDWR);

	if (fd0 != 0 || fd1 != 1 || fd2 != 2)
		baa_errno_msg(_("open() at line %d"), __LINE__);

	return 0;
}

BAALUE_EXPORT int
baa_lock_region(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	return fcntl(fd, F_SETLK, &fl);
}

BAALUE_EXPORT char *
baa_create_file_with_pid(const char *name, const char *dir)
{
	if (name == NULL || dir == NULL)
		return NULL;

	char *pid_file = NULL;
	int fd = -1;

	mode_t access_mode = O_RDWR | O_CREAT | O_TRUNC | O_EXCL;
	mode_t user_mode = S_IWUSR | S_IRUSR;

	char str[BAA_MAXLINE];
	memset(str, 0, BAA_MAXLINE);
	int n = snprintf(str, BAA_MAXLINE,"%s/%s.pid", dir, name);

	if ((unlink(str) == -1) && (errno != ENOENT)) {
		baa_errno_msg(_("could not unlink %s"), str);
		return NULL;
	}

	fd = open(str, access_mode, user_mode);
open_cmd:
	if (fd == -1) {
		if (errno == EINTR)
			goto open_cmd;

		baa_errno_msg(_("could not open %s"), str);
		return NULL;
	}

#ifdef __DEBUG__
	baa_info_msg(_("use %s as pid file"), str);
#endif
	pid_file = malloc(n + 1);
	if (pid_file == NULL) {
		baa_error_msg(_("pid_file == NULL %s"), str);
		goto error;
	}

	memset(pid_file, 0, n + 1);
	strncpy(pid_file, str, n);

	int flags = fcntl(fd, F_GETFD);
	if (fd == -1) {
		baa_errno_msg(_("could not set flag for %s"), str);
		goto error;
	}

	flags |= FD_CLOEXEC;

	if (fcntl(fd, F_SETFD, flags) == -1) {
		baa_errno_msg(_("could not set flag for %s"), str);
		goto error;
	}

	if (baa_lock_region(fd) == -1) {
		if (errno == EAGAIN || errno == EACCES)
			baa_error_msg(_("pid file %s already in-use"), str);
		else
			baa_error_msg(_("could not lock region in pid file: %s"), str);
		goto error;
	}

	int ret = -1;
ftruncate_cmd:
	ret = ftruncate(fd, 0);
	if (ret == -1) {
		if (errno == EINTR)
			goto ftruncate_cmd;

		baa_errno_msg(_("could not truncate pid file %s"), str);
		goto error;
	}

	memset(str, 0, BAA_MAXLINE);
	n = snprintf(str, BAA_MAXLINE, "%ld\n", (long) getpid());

	if (write(fd, str, n) != n) {
		baa_errno_msg(_("could not write %s to file %s"),
			      str, pid_file);
		goto error;
	}

	baa_wrap_close(fd);

	return pid_file;
error:
	if (pid_file != NULL)
		free(pid_file);

	if (fd != -1)
		close(fd);

	return NULL;
}

BAALUE_EXPORT int
baa_create_psem(char *name, sem_t **sem)
{
	mode_t access_mode = O_CREAT | O_EXCL;
	mode_t user_mode =  S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH;

	if ((*sem = sem_open(name, access_mode, user_mode, 1)) == SEM_FAILED) {
		if (errno == EEXIST) {
			baa_errno_msg(_("named semaphore %s already exist"), name);
			return EEXIST;
		} else {
			return -1;
		}
	}

	return 0;
}

BAALUE_EXPORT int
baa_open_psem(char *name, sem_t **sem)
{
	if ((*sem = sem_open(name, 0)) == SEM_FAILED)
		baa_errno_msg(_("could not open named semaphore %s"), name);
	else
		return 0;

	return 1;
}

BAALUE_EXPORT void
baa_unlink_psem(char *name)
{
	if (sem_unlink(name) == -1)
		baa_errno_msg(_("could not unlink named semaphore %s"), name);
}

BAALUE_EXPORT void
baa_close_psem(sem_t **sem)
{
	if (sem_close(*sem) == -1)
		baa_errno_msg(_("could not close named semaphore"));
}

BAALUE_EXPORT bool
baa_check_for_rtpreempt()
{
	FILE *fd;
	struct utsname u;

	if (uname(&u) == -1)
		baa_errno_msg(_("uname at line %d"), __LINE__);

	if (strstr(u.version, "PREEMPT") == NULL) {
		baa_error_msg(_("NO preempt kernel"));
		return false;
	}

	int flag;
	if ((fd = fopen("/sys/kernel/realtime","r")) != NULL) {
		if ((fscanf(fd, "%d", &flag) == 1) && (flag == 1)) {
			baa_errno_msg(_("kernel is a RT-PREEMPT kernel"));
			goto out;
		} else {
			baa_errno_msg(_("kernel is a PREEMPT kernel"));
			goto out;
		}
	} else {
		baa_info_msg(_("kernel is a PREEMPT kernel"));
		goto out;
	}

out:
	if (fd != NULL)
		fclose(fd);

	return true;
}

BAALUE_EXPORT void
baa_show_clock_resolution(clockid_t clock_type)
{
	struct timespec res;

	if (clock_getres(clock_type, &res) == 0)
		baa_info_msg(_("clock resoultion is %lu nsec"), res.tv_nsec);
	else
		baa_errno_msg(_("could not get clock resolution"));
}

BAALUE_EXPORT int
baa_drop_capability(int hold_capability)
{
	capng_clear(CAPNG_SELECT_BOTH);

	if (capng_update(CAPNG_ADD,
			 CAPNG_EFFECTIVE | CAPNG_PERMITTED,
			 hold_capability) != 0) {
		baa_error_msg(_("can't set capability -> %s"),
			      capng_print_caps_text(CAPNG_PRINT_STDOUT,
						    CAPNG_EFFECTIVE));
		return -1;
	}

	capng_apply(CAPNG_SELECT_BOTH);

	return 0;
}

BAALUE_EXPORT char *
baa_strlwr(char* str)
{
    char* s = str;

    for (;*s;++s)
        *s = tolower((unsigned char) *s);

    return str;
}

BAALUE_EXPORT char *
baa_strupr(char* str)
{
    char* s = str;

    for (;*s;++s)
        *s = toupper((unsigned char) *s);

    return str;
}
