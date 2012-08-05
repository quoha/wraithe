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
 */
const char *CRC(const char *src) {
	static char crc[128];

	unsigned int djbHash = 0;
	unsigned int fvnHash = 2166136261;
	unsigned int oatHash = 0;
	unsigned int rotHash = 0;
	unsigned int saxHash = 0;

	const unsigned char *p = (const unsigned char *)src;
	while (*p) {
		djbHash  = (djbHash *       33) + (*p);
		fvnHash  = (fvnHash * 16777619) ^ (*p);
		rotHash  = (rotHash << 4) ^ (rotHash >> 28) ^ (*p);
		saxHash ^= (saxHash << 5) + (saxHash >>  2) + (*p);

		oatHash += (*p);
		oatHash += (oatHash << 10);
		oatHash ^= (oatHash >>  6);

		p++;
	}

	oatHash += (oatHash <<  3);
	oatHash ^= (oatHash >> 11);
	oatHash += (oatHash << 15);

	sprintf(crc, "%08x-%08x-%08x-%08x", djbHash, fvnHash, oatHash, saxHash);

	return crc;
}
