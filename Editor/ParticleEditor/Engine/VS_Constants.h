#pragma once

class ENGINE_API CVS_Constants
{
private:
	svector<Fvector4,256>	array;
public:
	IC void					init	(u32 size)				{ array.resize(size);	}
	IC Fvector4*			access	(u32 id)				{ return &array[id];	}
	IC void					flush	(u32 start, u32 count)	{ HW.pDevice->SetVertexShaderConstant(start,access(start),count); }

	IC void					set		(u32 id, Fmatrix& A)	{ ((Fmatrix*)access(id))->transpose(A); }
	IC void					set		(u32 id, Fvector& A)	{ access(id)->set(A.x,A.y,A.z,1);		}
	IC void					set		(u32 id, DWORD A)		
	{ 
		const float f = 1.0f / 255.0f;
		access(id)->set(
			f * (float) BYTE(A >> 16),	// r/x
			f * (float) BYTE(A >>  8),	// g/y
			f * (float) BYTE(A >>  0),	// b/z
			f * (float) BYTE(A >> 24)	// w/a
		);
	}
	IC void					set		(u32 id, float x, float y, float z, float w)
	{ 	access(id)->set(x,y,z,w);	}
};
