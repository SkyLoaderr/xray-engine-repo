//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "SHMap.h"
#include "SceneClassList.h"
#include "UI_Main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

SHMap::SHMap( )
    :SceneObject()
{
	m_ClassID = OBJCLASS_HMAP;
    fDimAlt = 30.f;
    vOffset.set(0,0,0);
    fSlotSize = 0.5f;
}

void SHMap::Create( TPicture* pct )
{
    width = pct->Width; height = pct->Height;
    HMap = (BYTE*)malloc(height*width);
    for (int row=0; row<height; row++)
        CopyMemory(&HMap[width*row],pct->Bitmap->ScanLine[row],width);
}

void SHMap::Clear( )
{
    free(HMap);
}

SHMap::~SHMap(){
}


