#pragma once

class CDetail		: public IRender_DetailModel
{
public:
	void			Load		(IReader* S);
	void			Optimize	();
	void			Unload		();
	virtual void	Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset);

	virtual			~CDetail	();
};

