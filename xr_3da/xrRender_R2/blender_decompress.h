#pragma once

class CBlender_decompress : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: MET decompress";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_decompress();
	virtual ~CBlender_decompress();
};
