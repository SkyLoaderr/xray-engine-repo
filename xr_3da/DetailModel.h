#pragma once

class CDetail		: public IRender_DetailModel
{
	void		Load		(IReader* S);
	void		Optimize	();
	void		Unload		();
	void		Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, WORD* iDest, u32 iOffset);

	virtual		~CDetail	();
};

