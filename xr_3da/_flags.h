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
	T 	m_Flags;

    IC	void	zero	()											{m_Flags=(T)0x00000000;}
    IC	void	one		()											{m_Flags=(T)0xffffffff;}
    IC	void	invert	()											{m_Flags=!m_Flags;}
    IC	void	invert	(const Self& flags)							{m_Flags=!flags.m_Flags;}
    IC	void	invert	(const T mask)								{set(mask,!is(mask));}
	IC	void	set		(const T mask)								{m_Flags=mask;}
	IC	void	set		(const T mask, BOOL value)					{if (value) m_Flags|=mask; else m_Flags&=~mask; }
	IC 	BOOL	is		(const T mask)						const	{return !!(m_Flags&mask);}
	IC 	void	or		(const T mask)								{m_Flags|=mask;}
	IC 	void	or		(const Self& flags, const T mask) 			{m_Flags=flags.m_Flags|mask;}
	IC 	void	and		(const T mask)								{m_Flags&=mask;}
	IC 	void	and		(const Self& flags, const T mask) 			{m_Flags=flags.m_Flags&mask;}
	IC 	BOOL	equal	(const Self& flags) 			  	const	{return m_Flags==flags;}
	IC 	BOOL	equal	(const Self& flags, const T mask) 	const	{return (m_Flags&mask)&&(flags.m_Flags&mask);}
};

typedef _flags<u8>	Flags8;
typedef _flags<u16>	Flags16;
typedef _flags<u32>	Flags32;


#endif //__FLAGS_H__
