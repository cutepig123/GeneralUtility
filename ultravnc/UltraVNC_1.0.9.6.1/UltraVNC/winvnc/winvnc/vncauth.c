//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


/*
 *  Functions for VNC password management and authentication.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>
#include "vncauth.h"
#include "d3des.h"

/*
 *   We use a fixed key to store passwords, since we assume that our local
 *   file system is secure but nonetheless don't want to store passwords
 *   as plaintext.
 */

unsigned char fixedkey[8] = {23,82,107,6,35,78,88,7};

/*
 *   Encrypt a password and store it in a file.
 */
int
vncEncryptPasswd(char *passwd, char *encryptedPasswd)
{
    size_t i;

    /* pad password with nulls */

    for (i = 0; i < MAXPWLEN; i++) {
	if (i < strlen(passwd)) {
	    encryptedPasswd[i] = passwd[i];
	} else {
	    encryptedPasswd[i] = 0;
	}
    }

    /* Do encryption in-place - this way we overwrite our copy of the plaintext
       password */

    deskey(fixedkey, EN0);
    des(encryptedPasswd, encryptedPasswd);

    return 8;
}

/*
 *   Decrypt a password.  Returns a pointer to a newly allocated
 *   string containing the password or a null pointer if the password could
 *   not be retrieved for some reason.
 */
char *
vncDecryptPasswd(char *inouttext)
{
    unsigned char *passwd = (unsigned char *)malloc(9);

    deskey(fixedkey, DE1);
    des(inouttext, passwd);

    passwd[8] = 0;

    return (char *)passwd;
}

char *
vncDecryptPasswdMs(char *inouttext) 
{ 
        // Marscha@2004: corrected bug which occured for some passwords. 
        // See http://forum.ultravnc.net/viewtopic.php?t=803 
    unsigned char *passwd = (unsigned char *)malloc(33); 
        int i; 
        for (i = 0; i < MAXMSPWLEN; i++) 
                passwd[i] = inouttext[i]; 

        //des seems only to handle the first 8 chars.. 
    deskey(fixedkey, DE1); 
    des(passwd, passwd); 
    passwd[32] = 0; 

        // Marscha@2004: Seems to be not necessary, but I found no 
        // documentation what des() guarantees beyond the 8th byte. 
        for (i=8; i < MAXMSPWLEN; i++) 
                passwd[i] = inouttext[i]; 

    return (char *)passwd; 
}

/*
 *   Generate a set of random bytes for use in challenge-response authentication.
 */
void
vncRandomBytes(unsigned char *where) {
  int i;
  static unsigned int seed;
  seed += (unsigned int) time(0) + _getpid() + _getpid() * 987654;

  srand(seed);
  for (i=0; i < CHALLENGESIZE; i++) {
    where[i] = (unsigned char)(rand() & 255);    
  }
}

void
vncRandomBytesMs(unsigned char *where) {
  int i;
  static unsigned int seed;
  seed += (unsigned int) time(0) + _getpid() + _getpid() * 987654;

  srand(seed);
  for (i=0; i < CHALLENGESIZEMS; i++) {
    where[i] = (unsigned char)(rand() & 255);    
  }
}

/*
 *   Encrypt some bytes in memory using a password.
 */
void
vncEncryptBytes(unsigned char *where, const char *passwd)
{
    unsigned char key[8];
    size_t i;

    /* key is simply password padded with nulls */

    for (i = 0; i < 8; i++) {
	if (i < strlen(passwd)) {
	    key[i] = passwd[i];
	} else {
	    key[i] = 0;
	}
    }

    deskey(key, EN0);

    for (i = 0; i < CHALLENGESIZE; i += 8) {
	des(where+i, where+i);
    }
}

/*
 *   marscha@2006
 *   Decrypt bytes[length] in memory using key.
 *   Key has to be 8 bytes, length a multiple of 8 bytes.
 */
void
vncDecryptBytes(unsigned char *where, const int length, const unsigned char *key) {
	int i, j;
	deskey((unsigned char*) key, DE1);
	for (i = length - 8; i > 0; i -= 8) {
		des(where + i, where + i);
		for (j = 0; j < 8; j++)
			where[i + j] ^= where[i + j - 8];
	}
	/* i = 0 */
	des (where, where);
	for (i = 0; i < 8; i++)
		where[i] ^= key[i];
}
