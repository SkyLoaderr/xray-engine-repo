#pragma once

class	ENGINE_API	CMatrix
{
public:
	LPSTR			name;
	Fmatrix			xform;
};

class	ENGINE_API	CMatrixArray
{
public:
	typedef svector<CMatrix*,8>	VECTOR;

	svector<VECTOR,8>	matrices;

	VECTOR&				Pass		(DWORD ID)	{ return constants[ID]; }

	CMatrixArray		()
	{
		ZeroMemory		(this,sizeof(*this)));
	}
};
