////////////////////////////////////////////////////////////////////////////
//	Module 		: memory.cpp
//	Created 	: 12.11.2004
//  Modified 	: 12.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Hardware namespace
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memory.h"
#include "cpu.h"

//void test();

void Memory::init		()
{
//	test	();
}

void amd_copy(void *dest, const void *src, unsigned int n);

void Memory::mem_copy	(void *dest, const void *src, size_t size)
{
//	amd_copy(dest,src,(u32)size);
	memcpy	(dest,src,size);
}

void Memory::mem_fill	(void *dest, char val,  size_t size)
{
	memset	(dest,val,size);
}

template <bool MMX, bool SSE, bool SSE2, bool SSE3, bool _3DNow>
class memory_processor {
public:
	template <size_t size>
	IC	static void copy(void *dest, const void *src)
	{
		memcpy(dest,src,size);
	}
};

template <bool SSE, bool SSE2, bool SSE3, bool _3DNow>
class memory_processor<true,SSE,SSE2,SSE3,_3DNow> {
public:
	template <int size>
	IC	static void copy_blocks(void *dest, const void *src)
	{
		copy<64>(dest,src);
		copy_blocks<size - 1>((char*)dest + 64, (char*)src + 64);
	}

	template <>
	IC	static void copy_blocks<0>(void *dest, const void *src)
	{
	}

	template <int size>
	IC	static void copy_last(void *dest, const void *src)
	{
		memcpy(dest,src,size);
	}

	template <>
	IC	static void copy_last<0>(void *dest, const void *src)
	{
	}

	template <size_t size>
	IC	static void copy(void *dest, const void *src)
	{
		copy_blocks<size/64>(dest,src);
		copy_last<size%64>((char*)dest + (size/64)*64, (char*)src + (size/64)*64);
	}

	template <>
	IC	static void copy<64>(void *dest, const void *src)
	{
		__asm {
			movq	mm0,src +  0
			movq	mm1,src +  8
			movq	mm2,src + 16
			movq	mm3,src + 24
			movq	mm4,src + 32
			movq	mm5,src + 40
			movq	mm6,src + 48
			movq	mm7,src + 56
			movq	dest +  0,mm0
			movq	dest +  8,mm1
			movq	dest + 16,mm2
			movq	dest + 24,mm3
			movq	dest + 32,mm4
			movq	dest + 40,mm5
			movq	dest + 48,mm6
			movq	dest + 56,mm7
			emms
		};
	}
};

/**
void test()
{
	const u32 test_count = 100000;
	const u32 size = 0x100;
	char s[size], d[size];
	typedef memory_processor<true,true,false,false,true> mp;
	Memory::mem_fill(s,1,size);
	u64 start,finish;
	
	SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep(1);
	
	{
		start = CPU::cycles();
		for (u32 i=0; i<test_count; ++i)
			mp::copy<size>(d,s);
		finish = CPU::cycles();
		ui().log("%f\n",float(finish-start)*CPU::cycles2milisec);
	}

	{
		start = CPU::cycles();
		for (u32 i=0; i<test_count; ++i)
			Memory::mem_copy(d,s,size);
		finish = CPU::cycles();
		ui().log("%f\n",float(finish-start)*CPU::cycles2milisec);
	}

	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
}
/**/
