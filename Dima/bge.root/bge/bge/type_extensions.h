////////////////////////////////////////////////////////////////////////////
//	Module 		: type_extensions.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Type extensions
////////////////////////////////////////////////////////////////////////////

#pragma once

// types
typedef	signed		char	s8;
typedef	unsigned	char	u8;

typedef	signed		short	s16;
typedef	unsigned	short	u16;

typedef	signed		int		s32;
typedef	unsigned	int		u32;

typedef	signed		__int64	s64;
typedef	unsigned	__int64	u64;

typedef float				f32;
typedef double				f64;

typedef char*				LPSTR;
typedef const char*			LPCSTR;

typedef	char	string16	[16];
typedef	char	string32	[32];
typedef	char	string64	[64];
typedef	char	string128	[128];
typedef	char	string256	[256];
typedef	char	string512	[512];
typedef	char	string1024	[1024];
typedef	char	string2048	[2048];
typedef	char	string4096	[4096];

#define xr_vector	std::vector
#define xr_set		std::set
#define xr_map		std::map
#define xr_list		std::list
#define xr_stack	std::stack
