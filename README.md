Libbaalue
=========

My basic lib for my different projects like sdk_builder(https://github.com/tjohann/sdk_builder) or baalued(https://github.com/tjohann/baalued.git).

WARNING: This is work in progress! Don't expect things to be complete in any dimension.


Overview
--------

The prefix for all functions and datatypes is "baa". See src/libbaalue.h for more detailed informations. All functions represent some parts i want to encapsulate to make the usage of a technologie easier. Topics like setup a datagram based unix domain socket server are provided by the library. Everthing is grouped in files which represent a certain topic, only helper.c is a summary of unspecified parts.

Below the examples folder you found programs which show the usage of the functions.


error.c
-------

Common error handling functions.


helper.c
--------

Some general functions to become a daemon or posix semaphore handling.


network.c
---------

Network related topics like setup a unix domain socket server.


fiber.c
-------

Functions to set properties of a thread/process like scheduling policies or cpu affinity based on a datastructure to easily setup something like a time-triggert process environment (http://www.cs.utexas.edu/~mok/cs378/Spring16/Reading/KopetzTTvsET_1991.pdf and http://www.safetty.net/publications/pttes).


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


process.c
---------

Functions to handle special process topics like priority and cpu affinity.