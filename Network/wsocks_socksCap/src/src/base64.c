/*
 *  Copyright (C) 2006- xiaosuo <xiaosuo@gmail.com> 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "base64.h"

/* the relative RFC: RFC 3548 */
const char* base64_encode(const char *str, int length, int *retlen)
{  
	static const char base64_table[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/',
	};
	int str_enc_len, i, length_mod;
	char *str_enc;
	unsigned char *ptr, *pptr;

	length = (length < 0 ? strlen(str) : length);

	length_mod = length % 3;
	length /= 3;
	str_enc_len = length * 4;
	str_enc_len += ((length_mod == 0) ? 0 : 4);
	if(retlen) *retlen = str_enc_len;

	str_enc = (char*)malloc(str_enc_len + 1);
	if(!str_enc){
		return NULL;
	}

	ptr = (unsigned char*)str;
	pptr = (unsigned char*)str_enc;
	for(i = 0; i < length; i ++){
		*pptr++ = base64_table[(ptr[0] >> 2)];
		*pptr++ = base64_table[((ptr[0] & 0x3) << 4) + (ptr[1] >> 4)];
		*pptr++ = base64_table[((ptr[1] & 0xf) << 2) + (ptr[2] >> 6)];
		*pptr++ = base64_table[(ptr[2] & 0x3f)];
		ptr += 3;
	}

	if(length_mod == 1){
		*pptr++ = base64_table[(ptr[0] >> 2)];
		*pptr++ = base64_table[((ptr[0] & 0x3) << 4)];
		*pptr++ = '=';
		*pptr++ = '=';
	}else if(length_mod == 2){
		*pptr++ = base64_table[(ptr[0] >> 2)];
		*pptr++ = base64_table[((ptr[0] & 0x3) << 4) + (ptr[1] >> 4)];
		*pptr++ = base64_table[((ptr[1] & 0xf) << 2)];
		*pptr++ = '=';
	}

	*pptr = '\0';

	return str_enc;
}

static inline unsigned char base64_dechar(char ch)
{
	if(ch >= 'A' && ch <= 'Z'){
		return ch - 65;
	}else if(ch >= 'a' && ch <= 'z'){
		return ch - 71;
	}else if(ch >= '0' && ch <= '9'){
		return ch + 4;
	}else if(ch == '+'){
		return 62;
	}else if(ch == '/'){
		return 63;
	}else if(ch == '='){
		return 0;
	}else{ /* overflow char */
		return 0xff;
	}
}

static inline int base64_423(const char *src, unsigned char *dst)
{
	unsigned char tmp;

	tmp = base64_dechar(src[0]);
	if(tmp == 0xff || src[0] == '=') return -1;
	dst[0] = tmp << 2;
	tmp = base64_dechar(src[1]);
	dst[0] |= (tmp & 0x30) >> 4;
	if(tmp == 0xff || src[1] == '=') return -1;
	dst[1] = (tmp & 0x0f) << 4;
	tmp = base64_dechar(src[2]);
	dst[1] |= (tmp & 0x3C) >> 2;
	if(tmp == 0xff){
		return -1;
	}else if(src[2] == '=' && src[3] != '='){
		return -1;
	}
	dst[2] = ((tmp & 0x03) << 6) | base64_dechar(src[3]);

	return 3 - (src[3] == '=') - (src[2] == '=');
}

const char* base64_decode(const char *str, int length, int *retlen)
{
	int len, i, ret, reslen;
	unsigned char *retval, *pret;
	const char *pstr;

	len = length > 0 ? length : strlen(str);
	if(len % 4 != 0) return NULL;
	retval = malloc(len * 3 / 4 + 1);
	if(!retval){
		errno = -ENOMEM;
		return NULL;
	}
	for(pstr = str, pret = retval, i = 0, reslen = 0;
			i < len; pstr += 4, pret += 3, i += 4, reslen += 3){
		ret = base64_423(pstr, pret);
		if(ret < 0){ /* Not a base64 encoded string */
			free(retval);
			return NULL;
		}else if(ret != 3){ /* end or error */
			reslen += ret;
			break;
		}
	}
	if(i != len && i != len - 4){
		free(retval);
		return NULL;
	}
	retval[reslen] = '\0';
	if(retlen){
		*retlen = reslen;
	}
	
	return (const char*)retval;
}
