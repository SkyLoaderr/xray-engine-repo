#pragma once

class ENGINE_API CVS_Constants
{
private:
	svector<Fvector4,256>	array;
	u32						lo,hi;
public:
	IC void					init	(u32 size)				{ array.resize(size);	lo=hi=0;		}
	IC Fvector4*			access	(u32 id)				{ return &array[id];					}
	IC void					flush	()	
	{
		u32		count		= hi-lo;
		if (count)			{
			HW.pDevice->SetVertexShaderConstant(lo,access(lo),count);
			lo	=	hi	=	0;
		}
	}
	IC void					dirty	(u32 _lo, u32 _hi)
	{
		if (_lo<lo)	lo=_lo;
		if (_hi>hi)	hi=_hi;
	}

	IC void					set_m4x3	(u32 id, Fmatrix&	A)	// transpose and set
	{ 
		dirty				(id,id+3);
		Fvector4*	it		= (Fvector4*)access	(id);
		it[0]->set			(A._11, A._21, A._31, A._41);
		it[1]->set			(A._12, A._22, A._32, A._42);
		it[2]->set			(A._13, A._23, A._33, A._43);
	}
	IC void					set_m4x3t	(u32 id, Fmatrix&	A)	// already-transposed
	{ 
		dirty				(id,id+3);
		Fvector4*	it		= (Fvector4*)access	(id);
		it[0]->set			(A._11, A._12, A._13, A._14);
		it[1]->set			(A._21, A._22, A._23, A._24);
		it[2]->set			(A._31, A._32, A._33, A._34);
	}
	IC void					set_m4x4	(u32 id, Fmatrix&	A)	// transpose and set
	{ 
		dirty				(id,id+4);
		Fvector4*	it		= (Fvector4*)access	(id);
		it[0]->set			(A._11, A._21, A._31, A._41);
		it[1]->set			(A._12, A._22, A._32, A._42);
		it[2]->set			(A._13, A._23, A._33, A._43);
		it[3]->set			(A._14, A._24, A._34, A._44);
	}
	IC void					set_m4x4t	(u32 id, Fmatrix&	A)	// already-transposed
	{ 
		dirty				(id,id+4);
		Fvector4*	it		= (Fvector4*)access	(id);
		it[0]->set			(A._11, A._12, A._13, A._14);
		it[1]->set			(A._21, A._22, A._23, A._24);
		it[2]->set			(A._31, A._32, A._33, A._34);
		it[3]->set			(A._41, A._42, A._43, A._44);
	}

	IC void					set			(u32 id, Fvector&	A)	{ access(id)->set(A.x,A.y,A.z,1);		}
	IC void					set			(u32 id, Fvector4&	A)	{ access(id)->set(A.x,A.y,A.z,A.w);		}
	IC void					set			(u32 id, DWORD A)		
	{ 
		const float f = 1.0f / 255.0f;
		access(id)->set(
			f * float((A >> 16)&0xff),	// r/x
			f * float((A >>  8)&0xff),	// g/y
			f * float((A >>  0)&0xff),	// b/z
			f * float((A >> 24)&0xff)	// w/a
		);
	}
	IC void					set		(u32 id, float x, float y, float z, float w)
	{ 	access(id)->set(x,y,z,w);	}
};
