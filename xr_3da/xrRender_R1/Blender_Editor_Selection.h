#ifndef BLENDER_EDITOR_SELECTION_H
#define BLENDER_EDITOR_SELECTION_H
#pragma once

#include "blender.h"

class ENGINE_API CBlender_Editor_Selection : public CBlender  
{
	string64	oT_Factor;
public:
	virtual		LPCSTR		getComment()	{ return "EDITOR: selection"; }
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }
	
	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Editor_Selection();
	virtual ~CBlender_Editor_Selection();

};

#endif //BLENDER_EDITOR_SELECTION_H
