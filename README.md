Libbaalue
=========

A base library for my different projects like sdk_builder (https://github.com/tjohann/sdk_builder) or baalued (https://github.com/tjohann/baalued.git). In this library all useful code from my different playgrounds like https://github.com/tjohann/time_triggert_env.git or https://github.com/tjohann/can_lin_env.git are migrated to (and will in the future).

WARNING: This is work in progress! Don't expect things to be complete in any dimension.


Overview
--------

The prefix for all functions and datatypes is "baa". See src/libbaalue.h for more detailed informations. All functions represent some parts i want to encapsulate to make the usage of a technologie easier. Topics like setup a datagram based unix domain socket server are provided by the library. Everthing is grouped in files which represent a certain topic, only helper.c is a summary of unspecified parts.

Below the examples folder you found simple programs which show the usage of the functions (see below for more info).


error.c
-------

Common error handling functions:

	/*
	* +-------------------+------------+------------+--------------------------+
	* |     function      | use errno? | terminate? | log_level (man 3 syslog) |
	* +-------------------+------------+------------+--------------------------+
	* | baa_error_exit    |     yes    |   exit()   |         LOG_ERR          |
	* | baa_info_exit     |     no     |   exit()   |         LOG_ERR          |
	* | baa_dump_exit     |     yes    |  abort()   |         LOG_ERR          |
	* | baa_error_msg     |     no     |    no      |         LOG_ERR          |
	* | baa_errno_msg     |     yes    |    no      |         LOG_ERR          |
	* | baa_info_msg      |     no     |    no      |         LOG_INFO         |
	* | baa_debug_msg     |     yes    |    no      |         LOG_DEBUG        |
	* +-------------------+------------+------------+--------------------------+
	* | baa_th_error_msg  | errno_val  |    no      |         LOG_ERR          |
	* | baa_th_error_exit | errno_val  |   exit()   |         LOG_ERR          |
	* | baa_th_dump_exit  | errno_val  |  abourt()  |         LOG_ERR          |
	* +-------------------+------------+------------+--------------------------+
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


helper.c
--------

Some general functions to become a daemon or posix semaphore handling:

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


network.c
---------

Network related topics like setup a unix domain socket server/clients or inet socket server/clients:

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
	baa_inet_dgram_server(const char *host, const char *service);

	int
	baa_inet_stream_server(const char *host, const char *service);


fiber.c & process.c
-------------------

Functions to setup a thread/process like scheduling policies or cpu affinity based on a datastructure to easily setup something like a time-triggert process environment (http://www.cs.utexas.edu/~mok/cs378/Spring16/Reading/KopetzTTvsET_1991.pdf and http://www.safetty.net/publications/pttes):

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


sh_mem.c
--------

Functions to handle posix shared memory:

	int
	baa_shmem_server(char *name, size_t size, void **mmap_seg);

	int
	baa_shmem_client(char *name, size_t size, void **mmap_seg);

	void
	baa_unlink_mmap_seg(char *name);


sh_mq.c
-------

Functions to handle posix message queues:

	t.b.d.


serialize.c
-----------

Simple functions to pack/unpack in a printf like style (https://en.wikipedia.org/wiki/The_Practice_of_Programming):

	int
	baa_pack(unsigned char *buf, char *fmt, ...);

	int
	baa_unpack(unsigned char *buf, char *fmt, ...);


wrapper.c
---------

Some wrapper functions around some linux/unix syscalls:

	/*
	* wrapper functions around libc/syscalls
	*
	* usecase: - check all useful errors
	*          - in most cases exit due to failures (save state)
	*/

	void
	baa_wrap_close(int fd);


can_lin.c
---------

Functions to handle CAN and/or LIN communication:

	t.b.d.


process.c (in addtion to fiber.c)
---------------------------------

Functions to handle special process topics like priority and cpu affinity:

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


device_mgmt.c
-------------

Functions for device managment like shotdown or get cpu load.

	void *
	device_mgmt_th(void *args);


Provided examples (folder ./examples)
-------------------------------------

Below the folder examples you find some usecases of libbaalue. All server are implemented as daemon and therefore use syslog. In most cases everthing is tested on different A20 based ARM device (see https://github.com/tjohann/a20_sdk). On usecase of the A20 devices is my Bananapi (CAN) Cluster named baalue. For that I need some control and admin paths to the cluster as hole and to a specific node.


UDS (unix domain socket server/client):
---------------------------------------

This examples show how to use a unix domain socket to configure a threads in an unrelated process. The predefined thread baa_schedule_server_th does the "magic". It controls via the protocol typ PTYPE_SCHED_PROPS the policy (SCHED_FIFO/...), the cpu affinity and the schedule priority for different threads.

To set the properties of a thread the server must be started with root rights, but it drops all except CAP_SYS_NICE.

	uds_server -> create a uds server and wait for kdo's
	              sudo ./time_triggert_udp_server

	uds_client -> send some kdo to the server and plot the output to stdout
	              ./time_triggert_uds -d /tmp -f lt-time_triggert_uds_server.socket


UDP (inet datagram socket server/client):
-----------------------------------------

	the same like the uds example but here via UDP
	see also baalued (https://github.com/tjohann/baalued)

	inet_daytime_client -> simple daytime client
	inet_daytime_server -> simple daytime server (not a daemon)


CAN (can sender/receiver):
--------------------------

	can_send -> send cyclic some example can telegrams on VCAN0
	can_recv -> receive the telegrams of VCAN0 and plot them to stdout
	see also my can/lin playground (https://github.com/tjohann/can_lin_env)


Time-Triggert (unix domain socket server/client):
-------------------------------------------------

	time_triggert_uds_server -> create a uds server to configure unrelated fiber
	time_triggert_uds -> an time-triggert process which set the parameter via uds server
	see also my time-triggert playground (https://github.com/tjohann/time_triggert_env)


Time-Triggert (inet datagram socket server/client):
---------------------------------------------------

	the same like the uds example but here via UDP


Time-Triggert (simple):
-----------------------

This examples show how to use of my simple time triggert infrastructure. To set the properties of the thread the program must be started with root rights, but it drops all except CAP_SYS_NICE.

	time_triggert_simple -> create and direct run a schedule table
