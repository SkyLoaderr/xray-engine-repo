#pragma once

class CBlender_deffer_model : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: deffer-model-flat";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, WORD version);
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_deffer_model();
	virtual ~CBlender_deffer_model();
};
