/*
  simple_c.c     demo program for simple two-pass coder - compression

  (c) Michael Schindler
  1999
  http://www.compressconsult.com/
  michael@compressconsult.com

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA 02111-1307, USA.

  simple_c is an example compressor trying to compress files with
  a simple order 0 model. The files can be decompressed by simple_d.

  Note that I do not think that a model as here is good;
  For better compression see for example my freeware szip.
  http://www.compressconsult.com/szip/
  or ask me as consultant what compression method fits your data best.
*/

#include <stdio.h>
#include <stdlib.h>
#ifndef unix
#include <io.h>
#include <fcntl.h>
#endif
#include <ctype.h>
#include "port.h"
#include "rangecod.h"

/* keep the blocksize below 1<<16 or you'll see overflows */
#define BLOCKSIZE 60000

void usage()
{   printf("simple_c [inputfile [outputfile]]\n");
    printf("simple_c (c)1999 Michael Schindler, michael@compressconsult.com\n"); 
    exit(1);
}


/* count number of occurances of each byte */
void countblock(unsigned char *buffer, freq length, freq *counters)
{   unsigned int i;
    /* first zero the counters */
    for (i=0; i<257; i++)
        counters[i] = 0;
    /* then count the number of uccurances of each byte */
    for (i=0; i<length; i++)
        counters[buffer[i]]++;
}


int maaain( int argc, char *argv[] )
{   
	freq counts[257], blocksize;
    rangecoder rc;
    unsigned char buffer[BLOCKSIZE];

    if (argc < 3) usage();

    freopen( argv[1], "rb", stdin	);
    freopen( argv[2], "wb", stdout	);

#ifndef unix
    setmode( fileno( stdin ),	O_BINARY );
    setmode( fileno( stdout ),	O_BINARY );
#endif

    /* initialize the range coder, first byte 0, no header */
    start_encoding(&rc,0,0);

    while (1)
    {   freq i;
        /* get the statistics */
        blocksize = fread(buffer,1,(size_t)BLOCKSIZE,stdin);
        
        /* terminate if no more data */
        if (blocksize==0) break;
        
        encode_freq(&rc,1,1,2); /* a stupid way to code a bit */

        countblock(buffer,blocksize,counts);

        /* write the statistics. */
        /* Cant use putchar or other since we are after start of the rangecoder */
        /* as you can see the rangecoder doesn't care where probabilities come */
        /* from, it uses a flat distribution of 0..0xffff in encode_short. */
        for(i=0; i<256; i++)
            encode_short(&rc,counts[i]);


        /* store in counters[i] the number of all bytes < i, so sum up */
        counts[256] = blocksize;
        for (i=256; i; i--)
            counts[i-1] = counts[i]-counts[i-1];

        /* output the encoded symbols */
        for(i=0; i<blocksize; i++) {
            int ch = buffer[i];
            encode_freq(&rc,counts[ch+1]-counts[ch],counts[ch],counts[256]);
        }
    }

    /* flag absence of next block by a bit */
    encode_freq(&rc,1,0,2);

    /* close the encoder */
    done_encoding(&rc);

    return 0;
}
