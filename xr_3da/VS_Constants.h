#pragma once

class ENGINE_API CVS_Constants
{
private:
	svector<Fvector4,256>	array;
public:
	IC void					init	(u32 size)				{ array.resize(size);	}
	IC Fvector4*			access	(u32 id)				{ return &array[id];	}
	IC void					set		(u32 start, u32 count)	{ HW.pDevice->SetVertexShaderConstant(start,access(start),count); }
};
