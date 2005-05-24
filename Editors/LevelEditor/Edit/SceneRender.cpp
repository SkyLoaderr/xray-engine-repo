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

float NRand(float sigma)
{
#define ONE_OVER_SIGMA_EXP (1.0f / 0.7975f)
	
	if(sigma == 0) return 0;
	
	float y;
	do{
		y = -logf(drand48());
	}while(drand48() > expf(-_sqr(y - 1.0f)*0.5f));
	if(rand() & 0x1)
		return y * sigma * ONE_OVER_SIGMA_EXP;
	else
		return -y * sigma * ONE_OVER_SIGMA_EXP;
}

float NRand1(float sigma)
{
	return Random.randF(Random.randF(-sigma,sigma));
}


void EScene::RenderSky(const Fmatrix& camera)
{
	if( !valid() )	return;

//	draw sky
/*
//.
	if (m_SkyDome&&fraBottomBar->miDrawSky->Checked){
        st_Environment& E = m_LevelOp.m_Envs[m_LevelOp.m_CurEnv];
        m_SkyDome->PPosition = camera.c;
        m_SkyDome->UpdateTransform(true);
		Device.SetRS(D3DRS_TEXTUREFACTOR, E.m_SkyColor.get());
    	m_SkyDome->RenderSingle();
	    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    }
*/
}
//------------------------------------------------------------------------------

struct tools_rp_pred : public std::binary_function<ESceneCustomMTools*, ESceneCustomMTools*, bool>
{
    IC bool operator()(ESceneCustomMTools* x, ESceneCustomMTools* y) const
    {	return x->RenderPriority()<y->RenderPriority();	}
};

#define DEFINE_MSET_PRED(T,N,I,P)	typedef xr_multiset< T, P > N;		typedef N::iterator I;

DEFINE_MSET_PRED(ESceneCustomMTools*,SceneMToolsSet,SceneMToolsIt,tools_rp_pred);
DEFINE_MSET_PRED(ESceneCustomOTools*,SceneOToolsSet,SceneOToolsIt,tools_rp_pred);


void EScene::Render( const Fmatrix& camera )
{
	if( !valid() )	return;

    static U32Vec	cells(101); // -10, 10
    static u32 cnt	= 0;
    static float sigma	= 10.0f/3.1f;
    static bool reinit 	= false;
    if (reinit){
    	reinit = false;
        cells.clear();
    }

    static float mn=flt_max,mx=flt_min;
    for (u32 n=0; n<100; n++){
	 	cnt++;
	   	float val 		= NRand(sigma);
        mn				= _min(val,mn);
        mx				= _max(val,mx);
    }


    Fvector D={0,0,1};
    float alpha			= NRand	(sigma); clamp(alpha,-10.f,10.f);
    int c 				= clampr((iFloor(alpha)+10)*5,0,100);
    cells[c]++;

    Log					("alpha",alpha);
    alpha 				= deg2rad(alpha);
    float theta			= Random.randF(0,PI);
    float r 			= tan	(alpha);
    Fvector 			U,V,T;
    Fvector::generate_orthonormal_basis(D,U,V);
    U.mul				(r*_sin(theta));
    V.mul				(r*_cos(theta));
    T.add				(U,V);
    D.add				(T).normalize();

    static FvectorVec 	m_PTS;
    m_PTS.push_back		(T);
    if (m_PTS.size()>200)m_PTS.clear();
    for (FvectorIt it=m_PTS.begin(); it!=m_PTS.end(); it++)
	    DU.DrawLineSphere	(*it,0.002f,0xFFFF0000,false);
    DU.DrawLineSphere	(Fvector().set(-tan(deg2rad(10.f)),0,0),0.002f,0xFFFF00FF,false);
    DU.DrawLineSphere	(Fvector().set(+tan(deg2rad(10.f)),0,0),0.002f,0xFFFF00FF,false);
    DU.DrawLineSphere	(Fvector().set(0,-tan(deg2rad(10.f)),0),0.002f,0xFFFF00FF,false);
    DU.DrawLineSphere	(Fvector().set(0,+tan(deg2rad(10.f)),0),0.002f,0xFFFF00FF,false);
    
    for (u32 k=0; k<101; k++)
    	DU.DrawLine	(Fvector().set(float(k)/100.f-5.f,EPS_L,0),Fvector().set(float(k)/100.f-5.f,EPS_L,float(cells[k])/float(cnt)*10.f),0xFF00FF00);
    DU.DrawLine			(Fvector().set(0,0,0),Fvector().mad(Fvector().set(0,0,0),D,10.f),0xFF00FF00);
    
//	if( locked() )	return;

    // extract and sort object tools
    SceneOToolsSet object_tools;
    SceneMToolsSet scene_tools;
    {
        SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
        SceneToolsMapPairIt t_end 	= m_SceneTools.end();
        for (; t_it!=t_end; t_it++)
            if (t_it->second){
            	// before render
            	t_it->second->BeforeRender(); 
                // sort tools
                ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(t_it->second);
                if (mt)           	object_tools.insert(mt);
                scene_tools.insert	(t_it->second);
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

    // clear
    mapRenderObjects.clear			();


    SceneMToolsIt s_it 	= scene_tools.begin();
    SceneMToolsIt s_end	= scene_tools.end();
    for (; s_it!=s_end; s_it++) (*s_it)->AfterRender();
}
//------------------------------------------------------------------------------

 
