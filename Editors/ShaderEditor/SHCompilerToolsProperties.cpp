//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHCompilerTools.h"
#include "folderlib.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void CSHCompilerTools::UpdateProperties()
{
	PropItemVec items;
	if (m_Shader){
		Shader_xrLC& L 			= *m_Shader;
        PHelper.CreateName_TI	(items, "Name",					L.Name,  		sizeof(L.Name), FHelper.FindObject(View(),L.Name));
        PHelper.CreateFloat		(items, "Translucency",			&L.vert_translucency);
        PHelper.CreateFloat		(items, "Ambient",				&L.vert_ambient);
        PHelper.CreateFloat		(items, "LM density",			&L.lm_density,   0.01f,20.f);

        PHelper.CreateFlag32	(items, "Flags\\Collision",		&L.m_Flags,   	Shader_xrLC::flCollision);
        PHelper.CreateFlag32	(items, "Flags\\Rendering",		&L.m_Flags,   	Shader_xrLC::flRendering);
        PHelper.CreateFlag32	(items, "Flags\\OptimizeUV",	&L.m_Flags,   	Shader_xrLC::flOptimizeUV);
        PHelper.CreateFlag32	(items, "Flags\\Vertex light",	&L.m_Flags,   	Shader_xrLC::flLIGHT_Vertex);
        PHelper.CreateFlag32	(items, "Flags\\Cast shadow",	&L.m_Flags,   	Shader_xrLC::flLIGHT_CastShadow);
        PHelper.CreateFlag32	(items, "Flags\\Sharp",			&L.m_Flags,   	Shader_xrLC::flLIGHT_Sharp);
        PHelper.CreateFlag32	(items, "Flags\\Leave normals",	&L.m_Flags,   	Shader_xrLC::flSaveNormals);
    }
    Ext.m_ItemProps->AssignItems(items,true);
    Ext.m_ItemProps->SetModifiedEvent(Modified);
}
//---------------------------------------------------------------------------

 