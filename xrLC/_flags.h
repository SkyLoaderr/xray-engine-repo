#ifndef __FLAGS_H__
#define __FLAGS_H__

template <class T>
struct _flags {
public:
	typedef T			TYPE;
	typedef _flags<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T 	flags;

    IC	TYPE	get		()											{return flags;}
    IC	void	zero	()											{flags=(T)0x00000000;}
    IC	void	one		()											{flags=(T)0xffffffff;}
    IC	void	invert	()											{flags=!flags;}
    IC	void	invert	(const Self& flags)							{flags=!flags.flags;}
    IC	void	invert	(const T mask)								{set(mask,!is(mask));}
	IC	void	set		(const T mask)								{flags=mask;}
	IC	void	set		(const T mask, BOOL value)					{if (value) flags|=mask; else flags&=~mask; }
	IC 	BOOL	is		(const T mask)						const	{return !!(flags&mask);}
	IC 	void	or		(const T mask)								{flags|=mask;}
	IC 	void	or		(const Self& flags, const T mask) 			{flags=flags.flags|mask;}
	IC 	void	and		(const T mask)								{flags&=mask;}
	IC 	void	and		(const Self& flags, const T mask) 			{flags=flags.flags&mask;}
	IC 	BOOL	equal	(const Self& flags) 			  	const	{return flags==flags;}
	IC 	BOOL	equal	(const Self& flags, const T mask) 	const	{return (flags&mask)&&(flags.flags&mask);}
};

typedef _flags<u8>	Flags8;
typedef _flags<u16>	Flags16;
typedef _flags<u32>	Flags32;


#endif //__FLAGS_H__
