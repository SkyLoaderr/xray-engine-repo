//----------------------------------------------------
// file: SceneObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "SceneObject.h"
#include "ui_main.h"

//----------------------------------------------------

void SceneObject::Select( bool flag )
{
    if (m_Visible) m_Selected = flag;
    UI.UpdateScene();
};

void SceneObject::Show( bool flag )
{
    m_Visible = flag;
    if (!m_Visible) m_Selected = false;
    UI.UpdateScene();
};


//----------------------------------------------------

