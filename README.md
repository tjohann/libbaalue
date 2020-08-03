Libbaalue (Linux)
=================

A base library for my different projects like sdk_builder (https://github.com/tjohann/sdk_builder) or baalued (https://github.com/tjohann/baalued.git). In this library all useful code from my different playgrounds like https://github.com/tjohann/time_triggert_env.git or https://github.com/tjohann/can_lin_env.git are migrated to (and will in the future).

WARNING: This is work in progress! Don't expect things to be complete in any dimension.


Support FreeBSD and OpenBSD:
----------------------------

The acual version works for void-linux and slackware, but not for FreeBSD and OpenBSD.

State: started


Overview
--------

The prefix for all functions and datatypes is "baa". See src/libbaalue.h for more detailed informations. All functions represent some parts i want to encapsulate to make the usage of a technologie easier. Topics like setup a datagram based unix domain socket server are provided by the library. Everthing is grouped in files which represent a certain topic, only helper.c is a summary of unspecified parts.

For common task (like set scheduling of fiber -> baa_schedule_server_th) threads are provided (see the corresponding file -> fiber.c). Also for device managment of a baalue node corresponding threads are available.

Below the folder examples you find simple programs which show the usage of the functions.

For different topics standard threads with simple protocols are implemented (PTYPE_*):

	/*
	* supported protocol type:
	*
	* *****************************************************************************
	*
	* - PTYPE_SCHED_PROPS -> set scheduling properties (via unix domain):
	*	num_unpacked = baa_unpack(buf, "cLLLL",
	*				  &protocol_type,  <- TODO: use this info?
	*				  &fiber_element.kernel_tid,
	*				  &fiber_element.policy,
	*				  &fiber_element.cpu,
	*				  &fiber_element.sched_param.sched_priority);
	*
	* client:                            server:
	* |__ send one fiber-element __|
	*                                    |__ unpack(see above)  __|
	*                                    |__ send PTYPE_RCV_ACK __|
	* |__ recv PTYPE_RCV_ACK     __|
	*                                    |__ set properties     __|
	*                                    |__ send PTYPE_CMD_ACK __|
	* |__ recv PTYPE_CMD_ACK     __|
	*
	* *****************************************************************************
	*
	* - PTYPE_DEVICE_MGMT -> start device managment functions:
	* client:                            server:
	*      - t.b.d.
	*
	* *****************************************************************************
	*
	* - PTYPE_DEVICE_MGMT_HALT -> independed subtype of device managment
	*
	* client:                            server:
	* |__ send PTYPE_...         __|
	*                                    |__ send PTYPE_RCV_ACK __|
	*                                    |__ execv('halt')      __|
	*
	* *****************************************************************************
	*
	* - PTYPE_DEVICE_MGMT_REBOOT -> independed subtype of device managment
	*
	* client:                            server:
	* |__ send PTYPE_...         __|
	*                                    |__ send PTYPE_RCV_ACK __|
	*                                    |__ execv('reboot')    __|
	*
	* *****************************************************************************
	*
	* - PTYPE_DEVICE_MGMT_PING -> independed subtype of device managment
	*
	* client:                            server:
	* |__ send PTYPE_...         __|
	*                                    |__ send PTYPE_RCV_ACK __|
	*
	* *****************************************************************************
	*/

	... see libbaalue.h for more info


error.c
-------

Common error handling functions:

	/*
	* +-------------------+------------+------------+--------------------------+
	* |     function      | use errno? | terminate? | log_level (man 3 syslog) |
	* +-------------------+------------+------------+--------------------------+
	* | baa_error_exit    |     yes    |   exit()   |         LOG_ERR          |
	* | baa_info_exit     |     no     |   exit()   |         LOG_INFOx         |
	* | baa_dump_exit     |     yes    |  abort()   |         LOG_ERR          |
	* | baa_error_msg     |     no     |    no      |         LOG_ERR          |
	* | baa_errno_msg     |     yes    |    no      |         LOG_ERR          |
	* | baa_info_msg      |     no     |    no      |         LOG_INFO         |
	* | baa_debug_msg     |     yes    |    no      |         LOG_DEBUG        |
	* +-------------------+------------+------------+--------------------------+
	* | baa_th_error_msg  | errno_val  |    no      |         LOG_ERR          |
	* | baa_th_error_exit | errno_val  |   exit()   |         LOG_ERR          |
	* | baa_th_dump_exit  | errno_val  |  abort()   |         LOG_ERR          |
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
	baa_signal(int signo, sigfunc *func);

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
	baa_inet_dgram_server(const char *service);

	int
	baa_inet_stream_server(const char *service);

	void *
	baa_daytime_server_th(void *args);

Note: if you wont split the unix domain socket name into dir (/var/run) and file (my_daemon) you can use name which is then the absolute name (/var/run/my_daemon).


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

	int
	baa_can_raw_socket(char *ifname, struct sockaddr_can **addr, unsigned char flags);

	int
	baa_can_bcm_socket(char *ifname, struct sockaddr_can **addr, unsigned char flags);

	int
	baa_set_hw_error_mask(int fd_s);

	int
	baa_set_flist(int fds, char *flist);


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

	void
	baa_reboot(void);

	void
	baa_halt(void);

	int
	baa_reboot_device(int sfd);

	int
	baa_halt_device(int sfd);

	int
	baa_ping_device(int sfd);


user.c
------

Functions to get user related informations (struct passwd).

	int
	baa_get_username(uid_t uid, char *buf, size_t size);

	uid_t
	baa_get_userid(const char *name);

	gid_t
	baa_get_groupid_of_user(const char *name);

	int
	baa_get_homedir(uid_t uid, char *buf, size_t size);

	int
	baa_get_homeinfo(uid_t uid, char *buf, size_t size);


group.c
-------

Functions to get group related informations (struct group).

	int
	baa_get_groupname(gid_t gid, char *buf, size_t size);

	gid_t
	baa_get_groupid(const char *name);


datatypes.c
-----------

Some basic datatypes like linked-list, queue or btree and access functions.


Provided examples (folder ./examples)
-------------------------------------

Below the folder examples you find some usecases of libbaalue. Most server are implemented as a daemon and therefore use syslog. In most cases everthing is tested on different A20 based ARM device (see https://github.com/tjohann/a20_sdk).

One usecase of the A20 devices is my Bananapi (CAN) Cluster named baalue. For that I need some control and admin paths to the cluster as hole and to a specific node.


Inet stream socket server/client (inet_daytime_*.c):
----------------------------------------------------

Simple daytime server (server thread: baa_daytime_server_th) and client to demonstrate TCP usage.

	inet_daytime_client -> simple daytime client
	inet_daytime_server -> simple daytime server (not a daemon)

State: finished


CAN sender/receiver (can_send.c && can_recv.c):
-----------------------------------------------

	can_send -> send cyclic some example can telegrams on VCAN0
	can_recv -> receive the telegrams of VCAN0 and plot them to stdout
	see also my can/lin playground (https://github.com/tjohann/can_lin_env)

State: started


Time-Triggert system (time_triggert_simple.c):
----------------------------------------------

This examples show how to use of my simple time triggert infrastructure. To set the properties of the thread the program must be started with root rights, but it drops all except CAP_SYS_NICE.

	time_triggert_simple -> create and direct run a schedule table

State: finished


Time-Triggert system controlled via unix domain socket (time_triggert_uds*.c):
------------------------------------------------------------------------------

This examples shows how to use a unix domain socket to configure a threads in an unrelated process. The predefined thread baa_schedule_server_th does the "magic". It controls via the protocol typ PTYPE_SCHED_PROPS the policy (SCHED_FIFO/...), the cpu affinity and the schedule priority for different threads.

See also my time-triggert playground (https://github.com/tjohann/time_triggert_env)

To set the properties of a thread the server must be started with root rights, but it drops all except CAP_SYS_NICE.

	uds_server -> create a uds server and wait for kdo's
	              sudo ./time_triggert_uds_server

	uds_client -> send some kdo to the server and plot the output to stdout
	              ./time_triggert_uds -d /tmp -f lt-time_triggert_uds_server.socket

State: started


Remote device managment via unix datagram socket (udp_mgmt_*.c):
----------------------------------------------------------------

These examples show how to use a unix datagram socket for remote device managment like halt a baalue node (halt/reboot/.../ping). The server is implemented within the thread with baa_device_mgmt_th.

	udp_mgmt_server.c -> the server (a baalue node)

	udp_mgmt_ping_client.c -> a simple client which pings a baalue node
	udp_mgmt_reboot_client.c -> a simple client which reboots a baalue node

State: restarted for next examples/functions (finished)


Plot status info to an I2C-LCD1602 display (lcd1602.c):
-------------------------------------------------------

This example shows how to use a simple lcd as an state monitor (see also https://github.com/tjohann/baalued.git).

	lcd1602.c -> cycle some state info on the display

State: not started


Show some user and group related infos (user_group_info.c):
-----------------------------------------------------------

This example shows how to use the simple functions for user/group topics.

	user_group_info.c -> shows some infos about the your user account based on struct passwd and struct group

State: finished
