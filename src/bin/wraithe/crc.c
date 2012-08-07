/*************************************************************************
 * wraithe/src/bin/wraithe/crc.c
 *
 * Copyright (c) 2012 Michael D Henderson, all rights reserved
 *
 * This file is part of wraithe (http://github.com/quoha/wraithe).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************/
#include "local.h"

#include <stdio.h>

/*************************************************************************
 * Uses four common hash functions that don’t tend to collide to generate
 * a super hash value. Not the greatest thing in the world, but good enough
 * for non-critical usage.
 *
 * src buffer to calculate combined hash against
 * len number of bytes to hash
 * crc buffer should be one byte more than the number of bits in an int
 *       eg, 9/17/33/65 bytes for 8/16/32/64 bit integers
 */
const char *CRC(const unsigned char *src, int srcLen, char *crc, int crcLen) {
	const unsigned char *eoSrc = src + ((srcLen < 0) ? 0 : srcLen);

	unsigned int djbHash = 0;
	unsigned int fvnHash = 2166136261;
	unsigned int oatHash = 0;
	unsigned int saxHash = 0;

	while (src != eoSrc) {
		djbHash  = (djbHash *       33) + (*src);
		fvnHash  = (fvnHash * 16777619) ^ (*src);
		oatHash += (*src);
		oatHash += (oatHash << 10);
		oatHash ^= (oatHash >>  6);
		saxHash ^= (saxHash <<  5) + (saxHash >>  2) + (*src);

		src++;
	}

	oatHash += (oatHash <<  3);
	oatHash ^= (oatHash >> 11);
	oatHash += (oatHash << 15);

	if (crcLen > 0) {
		const char *fmt = "";
		switch (sizeof(int)) {
		case  1: /*   8 bit */ fmt = "%02x%02x%02x%02x"; break;
		case  2: /*  16 bit */ fmt = "%04x%04x%04x%04x"; break;
		case  4: /*  32 bit */ fmt = "%08x%08x%08x%08x"; break;
		case  8: /*  64 bit */ fmt = "%016x%016x%016x%016x"; break;
		case 16: /* 128 bit */ fmt = "%032x%032x%032x%032x"; break;
		}
		snprintf(crc, crcLen, fmt, djbHash, fvnHash, oatHash, saxHash);
		crc[crcLen - 1] = 0;
	}

	return crc;
}
