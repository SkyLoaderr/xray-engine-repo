#include "stdafx.h"
#pragma hdrstop

#include "Scene.h"
#include "SceneObject.h"
#include "bottombar.h"
#include "d3dutils.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void __fastcall object_Normal_0(EScene::mapObject_Node *N){ (N->val)->Render( 0, false ); }
void __fastcall object_Normal_1(EScene::mapObject_Node *N){ (N->val)->Render( 1, false ); }
void __fastcall object_Normal_2(EScene::mapObject_Node *N){ (N->val)->Render( 2, false ); }
void __fastcall object_Normal_3(EScene::mapObject_Node *N){ (N->val)->Render( 3, false ); }
//------------------------------------------------------------------------------
void __fastcall object_StrictB2F_0(EScene::mapObject_Node *N){(N->val)->Render( 0, true );}
void __fastcall object_StrictB2F_1(EScene::mapObject_Node *N){(N->val)->Render( 1, true );}
void __fastcall object_StrictB2F_2(EScene::mapObject_Node *N){(N->val)->Render( 2, true );}
void __fastcall object_StrictB2F_3(EScene::mapObject_Node *N){(N->val)->Render( 3, true );}
//------------------------------------------------------------------------------

#define RENDER_SCENE_TOOLS(P,B)\
	{\
		SceneMToolsIt s_it 	= scene_tools.begin();\
		SceneMToolsIt s_end	= scene_tools.end();\
        for (; s_it!=s_end; s_it++){\
            Device.SetShader		(B?Device.m_SelectionShader:Device.m_WireShader);\
            RCache.set_xform_world	(Fidentity);\
            (*s_it)->OnRender(P,B);\
        }\
    }
        
void EScene::RenderSky(const Fmatrix& camera)
{
	if( !valid() )	return;
//	draw sky
	if (m_SkyDome&&fraBottomBar->miDrawSky->Checked){
        st_Environment& E = m_LevelOp.m_Envs[m_LevelOp.m_CurEnv];
        m_SkyDome->PPosition = camera.c;
        m_SkyDome->UpdateTransform(true);
		Device.SetRS(D3DRS_TEXTUREFACTOR, E.m_SkyColor.get());
    	m_SkyDome->RenderSingle();
	    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    }
}
//------------------------------------------------------------------------------

struct tools_rp_pred : public std::binary_function<ESceneCustomMTools*, ESceneCustomMTools*, bool>
{
    IC bool operator()(ESceneCustomMTools* x, ESceneCustomMTools* y) const
    {	return x->render_priority<y->render_priority;	}
};

DEFINE_SET_PRED(ESceneCustomMTools*,SceneMToolsSet,SceneMToolsIt,tools_rp_pred);
DEFINE_SET_PRED(ESceneCustomOTools*,SceneOToolsSet,SceneOToolsIt,tools_rp_pred);

void EScene::Render( const Fmatrix& camera )
{
	if( !valid() )	return;
//	if( locked() )	return;

	SetLights();

    // extract and sort object tools
    SceneOToolsSet object_tools;
    SceneMToolsSet scene_tools;
    {
        SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
        SceneToolsMapPairIt t_end 	= m_SceneTools.end();
        for (; t_it!=t_end; t_it++)
            if (t_it->second){
                ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(t_it->second);
                if (mt)           	object_tools.insert(mt);
                else				scene_tools.insert(t_it->second);
            }
    }

    // insert objects
    {
	    SceneOToolsIt t_it	= object_tools.begin();
	    SceneOToolsIt t_end	= object_tools.end();
        for (; t_it!=t_end; t_it++){
            ObjectList& lst = (*t_it)->GetObjects();
            ObjectIt o_it 	= lst.begin();
            ObjectIt o_end 	= lst.end();
            for(;o_it!=o_end;o_it++){
                if( (*o_it)->Visible()&& (*o_it)->IsRender() ){
                    float distSQ = Device.vCameraPosition.distance_to_sqr((*o_it)->FPosition);
                    mapRenderObjects.insertInAnyWay(distSQ,*o_it);
                }
            }
        }
    }
    
// priority #0
    // normal
    mapRenderObjects.traverseLR		(object_Normal_0);
    RENDER_SCENE_TOOLS				(0,false);
    // alpha
    mapRenderObjects.traverseRL		(object_StrictB2F_0);
    RENDER_SCENE_TOOLS				(0,true);
// priority #1
    // normal
    mapRenderObjects.traverseLR		(object_Normal_1);
    RENDER_SCENE_TOOLS				(1,false);
    // alpha
    mapRenderObjects.traverseRL		(object_StrictB2F_1);
    RENDER_SCENE_TOOLS				(1,true);
// priority #2
    // normal
    mapRenderObjects.traverseLR		(object_Normal_2);
    RENDER_SCENE_TOOLS				(2,false);
    // alpha
    mapRenderObjects.traverseRL		(object_StrictB2F_2);
    RENDER_SCENE_TOOLS				(2,true);
// priority #3
    // normal
    mapRenderObjects.traverseLR		(object_Normal_3);
    RENDER_SCENE_TOOLS				(3,false);
    // alpha
    mapRenderObjects.traverseRL		(object_StrictB2F_3);
    RENDER_SCENE_TOOLS				(3,true);

    // render snap
    RenderSnapList			();

    // draw compiler errors
    RenderCompilerErrors	();

    // clear
    mapRenderObjects.clear			();
	ClearLights();
}
//------------------------------------------------------------------------------

void EScene::RenderCompilerErrors()
{
    Device.SetShader		(Device.m_SelectionShader);
    RCache.set_xform_world	(Fidentity);
    Device.RenderNearer		(0.0003f);
    Device.SetRS			(D3DRS_CULLMODE,D3DCULL_NONE);
    AnsiString temp;
    int cnt=0;
    for (ERR::VertIt vit=m_CompilerErrors.m_TJVerts.begin(); vit!=m_CompilerErrors.m_TJVerts.end(); vit++){
        temp.sprintf		("TJ: %d",cnt++);
        DU.dbgDrawVert(vit->p[0],						0xff0000ff,	temp.c_str());
    }
    cnt=0;
    for (ERR::EdgeIt eit=m_CompilerErrors.m_MultiEdges.begin(); eit!=m_CompilerErrors.m_MultiEdges.end(); eit++){
        temp.sprintf		("ME: %d",cnt++);
        DU.dbgDrawEdge(eit->p[0],eit->p[1],				0xff00ff00,	temp.c_str());
    }
    cnt=0;
    for (ERR::FaceIt fit=m_CompilerErrors.m_InvalidFaces.begin(); fit!=m_CompilerErrors.m_InvalidFaces.end(); fit++){
        temp.sprintf		("IF: %d",cnt++);
        DU.dbgDrawFace(fit->p[0],fit->p[1],fit->p[2],	0xffff0000,	temp.c_str());
    }
    Device.SetRS			(D3DRS_CULLMODE,D3DCULL_CCW);
    Device.ResetNearer		();
}
//------------------------------------------------------------------------------
 