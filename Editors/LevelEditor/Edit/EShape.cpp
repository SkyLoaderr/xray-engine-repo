//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "EShape.h"
#include "D3DUtils.h"
#include "render.h"
#include "du_box.h"
#include "Scene.h"

#define SHAPE_COLOR_TRANSP		0x1800FF00
#define SHAPE_COLOR_EDGE		0xFF202020
//---------------------------------------------------------------------------

#define SHAPE_CURRENT_VERSION 	0x0001
//---------------------------------------------------------------------------
#define SHAPE_CHUNK_VERSION 	0x0000
#define SHAPE_CHUNK_SHAPES 		0x0001
//---------------------------------------------------------------------------

CEditShape::CEditShape(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}

CEditShape::~CEditShape()
{
}

void CEditShape::Construct(LPVOID data)
{
	ClassID		= OBJCLASS_SHAPE;
	m_Box.invalidate();
}

void CEditShape::OnUpdateTransform()
{
	inherited::OnUpdateTransform();
}

void CEditShape::ComputeBounds()
{
	m_Box.invalidate	();

	for (ShapeIt it=m_Shapes.begin(); it!=m_Shapes.end(); it++){
		switch (it->type){
		case stSphere:{
            Fsphere&	T		= it->data.sphere;
            Fvector		P;
            P.set		(T.P);	P.sub(T.R);	m_Box.modify(P);
            P.set		(T.P);	P.add(T.R);	m_Box.modify(P);
		}break;
		case stBox:{
            Fvector		P;
            Fmatrix&	T		= it->data.box;
				
            // Build points
            Fvector p;
            for (int i=0; i<DU_BOX_NUMVERTEX; i++){
                T.transform_tiny	(P,du_box_vertices[i]); 
                m_Box.modify		(P);
            }
		}break;
		}
	}
	m_Box.getsphere(m_Sphere.P,m_Sphere.R);
}

void CEditShape::ScaleShapes(const Fvector& val)
{
	Fmatrix M;
    M.scale(val);
	for (ShapeIt it=m_Shapes.begin(); it!=m_Shapes.end(); it++){
		switch (it->type){
		case stSphere:{
            Fsphere&	T	= it->data.sphere;
        	if (!fis_zero(val.x))		T.R *= val.x;
        	else if (!fis_zero(val.y))	T.R *= val.y;
        	else if (!fis_zero(val.z))	T.R *= val.z;
            M.transform_tiny(T.P);
		}break;
		case stBox:{
            Fmatrix& B		= it->data.box;
            B.mulA			(M);
		}break;
        default: THROW;
		}
    }
    
	ComputeBounds();
}

void CEditShape::add_sphere(const Fsphere& S)
{
	m_Shapes.push_back(shape_def());
	m_Shapes.back().type	= stSphere;
	m_Shapes.back().data.sphere.set(S);
    
	ComputeBounds();
}

void CEditShape::add_box(const Fmatrix& B)
{
	m_Shapes.push_back(shape_def());
	m_Shapes.back().type	= stBox;
	m_Shapes.back().data.box.set(B);
    
	ComputeBounds();
}

void CEditShape::Attach(CEditShape* from)
{
	// transfer data
	Fmatrix M = from->_Transform();
    M.mulA(_ITransform());
	for (ShapeIt it=from->m_Shapes.begin(); it!=from->m_Shapes.end(); it++){
		switch (it->type){
		case stSphere:{
            Fsphere&	T	= it->data.sphere;
            M.transform_tiny(T.P);
            add_sphere		(T);
		}break;
		case stBox:{
            Fmatrix B		= it->data.box;
            B.mulA			(M);
            add_box			(B);
		}break;
        default: THROW;
		}
    }
    // common 
    Scene.RemoveObject(from,true);
    _DELETE		(from);
    
	ComputeBounds();
}

void CEditShape::Dettach()
{
	if (m_Shapes.size()>1){
    	Select			(true);
        // create scene shapes
        const Fmatrix& M = _Transform();
        ShapeIt it=m_Shapes.begin(); it++;
        for (; it!=m_Shapes.end(); it++){
            string256 namebuffer;
            Scene.GenObjectName	(OBJCLASS_SHAPE, namebuffer, Name);
            CEditShape* shape 	= (CEditShape*)NewObjectFromClassID(OBJCLASS_SHAPE, 0, namebuffer);
            switch (it->type){
            case stSphere:{
                Fsphere	T		= it->data.sphere;
                M.transform_tiny(T.P);
                shape->PPosition= T.P;
                T.P.set			(0,0,0);
                shape->add_sphere(T);
            }break;
            case stBox:{
                Fmatrix B		= it->data.box;
                B.mulA			(M);
                shape->PPosition= B.c;
                B.c.set			(0,0,0);
                shape->add_box	(B);
            }break;
            default: THROW;
            }
            Scene.AddObject		(shape,false);
	    	shape->Select		(true);
        }
        // erase shapes in base object 
        it=m_Shapes.begin(); it++;
        m_Shapes.erase(it,m_Shapes.end());
    
        ComputeBounds();

        Scene.UndoSave();
    }
}

bool CEditShape::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf)
{
    Fvector S,D;
    const Fmatrix& M	= _ITransform();
    M.transform_tiny	(S,start);
    M.transform_dir		(D,direction);
    bool bPick			= FALSE;
	
	for (ShapeIt it=m_Shapes.begin(); it!=m_Shapes.end(); it++){
		switch (it->type){
		case stSphere:{
            Fsphere&	T		= it->data.sphere;
            if (T.intersect(S,D,distance)) bPick=TRUE;
		}break;
		case stBox:{
        	Fbox box;
            box.identity		();
            Fmatrix& B			= it->data.box;
		    Fvector S1,D1,P;
		    B.transform_tiny	(S1,S);
		    B.transform_dir		(D1,D);
            if (box.Pick2		(S1,D1,P)){
	            P.sub			(S);
    	        float dist		= P.magnitude();
                if (dist<distance){
                	distance	= dist;
                    bPick		= true;
                }
            }
		}break;
		}
    }
	return bPick;
}

bool CEditShape::FrustumPick(const CFrustum& frustum)
{
	const Fmatrix& M	= _Transform();
    Fvector 			C;
	M.transform_tiny	(C,m_Sphere.P);
	if (!frustum.testSphere_dirty(C,m_Sphere.R)) return false;
	for (ShapeIt it=m_Shapes.begin(); it!=m_Shapes.end(); it++){
		switch (it->type){
		case stSphere:{
            Fsphere&	T	= it->data.sphere;
		    M.transform_tiny(C,T.P);
        	if (frustum.testSphere_dirty(C,T.R)) return true;
		}break;
		case stBox:{
        	Fbox 			box;
            box.identity	();
            Fmatrix B		= it->data.box;
            B.mulA	 		(_Transform());
            box.xform		(B);
			u32 mask		= 0xff;
            if (frustum.testAABB(box.min,box.max,mask)) return true;
		}break;
		}
    }
	return false;
}

bool CEditShape::GetBox(Fbox& box)
{
	if (m_Box.is_valid()){
    	box.xform(m_Box,FTransformRP);
    	return true;
    }
	return false;
}

bool CEditShape::Load(CStream& F)
{
	R_ASSERT(F.FindChunk(SHAPE_CHUNK_VERSION));
    u16 vers		= F.Rword();
	if (SHAPE_CURRENT_VERSION!=vers){
		ELog.DlgMsg( mtError, "CEditShape: unsupported version. Object can't load.");
    	return false;
    }
	inherited::Load	(F);

	R_ASSERT(F.FindChunk(SHAPE_CHUNK_SHAPES));
    m_Shapes.resize	(F.Rdword());
    F.Read			(m_Shapes.begin(),m_Shapes.size()*sizeof(shape_def));

	ComputeBounds();
	return true;
}

void CEditShape::Save(CFS_Base& F)
{
	inherited::Save	(F);
    
	F.open_chunk	(SHAPE_CHUNK_VERSION);
	F.Wword			(SHAPE_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(SHAPE_CHUNK_SHAPES);
    F.Wdword		(m_Shapes.size());
    F.write			(m_Shapes.begin(),m_Shapes.size()*sizeof(shape_def));
	F.close_chunk	();
}

void CEditShape::FillProp(LPCSTR pref, PropValueVec& values)
{
	inherited::FillProp(pref,values);
}

void CEditShape::Render(int priority, bool strictB2F)
{
	inherited::Render(priority, strictB2F);
    if (1==priority){
		Fbox bb; GetBox(bb);
		if (::Render->occ_visible(bb)){
            if (strictB2F){
				Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
                for (ShapeIt it=m_Shapes.begin(); it!=m_Shapes.end(); it++){
					switch(it->type){
                    case stSphere:{
		                Device.SetTransform	(D3DTS_WORLD,_Transform());
                    	Fsphere& S			= it->data.sphere;
		                Device.SetShader	(Device.m_SelectionShader);
                        DU::DrawSphere		(S.P,S.R,SHAPE_COLOR_TRANSP);
		                Device.SetShader	(Device.m_WireShader);
                        DU::DrawLineSphere	(S.P,S.R,SHAPE_COLOR_EDGE,true);
                    }break;
                    case stBox:		
                    	Fmatrix B			= it->data.box;
                        B.mulA				(_Transform());
		                Device.SetTransform	(D3DTS_WORLD,B);
		                Device.SetShader	(Device.m_SelectionShader);
				        DU::DrawIdentBox	(true,false,SHAPE_COLOR_TRANSP);
		                Device.SetShader	(Device.m_WireShader);
				        DU::DrawIdentBox	(false,true,SHAPE_COLOR_EDGE);
                    break;
				    }
                }
				Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
            }else{   
                if( Selected()&&m_Box.is_valid() ){
                    DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
	                Device.SetTransform	(D3DTS_WORLD,_Transform());
                    Device.SetShader(Device.m_WireShader);
                    DU::DrawSelectionBox(m_Box,&clr);
                }
            }
        }
    }
}

void CEditShape::OnFrame()
{
	inherited::OnFrame();
}

void CEditShape::OnShowHint(AStringVec& dest)
{
}

