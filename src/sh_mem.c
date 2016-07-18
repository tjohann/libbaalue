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

static int
shmem_handle(char *name, size_t size,
	     mode_t access_mode, mode_t user_mode,
	     void **mmap_seg)
{
	if (name == NULL)
		baa_info_msg(_("name for shared memory segment is NULL"));

	int fd = -1;
open_cmd:
	fd = shm_open(name, access_mode, user_mode);
	if (fd == -1 && errno == EINTR) {
		goto open_cmd;
	} else {
		if (errno == EEXIST) {
			baa_error_msg(_("shared memory segmente %s already exist"),
				  name);
			return EEXIST;
		} else {
			baa_error_exit(_("could not open %s"), name);
		}
	}

	int ret = -1;
ftruncate_cmd:
	ret = ftruncate(fd, size);
	if (ret == -1 && errno == EINTR)
		goto ftruncate_cmd;
	else
		baa_error_exit(_("could not truncate shmem segment %s"), name);

	*mmap_seg = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (*mmap_seg == MAP_FAILED)
		baa_error_exit(_("could not truncate shmem segment %s"), name);

	return fd;
}

BAALUE_EXPORT int
baa_shmem_server(char *name, size_t size, void **mmap_seg)
{
	mode_t access_mode = O_RDWR | O_CREAT | O_EXCL;
	mode_t user_mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH;

	return shmem_handle(name, size, access_mode, user_mode, mmap_seg);
}

BAALUE_EXPORT int
baa_shmem_client(char *name, size_t size, void **mmap_seg)
{
	mode_t access_mode = O_RDWR;
	mode_t user_mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH;

	return shmem_handle(name, size, access_mode, user_mode, mmap_seg);
}

BAALUE_EXPORT void
baa_unlink_mmap_seg(char *name)
{
	if (shm_unlink(name) == -1)
		baa_error_msg(_("could not unlink %"), name);
}
