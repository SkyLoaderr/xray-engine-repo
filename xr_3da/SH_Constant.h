#pragma once

class	ENGINE_API	CConstant
{
public:
	LPSTR			name;
	Fcolor			const_float;
	DWORD			const_dword;

	void			set_float	(float r, float g, float b, float a)
	{
		const_float.set	(r,g,b,a);
		const_dword		= const_float.get();
	}
	void			set_float	(Fcolor& c)
	{
		const_float.set	(c);
		const_dword		= const_float.get();
	}
	void			set_dword	(DWORD c)
	{
		const_float.set(c);
		const_dword		= c;
	}
};

class	ENGINE_API	CConstantArray
{
public:
	typedef svector<CConstant*,8>	VECTOR;

	svector<VECTOR,8>	constants;

	VECTOR&				Pass		(DWORD ID)	{ return constants[ID]; }

	CConstantArray	()
	{
		ZeroMemory	(this,sizeof(*this)));
	}
};
