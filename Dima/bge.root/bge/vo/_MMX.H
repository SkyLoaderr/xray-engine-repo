//////////////////////////////////////////////////////////
//	Module			:	Routines with a speedup of MMX	//
//	File			:	_mmx.h							//
//	Creation date	:	31.08.2002						//
//	Author			:	Oles Shishkovtsov				//
//	Modifications	:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#ifndef __VO_MMX__
#define __VO_MMX__

#define MMX_FLUSH	__asm emms;

#ifdef __MPI__
	#define COPY_EF_INDEXES(Array0,Array1) \
		TIMER_START(MPIMemCpy)\
		vfCopyEFIndexes(Array0,Array1);\
		TIMER_STOP(MPIMemCpy)
	
	#define COPY_MOVES(Array0,Array1) \
		TIMER_START(GlobalMoveStack)\
		vfCopyMoves(Array0,Array1);\
		TIMER_STOP(GlobalMoveStack)
#endif

#define COPY_MEMORY(a,b,c) \
	TIMER_START(GlobalMoveStack)\
	memcpy(a,b,c);\
	TIMER_STOP(GlobalMoveStack)

#define SWAP_SAVED_MOVES(a,b) \
	TIMER_START(SwapMoves)\
	vfSwapSavedMoves(a,b);\
	TIMER_STOP(SwapMoves)

IC void vfCopyEFIndexes(u16 *waDestination, u16 *waSource)
{
	__asm {
		mov		eax,[waDestination];
		mov		edx,[waSource];
		
		movq	mm0,[edx +  0*8];
		movq	[eax +  0*8],mm0;
		movq	mm1,[edx +  1*8];
		movq	[eax +  1*8],mm1;
		
		movq	mm2,[edx +  2*8];
		movq	mm3,[edx +  3*8];

		movq	[eax +  2*8],mm2;
		movq	[eax +  3*8],mm3;

		movq	mm0,[edx +  4*8];
		movq	mm1,[edx +  5*8];
		movq	mm2,[edx +  6*8];
		movq	mm3,[edx +  7*8];
		movq	mm4,[edx +  8*8];
		movq	mm5,[edx +  9*8];
		movq	mm6,[edx + 10*8];
		movq	mm7,[edx + 11*8];
		
		movq	[eax +  4*8],mm0;
		movq	[eax +  5*8],mm1;
		movq	[eax +  6*8],mm2;
		movq	[eax +  7*8],mm3;
		movq	[eax +  8*8],mm4;
		movq	[eax +  9*8],mm5;
		movq	[eax + 10*8],mm6;
		movq	[eax + 11*8],mm7;

		//emms;
	}
}

IC void vfCopyMoves(char *caDestination, char *caSource)
{
	__asm {
		mov		eax,[caDestination];
		mov		edx,[caSource];
		
		movq	mm0,[edx +  0*8];
		movq	mm1,[edx +  1*8];
		movq	mm2,[edx +  2*8];
		movq	mm3,[edx +  3*8];
		movq	mm4,[edx +  4*8];
		movq	mm5,[edx +  5*8];
		movq	mm6,[edx +  6*8];
		movq	mm7,[edx +  7*8];
		
		movq	[eax +  0*8],mm0;
		movq	[eax +  1*8],mm1;
		movq	[eax +  2*8],mm2;
		movq	[eax +  3*8],mm3;
		movq	[eax +  4*8],mm4;
		movq	[eax +  5*8],mm5;
		movq	[eax +  6*8],mm6;
		movq	[eax +  7*8],mm7;

		//emms;
	}
}

IC void vfSwapSavedMoves(char *caDestination, char *caSource)
{
	__asm {
		mov		eax,[caDestination];
		mov		edx,[caSource];
		
		movq	mm0,[eax +  0*8];
		movq	mm1,[eax +  1*8];
		movq	mm2,[eax +  2*8];
		movq	mm3,[edx +  0*8];
		movq	mm4,[edx +  1*8];
		movq	mm5,[edx +  2*8];
		
		movq	[edx +  0*8],mm0;
		movq	[edx +  1*8],mm1;
		movq	[edx +  2*8],mm2;
		movq	[eax +  0*8],mm3;
		movq	[eax +  1*8],mm4;
		movq	[eax +  2*8],mm5;

		//emms;
	}
}

#endif //__VO_MMX__
