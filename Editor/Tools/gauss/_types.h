#ifndef TYPES_H
#define TYPES_H

// Type defs
typedef	signed		__int8	s8;
typedef	unsigned	__int8	u8;

typedef	signed		__int16	s16;
typedef	unsigned	__int16	u16;

typedef	signed		__int32	s32;
typedef	unsigned	__int32	u32;

typedef	signed		__int64	s64;
typedef	unsigned	__int64	u64;

typedef float				f32;
typedef double				f64;

// Type limits
#define type_max(T)		(std::numeric_limits<T>::max())
#define type_min(T)		(-std::numeric_limits<T>::max())
#define type_zero(T)	(std::numeric_limits<T>::min())

#define int_max			type_max(int)
#define int_min			type_min(int)
#define int_zero		type_zero(int)

#define flt_max			type_max(float)
#define flt_min			type_min(float)
#define flt_zero		type_zero(float)

#define dbl_max			type_max(double)
#define dbl_min			type_min(double)
#define dbl_zero		type_zero(double)

#endif