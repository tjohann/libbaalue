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

#include "libbaalue.h"


static void
pack_16(unsigned char *buf, unsigned int i)
{
	*buf++ = i >> 8;
	*buf++ = i;
}

static void
pack_32(unsigned char *buf, unsigned long int i)
{
	*buf++ = i >> 24;
	*buf++ = i >> 16;
	*buf++ = i >> 8;
	*buf++ = i;
}

static signed char
unpack_i8(unsigned char *buf)
{
	unsigned int tmp = (unsigned int) buf[0];
	int i;

	/*
	 * really tricky stuff ... see www for more info
	 */
	if (tmp <= 0x7fu)
		i = tmp;
	else
		i = (-1) - (unsigned int) (0xffu - tmp);

	return i;
}

static signed int
unpack_i16(unsigned char *buf)
{
	unsigned int tmp = ((unsigned int) buf[0] << 8) | buf[1];
	int i;

	/*
	 * really tricky stuff ... see www for more info
	 */
	if (tmp <= 0x7fffu)
		i = tmp;
	else
		i = (-1) - (unsigned int) (0xffffu - tmp);

	return i;
}

static signed long int
unpack_i32(unsigned char *buf)
{
	unsigned long int tmp = ((unsigned long int) buf[0] << 24) |
		((unsigned long int) buf[1] << 16) |
		((unsigned long int) buf[2] << 8)  |
		buf[3];
	long int i;

	/*
	 * really tricky stuff ... see www for more info
	 */
	if (tmp <= 0x7fffffffu)
		i = tmp;
	else
		i = (-1) - (unsigned long int) (0xffffffffu - tmp);

	return i;
}

static unsigned int
unpack_16(unsigned char *buf)
{
	return ((unsigned int) buf[0] << 8) | buf[1];
}

static unsigned long int
unpack_32(unsigned char *buf)
{
	return ((unsigned long int) buf[0] << 24) |
	       ((unsigned long int) buf[1] << 16) |
	       ((unsigned long int) buf[2] << 8)  |
	       buf[3];
}

int
baa_pack(unsigned char *buf, char *fmt, ...)
{
	unsigned char c;
	unsigned short s;
	unsigned long int l;

	signed char C;
	signed short S;
	signed long int L;

	size_t size = 0;

	va_list ap;
	va_start(ap, fmt);

	for(; *fmt != '\0'; fmt++) {
		switch(*fmt) {
		case 'c':
			size += 1;
			c = (unsigned char) va_arg(ap, unsigned int);
			*buf++ = c;
			break;
		case 'C':
			size += 1;
			C = (signed char) va_arg(ap, signed int);
			*buf++ = C;
			break;
		case 's':
			size += 2;
			s = va_arg(ap, unsigned int);
			pack_16(buf, s);
			buf += 2;
			break;
		case 'S':
			size += 2;
			S = va_arg(ap, signed int);
			pack_16(buf, S);
			buf += 2;
			break;
		case 'l':
			size += 4;
			l = va_arg(ap, unsigned long int);
			pack_32(buf, l);
			buf += 4;
			break;
		case 'L':
			size += 4;
			L = va_arg(ap, signed long int);
			pack_32(buf, L);
			buf += 4;
			break;
		default:
			baa_error_msg(_("unknown format %c"), fmt);
		}
	}

	va_end(ap);

	return size;
}

int
baa_unpack(unsigned char *buf, char *fmt, ...)
{
	unsigned char *c;
	unsigned short *s;
	unsigned long int *l;

	signed char *C;
	signed short *S;
	signed long int *L;

	size_t size = 0;

	va_list ap;
	va_start(ap, fmt);

	for(; *fmt != '\0'; fmt++) {
		switch (*fmt) {
		case 'c':
			size += 1;
			c = va_arg(ap, unsigned char*);
			*c = *buf++;
			break;
		case 'C':
			size += 1;
			C = va_arg(ap, signed char*);
			*C = unpack_i8(buf);
			buf += 1;
			break;
		case 's':
			size += 2;
			s = va_arg(ap, unsigned short*);
			*s = unpack_16(buf);
			buf += 2;
			break;
		case 'S':
			size += 2;
			S = va_arg(ap, signed short*);
			*S = unpack_i16(buf);
			buf += 2;
			break;
		case 'l':
			size += 4;
			l = va_arg(ap, unsigned long int*);
			*l = unpack_32(buf);
			buf += 4;
			break;
		case 'L':
			size += 4;
			L = va_arg(ap, signed long int*);
			*L = unpack_i32(buf);
			buf += 4;
			break;
		default:
			baa_error_msg(_("unknown format %c"), fmt);
		}
	}

	va_end(ap);

	return size;
}
