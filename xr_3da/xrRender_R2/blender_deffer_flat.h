#pragma once

class CBlender_deffer_flat : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: defer-base-flat";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Save			(IWriter&	FS);
	virtual		void		Load			(IReader&	FS, WORD version);
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_deffer_flat();
	virtual ~CBlender_deffer_flat();
};
