/*
  simple_d.c     demo program for simple two-pass coder - decompression

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
  GNU General Public License for more details.  It may be that this
  program violates local patents in your country, however it is
  belived (NO WARRANTY!) to be patent-free here in Austria.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA 02111-1307, USA.

  simple_d.c is an example decompressor able to decompress the files
  produced by simple_c.c

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

void usage()
{   fprintf(stderr,"simple_d [inputfile [outputfile]]\n");
    fprintf(stderr,"simple_d (c)1999 Michael Schindler, michael@compressconsult.com\n"); 
    exit(1);
}


void readcounts(rangecoder *rc, freq *counters)
{   int i;
    for (i=0; i<256; i++)
        counters[i] = decode_short(rc);
}

int main( int argc, char *argv[] )
{   freq cf;
    rangecoder rc;

    if (argc < 3)  usage();

	freopen( argv[1], "rb", stdin );
	freopen( argv[2], "wb", stdout );

#ifndef unix
    setmode( fileno( stdin ), O_BINARY );
    setmode( fileno( stdout ), O_BINARY );
#endif

    if (start_decoding(&rc) != 0)
    {   fprintf(stderr,"could not suceessfully open input data\n");
        exit(1);
    }

    while (cf = decode_culfreq(&rc,2))
    {   freq counts[257], i, blocksize;
        
        decode_update	(&rc,1,1,2);
        readcounts		(&rc,counts);
        
        /* figure out blocksize by summing counts; also use counta as in encoder */
        blocksize = 0;
        for (i=0; i<256; i++)
        {   freq tmp = counts[i];
            counts[i] = blocksize;
            blocksize += tmp;
        }
        counts[256] = blocksize;

        for (i=0; i<blocksize; i++)
        {   freq cf, symbol;
            cf = decode_culfreq(&rc,blocksize);
            /* figure out symbol inefficiently; a binary search would be much better */
            for (symbol=0; counts[symbol+1]<=cf; symbol++)
                /* void */;
            decode_update(&rc, counts[symbol+1]-counts[symbol],counts[symbol],blocksize);
            putchar(symbol);
        }
    }

    done_decoding(&rc);

    return 0;
}
