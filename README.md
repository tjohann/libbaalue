Libbaalue
=========

My basic lib for my different projects like sdk_builder (https://github.com/tjohann/sdk_builder) or baalued (https://github.com/tjohann/baalued.git). This is something like a migration path from my different playgrounds like https://github.com/tjohann/time_triggert_env.git or https://github.com/tjohann/can_lin_env.git .

WARNING: This is work in progress! Don't expect things to be complete in any dimension.


Overview
--------

The prefix for all functions and datatypes is "baa". See src/libbaalue.h for more detailed informations. All functions represent some parts i want to encapsulate to make the usage of a technologie easier. Topics like setup a datagram based unix domain socket server are provided by the library. Everthing is grouped in files which represent a certain topic, only helper.c is a summary of unspecified parts.

Below the examples folder you found simple programs which show the usage of the functions (see also below for more info).


error.c
-------

Common error handling functions.


helper.c
--------

Some general functions to become a daemon or posix semaphore handling.


network.c
---------

Network related topics like setup a unix domain socket server/clients or inet socket server/clients.


fiber.c & process.c
-------------------

Functions to setup a thread/process like scheduling policies or cpu affinity based on a datastructure to easily setup something like a time-triggert process environment (http://www.cs.utexas.edu/~mok/cs378/Spring16/Reading/KopetzTTvsET_1991.pdf and http://www.safetty.net/publications/pttes).


sh_mem.c
--------

Functions to handle posix shared memory.


sh_mq.c
-------

Functions to handle posix message queues.


serialize.c
-----------

Simple functions to pack/unpack in a printf like style (https://en.wikipedia.org/wiki/The_Practice_of_Programming).


wrapper.c
---------

Some wrapper functions around some linux/unix syscalls.


can_lin.c
---------

Functions to handle CAN and/or LIN communication.


process.c (in addtion to fiber.c)
---------------------------------

Functions to handle special process topics like priority and cpu affinity.


Provided examples
-----------------

Below the folder examples you find some usecases of libbaalue. All server are implemented as daemon and therefore use syslog. In most cases everthing is tested on different A20 based ARM device (see https://github.com/tjohann/a20_sdk). On usecase of the A20 devices is my Bananapi (CAN) Cluster named baalue. For that I need some control and admin paths to the cluster as hole and to a specific node.

UDS (unix domain socket server/client):

This examples show how to use a unix domain socket to configure a threads in an unrelated process. The predefined thread baa_schedule_server_th does the "magic". It controlls via the protocol typ PTYPE_SCHED_PROPS the policy (SCHED_FIFO/...), the cpu affinity and the schedule priority for a kernel-tid.

	uds_server -> create a uds server and wait for kdo's
	              ./time_triggert_udp_server
	uds_client -> send some kdo to the server and plot the output to stdout
	              ./time_triggert_uds -d /var/tmp -f lt-time_triggert_uds_server.socket


UDP (inet datagram socket server/client):

	the same like the uds example but here via UDP
	see also baalued (https://github.com/tjohann/baalued)


CAN (can sender/receiver):

	can_send -> send cyclic some example can telegrams on VCAN0
	can_recv -> receive the telegrams of VCAN0 and plot them to stdout
	see also my can/lin playground (https://github.com/tjohann/can_lin_env)


Time-Triggert (unix domain socket server/client)

	time_triggert_uds_server -> create a uds server to configure unrelated fiber
	time_triggert_uds -> an time-triggert process which set the parameter via uds server
	see also my time-triggert playground (https://github.com/tjohann/time_triggert_env)


Time-Triggert (inet datagram socket server/client)

	the same like the uds example but here via UDP



TO_REMOVE:
http://www.thomasstover.com/uds.html
http://man7.org/tlpi/code/online/book/sockets/scm_cred_recv.c.html
http://man7.org/tlpi/code/online/book/sockets/scm_cred_send.c.html
