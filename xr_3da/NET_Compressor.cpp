// NET_Compressor.cpp: implementation of the NET_Compressor class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NET_Compressor.h"

#define NOWARN

/* SIZE OF RANGE ENCODING CODE VALUES. */
#define CODE_BITS		32
#define Top_value		((code_value)1 << (CODE_BITS-1))

#define SHIFT_BITS		(CODE_BITS - 9)
#define EXTRA_BITS		((CODE_BITS-2) % 8 + 1)
#define Bottom_value	(Top_value >> 8)

/* c is written as first byte in the datastream                */
/* one could do without c, but then you have an additional if  */
/* per outputbyte.                                             */
void NET_Compressor::start_encoding		( BYTE* dest, DWORD header_size )
{  
	dest			+=	header_size-1;
	RNGC.low		=	0;				/* Full code range */
    RNGC.range		=	Top_value;
    RNGC.buffer		=	0;
    RNGC.help		=	0;				/* No bytes to follow */
    RNGC.bytecount	=	0;
	RNGC.ptr		=	dest;
}

/* I do the normalization before I need a defined state instead of */
/* after messing it up. This simplifies starting and ending.       */
void NET_Compressor::encode_normalize	( )
{   
	while(RNGC.range <= Bottom_value)     /* do we need renormalisation?  */
    {   
		if (RNGC.low < code_value(0xff) << SHIFT_BITS)  /* no carry possible --> output */
        {
			RNGC.byte_out	(RNGC.buffer);
            for(; RNGC.help; RNGC.help--)	RNGC.byte_out(0xff);
            RNGC.buffer		= (BYTE)(RNGC.low >> SHIFT_BITS);
        } 
		else if (RNGC.low & Top_value) /* carry now, no future carry */
        {   
			RNGC.byte_out	(RNGC.buffer+1);
            for(; RNGC.help; RNGC.help--)	RNGC.byte_out(0);
            RNGC.buffer		= (BYTE)(RNGC.low >> SHIFT_BITS);
        } else                           /* passes on a potential carry */
            RNGC.help++;
        RNGC.range		<<= 8;
        RNGC.low		=	(RNGC.low<<8) & (Top_value-1);
        RNGC.bytecount	++;
    }
}

/* Encode a symbol using frequencies                         */
/* sy_f is the interval length (frequency of the symbol)     */
/* lt_f is the lower end (frequency sum of < symbols)        */
/* tot_f is the total interval length (total frequency sum)  */
/* or (faster): tot_f = (code_value)1<<shift                             */
void NET_Compressor::encode_freq(freq sy_f, freq lt_f, freq tot_f )
{	
	code_value	r, tmp;

	encode_normalize();
	r			=	RNGC.range / tot_f;
	tmp			=	r * lt_f;
	RNGC.low	+=	tmp;
    if (lt_f+sy_f < tot_f)		RNGC.range  =	r * sy_f;
    else						RNGC.range  -=	tmp;
}

void NET_Compressor::encode_shift	( freq sy_f, freq lt_f, freq shift )
{	
	code_value r, tmp;

	encode_normalize();
	r			= RNGC.range >> shift;
	tmp			= r * lt_f;
	RNGC.low	+= tmp;
	if ((lt_f+sy_f) >> shift)	RNGC.range -= tmp;
	else						RNGC.range = r * sy_f;
}

/* Finish encoding                                           */
/* actually not that many bytes need to be output, but who   */
/* cares. I output them because decode will read them :)     */
/* the return value is the number of bytes written           */
DWORD NET_Compressor::done_encoding	( )
{   
	DWORD tmp;

    encode_normalize	();     /* now we have a normalized state */
    RNGC.bytecount		+= 3;
    if ((RNGC.low & (Bottom_value-1)) < ((RNGC.bytecount&0xffffffL)>>1))	tmp = RNGC.low >> SHIFT_BITS;
    else																	tmp = (RNGC.low >> SHIFT_BITS) + 1;

    if (tmp > 0xff) /* we have a carry */
    {   
		RNGC.byte_out	(RNGC.buffer+1);
        for(; RNGC.help; RNGC.help--)	RNGC.byte_out(0);
    } else  /* no carry */
    {
		RNGC.byte_out	(RNGC.buffer);
        for(; RNGC.help; RNGC.help--)	RNGC.byte_out(0xff);
    }
    RNGC.byte_out	(BYTE(tmp & 0xff));
    RNGC.byte_out	(0);
    return			RNGC.bytecount;
}

/* Start the decoder                                         */
int NET_Compressor::start_decoding	( BYTE* src, DWORD header_size )
{
	src			+= header_size;
	RNGC.ptr	= src;
    RNGC.buffer = RNGC.byte_in();
    RNGC.low	= RNGC.buffer >> (8-EXTRA_BITS);
    RNGC.range	= (code_value)1 << EXTRA_BITS;
    return 0;
}

void NET_Compressor::decode_normalize	( )
{   
	while (RNGC.range <= Bottom_value)
    {   
		RNGC.low	=	(RNGC.low<<8) | ((RNGC.buffer<<EXTRA_BITS)&0xff);
        RNGC.buffer =	RNGC.byte_in();
        RNGC.low	|=	RNGC.buffer >> (8-EXTRA_BITS);
        RNGC.range	<<= 8;
    }
}


/* Calculate culmulative frequency for next symbol. Does NO update!	*/
/* tot_f is the total frequency										*/
/* or: totf is (code_value)1<<shift                                 */
/* returns the culmulative frequency								*/
NET_Compressor::freq NET_Compressor::decode_culfreq		( freq tot_f )
{   
	freq tmp;

    decode_normalize	();
    RNGC.help			= RNGC.range/tot_f;
    tmp					= RNGC.low/RNGC.help;
    return (tmp>=tot_f) ? (tot_f-1) : (tmp);
}

NET_Compressor::freq NET_Compressor::decode_culshift	( freq shift )
{
	freq tmp;

    decode_normalize	();
    RNGC.help			= RNGC.range>>shift;
    tmp					= RNGC.low/RNGC.help;
    return				(tmp>>shift) ? ((code_value(1)<<shift)-1) : (tmp);
}


/* Update decoding state                                     */
/* sy_f is the interval length (frequency of the symbol)     */
/* lt_f is the lower end (frequency sum of < symbols)        */
/* tot_f is the total interval length (total frequency sum)  */
void NET_Compressor::decode_update		( freq sy_f, freq lt_f, freq tot_f )
{
	code_value tmp;

    tmp			=	RNGC.help * lt_f;
    RNGC.low	-=	tmp;
    if (lt_f + sy_f < tot_f)	RNGC.range = RNGC.help * sy_f;
    else						RNGC.range -= tmp;
}


/* Decode a byte/short without modelling                     */
BYTE NET_Compressor::decode_byte		( )
{
	DWORD tmp	=	decode_culshift(8);
    decode_update	( 1,tmp,(freq)1<<8 );
    return BYTE(tmp);
}

WORD NET_Compressor::decode_short		( )
{   
	DWORD tmp	=	decode_culshift(16);
    decode_update	( 1,tmp,(freq)1<<16);
    return WORD(tmp);
}


/* Finish decoding                                           */
/* rc is the range coder to be used                          */
void NET_Compressor::done_decoding		( )
{
	decode_normalize();      /* normalize to use up all bytes */
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NET_Compressor::NET_Compressor()
{

}

NET_Compressor::~NET_Compressor()
{

}

void NET_Compressor::Initialize	(NET_Compressor_FREQ& compress, NET_Compressor_FREQ& decompress)
{
	CS.Enter		();
	freqCompress	= compress;
	freqCompress.Cumulate	();

	freqDecompress	= decompress;
	freqDecompress.Cumulate	();
	CS.Leave		();
}

WORD NET_Compressor::Compress	(BYTE* dest, BYTE* src, DWORD count)
{
	R_ASSERT(dest && src && count);

	CopyMemory	(dest,src,count);
	return WORD(count);

	/*
	CS.Enter		();
    start_encoding	(dest, 2);
	
	// output the encoded symbols
	DWORD	cum	= freqCompress[256];
	BYTE*	end	= src+count;
	for(; src!=end; ) 
	{
		int ch				=	*src++;
		DWORD freq			=	freqCompress[ch];
		encode_freq			(freqCompress[ch+1]-freq, freq, cum);
	}

    // close the encoder
	*LPWORD(dest)			=	WORD(count);
	DWORD size				=	done_encoding(); //-1+2;
	CS.Leave		();

    return WORD(size);
	*/
}


WORD NET_Compressor::Decompress	(BYTE* dest, BYTE* src, DWORD count)
{  
	R_ASSERT(dest && src && count);

	CopyMemory	(dest,src,count);
	return		WORD(count);

	/*
	CS.Enter		();
	DWORD			size	= DWORD(*LPWORD(src));

	start_decoding	(src, 2);
	
	DWORD			cum = freqDecompress[256];
	for (DWORD i=0; i<size; i++)
	{
		freq	cf, symbol;
		cf		= decode_culfreq(cum);

		// 1
		DWORD*	begin	= freqDecompress.table;
		DWORD*	end		= begin+257;
		DWORD*	pEqual	= std::upper_bound(begin,end,DWORD(cf));
		symbol	= pEqual-begin-1;

		// 2
		// for (symbol=0; freqDecompress[symbol+1] <= cf; symbol++);

		freq	Fcur	= freqDecompress[symbol];
		freq	Fnext	= freqDecompress[symbol+1];
		decode_update	(Fnext-Fcur,Fcur,cum);
		*dest ++		= BYTE(symbol);
	}
	
    done_decoding	();
	CS.Leave		();
	return WORD(size);
	*/
}

void NET_Compressor_FREQ::Normalize()
{
	// summarize counters
	DWORD I, total	= 0;
	for (I=0; I<256; I++)	total += table[I];
	
	if (total>60000)	{
		// rescale counters so in summary they gives us less than 60 000 freq
		float fScale	= float(60000)/float(total);
		for (I=0; I<256; I++) {
			float	value	= float(table[I]);
			float	scaled	= value*fScale;
			table[I]		= iFloor(scaled);
		}
	}
}
