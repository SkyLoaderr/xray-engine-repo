#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include "net_compressor.h"

#pragma comment(lib,"winmm.lib")

#define MSG_SIZE	(50000)
typedef	BYTE	buffer[MSG_SIZE*2];
void __cdecl main(int argc, char *argv[])
{
	buffer	source		= "hello world!!! my sweet greets to you!";
	buffer	compressed;
	buffer	uncompressed;

	timeBeginPeriod(1);
	DWORD T = timeGetTime();
	for (int TEST=0; TEST<400; TEST++) 
	{
		NET_Compressor_FREQ	S;
		// fill source & calculate stats
		for (int i=0; i<MSG_SIZE; i++) 
		{
			int	C		=	rand()%256;
			source[i]	=	BYTE(C);
//			int C		=	source[i];
			S[C]		+=	1;
		}
		
		// test
		NET_Compressor	C;
		C.Initialize	(S,S);
		WORD sz			= C.Compress(compressed,source,MSG_SIZE);
//		printf("%2d === %4d : %4d -> %3.1f, -",TEST,DWORD(sz),DWORD(MSG_SIZE),100.f*float(sz)/float(MSG_SIZE));
		C.Decompress	(uncompressed,compressed,DWORD(sz));
		/*
		if (memcmp(source,uncompressed,MSG_SIZE)==0)	printf("OK\n");
		else {
			printf("FAILED");

			for (int b=0; b<MSG_SIZE; b++)
				if (source[b]!=uncompressed[b]) printf("(%d: %x/%x) ",b,int(source[b]),int(uncompressed[b]));

			printf("\n");
			getch ();
		}
		*/
	}
	printf("%d ms\n",timeGetTime()-T);
	timeEndPeriod(1);
}

