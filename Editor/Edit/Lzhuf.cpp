// LzHuf.cpp : Defines the entry point for the console application.
//
#include "pch.h"
#pragma hdrstop

#define MODULE

//typedef unsigned char uchar;

unsigned	textsize = 0, codesize = 0;

char		wterr[] = "Can't write.";

void Error(char *message)
{
    Log.Msg("\n%s\n", message);
    exit(EXIT_FAILURE);
}

/********** LZSS compression **********/

#define N			4096    /* buffer size */
#define F			60		/* lookahead buffer size */
#define THRESHOLD   2
#define NIL			N		/* leaf of tree */

#define N_CHAR      (256 - THRESHOLD + F)	/* kinds of characters (character code = 0..N_CHAR-1) */
#define T			(N_CHAR * 2 - 1)		/* size of table */
#define R			(T - 1)					/* position of root */
#define MAX_FREQ    0x4000					/* updates tree when the */


uchar				text_buf[N + F];
int					match_position, match_length, lson[N + 1], rson[N + 257], dad[N + 1];
uchar				*data, *data_out;
unsigned			data_size=0,data_ptr=0,data_out_ptr=0,data_out_size=0;
unsigned			code, len;
unsigned			tim_size=0;

__forceinline int _getb(void) {
	if (data_ptr>=data_size) return EOF;
	return data[data_ptr++];
}
__forceinline void _putb(int c) {
	data_out[data_out_ptr++]=uchar(c);
	VERIFY(data_out_ptr<data_out_size);
}

void ResizeOutCache(unsigned sz)
{
	if (sz) {
		data_out = (uchar*)realloc(data_out,sz);
		data_out_size = sz;
	}
}

void InitTree(void)  /* initialize trees */
{
    int  i;

    for (i = N + 1; i <= N + 256; i++)	        rson[i] = NIL;        /* root */
    for (i = 0; i < N; i++)						dad[i] = NIL;         /* node */
}

void InsertNode(int r)  /* insert to tree */
{
    int				i, p, cmp;
    uchar			*key;
    unsigned		c;

    cmp = 1;
    key = &text_buf[r];
    p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;
    match_length = 0;
    for ( ; ; ) {
        if (cmp >= 0) {
            if (rson[p] != NIL)
                p = rson[p];
            else {
                rson[p] = r;
                dad[r] = p;
                return;
            }
        } else {
            if (lson[p] != NIL)
                p = lson[p];
            else {
                lson[p] = r;
                dad[r] = p;
                return;
            }
        }
        for (i = 1; i < F; i++)
            if ((cmp = key[i] - text_buf[p + i]) != 0)
                break;
        if (i > THRESHOLD) {
            if (i > match_length) {
                match_position = ((r - p) & (N - 1)) - 1;
                if ((match_length = i) >= F)
                    break;
            }
            if (i == match_length) {
                if ((c = ((r - p) & (N-1)) - 1) < (unsigned)match_position) {
                    match_position = c;
                }
            }
        }
    }
    dad[r]  = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;
    if (rson[dad[p]] == p)
        rson[dad[p]] = r;
    else
        lson[dad[p]] = r;
    dad[p] = NIL; /* remove p */
}

void DeleteNode(int p)  /* remove from tree */
{
    int  q;

    if (dad[p] == NIL)
        return;         /* not registered */
    if (rson[p] == NIL)
        q = lson[p];
    else
    if (lson[p] == NIL)
        q = rson[p];
    else {
        q = lson[p];
        if (rson[q] != NIL) {
            do {
                q = rson[q];
            } while (rson[q] != NIL);
            rson[dad[q]] = lson[q];
            dad[lson[q]] = dad[q];
            lson[q] = lson[p];
            dad[lson[p]] = q;
        }
        rson[q] = rson[p];
        dad[rson[p]] = q;
    }
    dad[q] = dad[p];
    if (rson[dad[p]] == p)
        rson[dad[p]] = q;
    else
        lson[dad[p]] = q;
    dad[p] = NIL;
}

/* Huffman coding */


/* table for encoding and decoding the upper 6 bits of position */
/* for encoding */
uchar p_len[64] = {
    0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
};

uchar p_code[64] = {
    0x00, 0x20, 0x30, 0x40, 0x50, 0x58, 0x60, 0x68,
    0x70, 0x78, 0x80, 0x88, 0x90, 0x94, 0x98, 0x9C,
    0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC,
    0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
    0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
    0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

/* for decoding */
uchar d_code[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
    0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
    0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
    0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
    0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
    0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
    0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
    0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
    0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

uchar d_len[256] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

unsigned freq[T + 1];	/* frequency table */

int prnt[T + N_CHAR+1];	/* pointers to parent nodes, except for the				*/
						/* elements [T..T + N_CHAR - 1] which are used to get	*/
						/* the positions of leaves corresponding to the codes.	*/

int son[T];				/* pointers to child nodes (son[], son[] + 1)			*/

unsigned	getbuf = 0;
unsigned	getlen = 0;

int GetBit(void)    /* get one bit */
{
    unsigned i;

    while (getlen <= 8) {
        if ((int)(i = _getb()) < 0) i = 0;
        getbuf |= i << (8 - getlen);
        getlen += 8;
    }
    i = getbuf;
    getbuf <<= 1;
    getlen--;
    return (int)((i & 0x8000) >> 15);
}

int GetByte(void)   /* get one byte */
{
    unsigned i;

    while (getlen <= 8) {
        if ((int)(i = _getb()) < 0) i = 0;
        getbuf |= i << (8 - getlen);
        getlen += 8;
    }
    i = getbuf;
    getbuf <<= 8;
    getlen -= 8;
    return (int)((i & 0xff00) >> 8);
}

unsigned putbuf = 0;
unsigned putlen = 0;

void Putcode(int l, unsigned c)     /* output c bits of code */
{
    putbuf |= c >> putlen;
    if ((putlen += l) >= 8) {
        _putb(putbuf >> 8);
        if ((putlen -= 8) >= 8) {
            _putb(putbuf);
            codesize += 2;
            putlen -= 8;
            putbuf = c << (l - putlen);
        } else {
            putbuf <<= 8;
            codesize++;
        }
    }
}


/* initialization of tree */

void StartHuff(void)
{
    int i, j;

    for (i = 0; i < N_CHAR; i++) {
        freq[i]		= 1;
        son	[i]		= i + T;
        prnt[i + T] = i;
    }
    i = 0; j = N_CHAR;
    while (j <= R) {
        freq[j]		= freq[i] + freq[i + 1];
        son[j]		= i;
        prnt[i]		= prnt[i + 1] = j;
        i += 2; j++;
    }
    freq[T] = 0xffff;
    prnt[R] = 0;
}


/* reconstruction of tree */

void reconst(void)
{
    int			i, j, k;
    unsigned	f, l;

    /* collect leaf nodes in the first half of the table */
    /* and replace the freq by (freq + 1) / 2. */
    j = 0;
    for (i = 0; i < T; i++) {
        if (son[i] >= T) {
            freq[j] = (freq[i] + 1) / 2;
            son[j] = son[i];
            j++;
        }
    }
    /* begin constructing tree by connecting sons */
    for (i = 0, j = N_CHAR; j < T; i += 2, j++) {
        k = i + 1;
        f = freq[j] = freq[i] + freq[k];
        for (k = j - 1; f < freq[k]; k--);
        k++;
        l = (j - k) * sizeof(unsigned);
        memmove(&freq[k + 1], &freq[k], l);
        freq[k] = f;
        memmove(&son[k + 1], &son[k], l);
        son[k] = i;
    }
    /* connect prnt */
    for (i = 0; i < T; i++) {
        if ((k = son[i]) >= T) {
            prnt[k] = i;
        } else {
            prnt[k] = prnt[k + 1] = i;
        }
    }
}


/* increment frequency of given code by one, and update tree */

void update(int c)
{
    int i, j, k, l;

    if (freq[R] == MAX_FREQ) {
        reconst();
    }
    c = prnt[c + T];
    do {
        k = ++freq[c];

        /* if the order is disturbed, exchange nodes */
        if ((unsigned)k > freq[l = c + 1]) {
            while ((unsigned)k > freq[++l]);
            l--;
            freq[c] = freq[l];
            freq[l] = k;

            i = son[c];
            prnt[i] = l;
            if (i < T) prnt[i + 1] = l;

            j = son[l];
            son[l] = i;

            prnt[j] = c;
            if (j < T) prnt[j + 1] = c;
            son[c] = j;

            c = l;
        }
    } while ((c = prnt[c]) != 0); /* repeat up to root */
}

void EncodeChar(unsigned c)
{
    unsigned i;
    int j, k;

    i = 0;
    j = 0;
    k = prnt[c + T];

    /* travel from leaf to root */
    do {
        i >>= 1;

        /* if node's address is odd-numbered, choose bigger brother node */
        if (k & 1) i += 0x8000;

        j++;
		k = prnt[k];
    } while (k != R);
    Putcode(j, i);
    code = i;
    len = j;
    update(c);
}

void EncodePosition(unsigned c)
{
    unsigned i;

    /* output upper 6 bits by table lookup */
    i = c >> 6;
    Putcode(p_len[i], (unsigned)p_code[i] << 8);

    /* output lower 6 bits verbatim */
    Putcode(6, (c & 0x3f) << 10);
}

void EncodeEnd(void)
{
    if (putlen) {
        _putb(putbuf >> 8);
        codesize++;
    }
}

int DecodeChar(void)
{
    unsigned c;

    c = son[R];

    /* travel from root to leaf, */
    /* choosing the smaller child node (son[]) if the read bit is 0, */
    /* the bigger (son[]+1} if 1 */
    while (c < T) {
        c += GetBit();
        c = son[c];
    }
    c -= T;
    update(c);
    return (int)c;
}

int DecodePosition(void)
{
    unsigned i, j, c;

    /* recover upper 6 bits from table */
    i = GetByte();
    c = (unsigned)d_code[i] << 6;
    j = d_len[i];

    /* read lower 6 bits verbatim */
    j -= 2;
    while (j--) {
        i = (i << 1) + GetBit();
    }
    return (int)(c | (i & 0x3f));
}

/* compression */

void Encode(void)  /* compression */
{
    int  i, c, len, r, s, last_match_length;

    textsize = data_size;
	ResizeOutCache((textsize*3)/2);
	_putb((textsize & 0xff));
	_putb((textsize & 0xff00) >> 8);
	_putb((textsize & 0xff0000L) >> 16);
	_putb((textsize & 0xff000000L) >> 24);
    if (textsize == 0)
        return;
    textsize = 0;           /* rewind and re-read */
    StartHuff();
    InitTree();
    s = 0;
    r = N - F;
    for (i = s; i < r; i++)
        text_buf[i] = 0x20;
    for (len = 0; len < F && (c = _getb()) != EOF; len++)
        text_buf[r + len] = (unsigned char)c;
    textsize = len;
    for (i = 1; i <= F; i++)
        InsertNode(r - i);
    InsertNode(r);
    do {
        if (match_length > len)
            match_length = len;
        if (match_length <= THRESHOLD) {
            match_length = 1;
			// textsize==56158    - FATAL :(
            EncodeChar(text_buf[r]);
        } else {
            EncodeChar(255 - THRESHOLD + match_length);
            EncodePosition(match_position);
        }
        last_match_length = match_length;
        for (i = 0; i < last_match_length &&
                (c = _getb()) != EOF; i++) {
            DeleteNode(s);
            text_buf[s] = (unsigned char)c;
            if (s < F - 1)
                text_buf[s + N] = (unsigned char)c;
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            InsertNode(r);
        }
		textsize += i;
		/*
        if (textsize > printcount) {
            Msg("%12ld\r", textsize);
            printcount += 32768;
        }
		*/
        while (i++ < last_match_length) {
            DeleteNode(s);
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            if (--len) InsertNode(r);
        }
    } while (len > 0);
    EncodeEnd();
#ifndef MODULE
    Msg("In : %ld bytes\n", textsize);
    Msg("Out: %ld bytes\n", codesize);
    Msg("Out/In: %.3f\n", 1.0 * codesize / textsize);
#endif
	tim_size = textsize;
}

void Decode(void)  /* recover */
{
    int  i, j, k, r, c;
    unsigned int  count;

    textsize =  (_getb());
    textsize |= (_getb() << 8);
    textsize |= (_getb() << 16);
    textsize |= (_getb() << 24);
    if (textsize == 0) return;
	ResizeOutCache(textsize);
    StartHuff();
    for (i = 0; i < N - F; i++)
        text_buf[i] = 0x20;
    r = N - F;
    for (count = 0; count < textsize; ) {
        c = DecodeChar();
        if (c < 256) {
            _putb(c);
            text_buf[r++] = (unsigned char)c;
            r &= (N - 1);
            count++;
        } else {
            i = (r - DecodePosition() - 1) & (N - 1);
            j = c - 255 + THRESHOLD;
            for (k = 0; k < j; k++) {
                c = text_buf[(i + k) & (N - 1)];
                _putb(c);
                text_buf[r++] = (unsigned char)c;
                r &= (N - 1);
                count++;
            }
        }
		/*
        if (count > printcount) {
            Msg("%12ld\r", count);
            printcount += 1024;
        }
		*/
    }
#ifndef MODULE
    Msg("%12ld\n", textsize);
#endif
	tim_size = count;
}

unsigned _writeLZ	(int hf, void* d, unsigned size)
{
	// Read file in memory
	data_size	= size;
	data		= (uchar*)d;
	data_ptr	= 0;

	// Write-Back cache
	data_out		= NULL;
	data_out_ptr	= 0;
	data_out_size	= 0;

	// Actual compression
#ifndef MODULE
	DWORD tm = timeGetTime();
#endif
    Encode();
#ifndef MODULE
	tm = timeGetTime() - tm;
#endif

	// Flush cache
	if (data_out_ptr) _write(hf,data_out,data_out_ptr);
	free(data_out);
#ifndef MODULE
	Msg("* %d ms elapsed\n* %5.1f Kb/sec processing speed\n",tm,(float(tim_size)/1024.f)/(float(tm)/1000.f));
#endif
	return data_out_ptr;
}

unsigned _readLZ	(int hf, void* &d, unsigned size)
{
	// Read file in memory
	data_size	= size;
	data		= (uchar*)malloc(size);
	_read(hf,data,size);
	data_ptr	= 0;

	// Write-Back cache
	data_out		= (uchar *)d;
	data_out_ptr	= 0;
	data_out_size	= 0;

	// Actual compression
#ifndef MODULE
	DWORD tm = timeGetTime();
#endif
    Decode();
#ifndef MODULE
	tm = timeGetTime() - tm;
#endif

	// Flush cache
	free(data);
	d = data_out;
#ifndef MODULE
	Msg("* %d ms elapsed\n* %5.1f Kb/sec processing speed\n",tm,(float(tim_size)/1024.f)/(float(tm)/1000.f));
#endif
	return data_out_size;
}

#ifndef MODULE
int __cdecl main(int argc, char *argv[])
{
    if (argc != 4) {
        Msg("'lz4k e file1 file2' encodes file1 into file2.\n"
               "'lz4k d file2 file1' decodes file2 into file1.\n");
        return EXIT_FAILURE;
    }
    if (toupper(*argv[1]) == 'E') {
		int fin		= _open(argv[2],O_RDONLY|O_BINARY);
		int fout	= _open(argv[3],O_WRONLY|O_BINARY|O_CREAT,S_IREAD|S_IWRITE);
		unsigned sz = filelength(fin);
		void *d		= malloc(sz);
		_read		(fin,d,sz);
		_writeLZ	(fout,d,sz);
		_close		(fin);
		_close		(fout);
		free		(d);
    } else {
		int fin		= _open(argv[2],O_RDONLY|O_BINARY);
		int fout	= _open(argv[3],O_WRONLY|O_BINARY|O_CREAT,S_IREAD|S_IWRITE);
		unsigned sz = filelength(fin);
		void *d		= NULL;
		_readLZ		(fin,d,sz);
		_write		(fout,d,sz);
		_close		(fin);
		_close		(fout);
		free		(d);
	}
    return EXIT_SUCCESS;
}
#endif
