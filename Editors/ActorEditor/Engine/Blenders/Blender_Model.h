// Blender_Model.h: interface for the Blender_Screen_SET class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_MODEL_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_)
#define AFX_BLENDER_MODEL_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_
#pragma once

#include "blender.h"

class ENGINE_API CBlender_Model : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "MODEL: Default"; }
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Model();
	virtual ~CBlender_Model();
};

#endif // !defined(AFX_BLENDER_MODEL_H__A215FA40_D885_4D06_9032_ED934AE295E3__INCLUDED_)
