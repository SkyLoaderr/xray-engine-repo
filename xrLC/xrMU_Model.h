#pragma once

class xrMU_Model
{
public:
	struct	_vertex
	{
		Fvector		P;
		Fvector		N;
	};
public:
	void		Load	(CStream& FS);
};

class xrMU_Reference
{
public:

public:
	void		Load	(CStream& FS);
};
