//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "EShape.h"

//---------------------------------------------------------------------------

CEditShape::CEditShape(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
}

CEditShape::~CEditShape()
{
}

void CEditShape::Construct(LPVOID data)
{
	m_Box.invalidate();
}

bool CEditShape::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf)
{
	return false;
}

bool CEditShape::FrustumPick(const CFrustum& frustum)
{
	return false;
}

bool CEditShape::GetBox(Fbox& box)
{
	if (m_Box.is_Valid()){
    	box.xform(m_Box,FTransformRP);
    	return true;
    }
	return false;
}

bool CEditShape::Load(CStream& F)
{
	return false;
}

void CEditShape::Save(CFS_Base& F)
{
}

void CEditShape::FillProp(LPCSTR pref, PropValueVec& values)
{
}

void CEditShape::Render(int priority, bool strictB2F)
{
}

void CEditShape::OnFrame()
{
}

void CEditShape::OnShowHint(AStringVec& dest)
{
}

