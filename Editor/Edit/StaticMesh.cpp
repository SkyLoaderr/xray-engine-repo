//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "FileSystem.h"
#include "statistic.h"

#include "UI_Main.h"
#include "cl_RAPID.h"
#include "cl_intersect.h"
#include "StaticMeshChunks.h"

#include "scene.h"
#include <functional>

//----------------------------------------------------
static Fmaterial static_mat={{1.f,1.f,1.f,1.f},
                             {0,0,0,0},
                             {0,0,0,0},
                             {0,0,0,0},
                             0};
void CLSID2TEXT(CLASS_ID id, char *text) {
	text[8]=0;
	for (int i=7; i>=0; i--) { text[i]=char(id&0xff); id>>=8; }
}
CLASS_ID TEXT2CLSID(char *text) {
	char buf[10];
	strncpy(buf,text,9);
	int need = 8-strlen(buf);
	while (need) { buf[8-need]=' '; need--; }
	return MK_CLSID(buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
}
/*
class CmpTFaces
{   int ref;
public:
    CmpTFaces(int a):ref(a) {};
    bool operator() (STFace &f) { return f.GFaceID==ref; }
};
    if (m_TFaces.end() != find_if(m_TFaces.begin(),m_TFaces.end(),CmpTFaces(face_id) ))
        return false;
*/
CTextureLayer::CTextureLayer(Mesh* parent){
    m_Parent    = parent;
    sprintf     (m_LayerName,"Layer %d",parent->m_Layers.size());
    m_SOP       = sopBlend;
    m_Texture   = 0;
    m_CLS       = TEXT2CLSID("STD");
    m_Script[0] = 0;
    m_Mat.diffuse.set(1,1,1,1);
    m_Mat.ambient.set(0,0,0,0);
    m_Mat.specular.set(0,0,0,0);
    m_Mat.emissive.set(0,0,0,0);
    m_Mat.power = 0;
}
//----------------------------------------------------------------------------

CTextureLayer::~CTextureLayer(){
    SAFE_DELETE(m_Texture);
}
//----------------------------------------------------------------------------

bool CTextureLayer::AddFace(DWORD face_id, SUV_tri* uv)
{
    VERIFY(face_id<m_Parent->m_GFaces.size());
    if (m_GFaceIDs.end() != find(m_GFaceIDs.begin(),m_GFaceIDs.end(),face_id)) return false;
    if (uv) m_UVMaps.push_back(SUV_tri((SUV*)uv));
    else    m_UVMaps.push_back(SUV_tri());
    m_GFaceIDs.push_back(face_id);

// update d3d buffer
    Fvertex v;
    int i=m_GFaceIDs.size()-1;
    v.set(  m_Parent->m_Vertices[m_Parent->m_GFaces[face_id].p0].m_Point,
            m_Parent->m_Vertices[m_Parent->m_GFaces[face_id].p0].m_Normal,
            m_UVMaps[i].uv[0].tu, m_UVMaps[i].uv[0].tv);
    m_RenderBuffer.push_back(v);
    v.set(  m_Parent->m_Vertices[m_Parent->m_GFaces[face_id].p1].m_Point,
            m_Parent->m_Vertices[m_Parent->m_GFaces[face_id].p1].m_Normal,
            m_UVMaps[i].uv[1].tu, m_UVMaps[i].uv[1].tv);
    m_RenderBuffer.push_back(v);
    v.set(  m_Parent->m_Vertices[m_Parent->m_GFaces[face_id].p2].m_Point,
            m_Parent->m_Vertices[m_Parent->m_GFaces[face_id].p2].m_Normal,
            m_UVMaps[i].uv[2].tu, m_UVMaps[i].uv[2].tv);
    m_RenderBuffer.push_back(v);

    return true;
}
//----------------------------------------------------------------------------

bool  CTextureLayer::ContainsFace(DWORD face_id){
    return (m_GFaceIDs.end()!=find(m_GFaceIDs.begin(),m_GFaceIDs.end(),face_id));
}
//----------------------------------------------------------------------------
bool  CTextureLayer::DelFace(DWORD face_id)
{
    VERIFY(face_id<m_Parent->m_GFaces.size());
    DWORDIt f = find(m_GFaceIDs.begin(),m_GFaceIDs.end(),face_id);
    if (m_GFaceIDs.end() == f) return false;
    int id = f-m_GFaceIDs.begin();
    m_GFaceIDs.erase(f);
    m_UVMaps.erase(m_UVMaps.begin()+id);
    m_RenderBuffer.erase(m_RenderBuffer.begin()+id*3,m_RenderBuffer.begin()+(id+1)*3);
    return true;
}
//----------------------------------------------------------------------------

void CTextureLayer::UpdateRenderBuffer()
{
    DWORDIt fid;
    Fvertex v;
    m_RenderBuffer.clear();
    int i=0;
    for (fid=m_GFaceIDs.begin(); fid!=m_GFaceIDs.end(); fid++,i++){
        v.set(  m_Parent->m_Vertices[m_Parent->m_GFaces[*fid].p0].m_Point,
                m_Parent->m_Vertices[m_Parent->m_GFaces[*fid].p0].m_Normal,
                m_UVMaps[i].uv[0].tu, m_UVMaps[i].uv[0].tv);
        m_RenderBuffer.push_back(v);
        v.set(  m_Parent->m_Vertices[m_Parent->m_GFaces[*fid].p1].m_Point,
                m_Parent->m_Vertices[m_Parent->m_GFaces[*fid].p1].m_Normal,
                m_UVMaps[i].uv[1].tu, m_UVMaps[i].uv[1].tv);
        m_RenderBuffer.push_back(v);
        v.set(  m_Parent->m_Vertices[m_Parent->m_GFaces[*fid].p2].m_Point,
                m_Parent->m_Vertices[m_Parent->m_GFaces[*fid].p2].m_Normal,
                m_UVMaps[i].uv[2].tu, m_UVMaps[i].uv[2].tv);
        m_RenderBuffer.push_back(v);
    }
}
//----------------------------------------------------------------------------

void CTextureLayer::Copy( CTextureLayer* source, DWORD face_offs )
//копирует в себя исходный слой + если нужно смещает индексы
{
    strcpy(m_LayerName, source->m_LayerName);
	strcpy(m_Script, source->m_Script);
	m_Texture   = (source->m_Texture)?new ETexture( source->m_Texture->name()):NULL;
    m_CLS		= source->m_CLS;
    m_Mat.set	(source->m_Mat);
    m_SOP		= source->m_SOP;

	m_UVMaps.insert		(m_UVMaps.end(),	source->m_UVMaps.begin(),	source->m_UVMaps.end());
	m_GFaceIDs.insert	(m_GFaceIDs.end(),	source->m_GFaceIDs.begin(),	source->m_GFaceIDs.end());
	if (face_offs)
		for (DWORDIt id=m_GFaceIDs.begin(); id!=m_GFaceIDs.end(); id++)	(*id) += face_offs;
}
//----------------------------------------------------------------------------

bool CTextureLayer::Load( FSChunkDef *chunk ){
	char namebuffer[MAX_PATH];
	VERIFY( chunk );
	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	while( FS.rinchunk( chunk ) ){
		FS.ropenchunk( &current );
		switch( current.chunkid ){
		case STM_CHUNK_LAYER_NAME:
			FS.readstring( current.filehandle, m_LayerName, MAX_LAYER_NAME );
			break;
		case STM_CHUNK_LAYER_TNAME:
			FS.readstring( current.filehandle, namebuffer, MAX_PATH );
			m_Texture = new ETexture( namebuffer );
			break;
		case STM_CHUNK_LAYER_TCLASS:
			FS.read     ( current.filehandle, &m_CLS, sizeof(m_CLS) );
			break;
		case STM_CHUNK_LAYER_SCRIPT:
			FS.readstring( current.filehandle, m_Script, MAX_PATH );
			break;
		case STM_CHUNK_LAYER_MATERIAL:
			FS.read     ( current.filehandle, &m_Mat, sizeof(m_Mat) );
			break;
		case STM_CHUNK_LAYER_SHADER_OP:
			FS.read     ( current.filehandle, &m_SOP, sizeof(m_SOP) );
			break;
		case STM_CHUNK_LAYER_UVMAPS:
			m_UVMaps.resize( current.chunksize / sizeof(SUV_tri) );
			FS.read( current.filehandle, m_UVMaps.begin(), current.chunksize );
			break;
		case STM_CHUNK_LAYER_FACE_REFS:
			m_GFaceIDs.resize( current.chunksize / sizeof(DWORD) );
			FS.read( current.filehandle, m_GFaceIDs.begin(), current.chunksize );
			break;
		default:
			Log.Msg( "StaticMesh Load layer: undefined chunk 0x%04X", current.chunkid );
			break; }
		FS.rclosechunk( &current );
	}
	return true;
}

//----------------------------------------------------
Mesh::Mesh()
{
    cf_model = 0;
}
//----------------------------------------------------

Mesh::Mesh(Mesh* source)
{
    cf_model = 0;
    CloneFrom(source);
}
//----------------------------------------------------

Mesh::~Mesh(){
    Clear();
}

void Mesh::Clear(){
	m_Vertices.clear();
	m_GFaces.clear();
    _DELETE(cf_model);

    for(TLayerIt l=m_Layers.begin(); l!=m_Layers.end();l++)
        SAFE_DELETE(*l);
    m_Layers.clear();
}

CTextureLayer* Mesh::AddTextureLayer(ETexture* t)
{
    CTextureLayer* l = new CTextureLayer(this);  VERIFY(l);
    l->m_Texture = t;
    m_Layers.push_back(l);
    return l;
}
//----------------------------------------------------

void Mesh::RemoveTextureLayer(CTextureLayer** remove_layer)
{
    VERIFY(remove_layer);
    TLayerIt remove_layer_it = remove(m_Layers.begin(), m_Layers.end(), *remove_layer);
    m_Layers.erase(remove_layer_it,m_Layers.end());
    SAFE_DELETE(*remove_layer);
}
//----------------------------------------------------

bool Mesh::ContainsLayer(CTextureLayer* l)
{
    VERIFY(l);
    for(TLayerIt l_def=m_Layers.begin(); l_def!=m_Layers.end();l_def++)
        if ((*l_def)==l) return true;
    return false;
}
//----------------------------------------------------

void Mesh::Render( Fmatrix& parent, bool use_material ){
    if( !m_Layers.empty() ){
		CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,parent.d3d()) );

        for(TLayerIt l=m_Layers.begin(); l!=m_Layers.end();l++){
            if (use_material){ CDX( UI.d3d()->SetMaterial( (*l)->m_Mat.d3d() ) );
            }else{             CDX( UI.d3d()->SetMaterial( static_mat.d3d() ));}

            // если пустой пропустить
            if ((*l)->m_RenderBuffer.size()){
                if (l-m_Layers.begin()){
                    // render second layers and higher
                    switch ((*l)->m_SOP) {
                    case sopAdd:
                        UI.D3D_ApplyRenderState(UI.SB_DrawMeshLayerAdd);
                        break;
                    case sopMul:
                        UI.D3D_ApplyRenderState(UI.SB_DrawMeshLayerMul);
                        break;
                    case sopBlend:
                        if (((*l)->m_Texture)&&((*l)->m_Texture->alpha()))  UI.D3D_ApplyRenderState(UI.SB_DrawMeshLayerAlpha);
                        else                                                UI.D3D_ApplyRenderState(UI.SB_DrawMeshLayer);
                        if (!UI.bRenderLights) CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE) );
                        break;
                    default: throw -1;
                    }
                }else{
                    // render first layer
                    if (((*l)->m_Texture)&&((*l)->m_Texture->alpha()))  UI.D3D_ApplyRenderState(UI.SB_DrawMeshLayerAlpha);
                    else                                                UI.D3D_ApplyRenderState(UI.SB_DrawMeshLayer);
                    if (!UI.bRenderLights) CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE) );
                }

                if(UI.bRenderTextures){
                    if((*l)->m_Texture) (*l)->m_Texture->Bind();
                    else                TM.BindNone();
                }else{ TM.BindNone(); }

                CDX( UI.d3d()->DrawPrimitive(
                    D3DPT_TRIANGLELIST,D3DFVF_VERTEX,
                    (*l)->m_RenderBuffer.begin(),
                    (*l)->m_RenderBuffer.size(),
                    0 ));
                UI.stat->lRenderPolyCount += (*l)->m_GFaceIDs.size();
            }
// if multi-texturing disabled exit
            if (!UI.bRenderMultiTex) break;
		}
    }
}
//----------------------------------------------------
void Mesh::RenderEdge( Fmatrix& parent, FPcolor& color, CTextureLayer* layer )
{
	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,parent.d3d()) );
    vector<FPcolor> colors;
    vector<WORD> indices;
    UI.D3D_ApplyRenderState(UI.SB_DrawWire);
    TM.BindNone();
    if (layer){
        if (layer->m_RenderBuffer.size()){
            colors.resize(layer->m_RenderBuffer.size(),color);
            if (!UI.bRenderWire) UI.d3d()->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
            D3DDRAWPRIMITIVESTRIDEDDATA dt;
            dt.position.lpvData = &(layer->m_RenderBuffer.front().p);
            dt.position.dwStride = sizeof(Fvertex);
            dt.diffuse.lpvData = &(colors[0].c);
            dt.diffuse.dwStride = sizeof(FPcolor);
            CDX( UI.d3d()->DrawPrimitiveStrided(
                D3DPT_TRIANGLELIST, D3DFVF_XYZ|D3DFVF_DIFFUSE,
                &dt, layer->m_RenderBuffer.size(),
                0));
            if (!UI.bRenderWire) UI.d3d()->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID);
/*
            colors.resize(m_Vertices.size(),color);
            indices.resize(layer->m_GFaceIDs.size()*3*2);
            int u = indices.size();
            DWORD i = 0;
            for (DWORDIt it=layer->m_GFaceIDs.begin(); it!=layer->m_GFaceIDs.end(); it++,i++){
                indices[i*3*2+0] = m_GFaces[*it].p0;
                indices[i*3*2+1] = m_GFaces[*it].p1;
                indices[i*3*2+2] = m_GFaces[*it].p1;
                indices[i*3*2+3] = m_GFaces[*it].p2;
                indices[i*3*2+4] = m_GFaces[*it].p2;
                indices[i*3*2+5] = m_GFaces[*it].p0;
            }
            D3DDRAWPRIMITIVESTRIDEDDATA dt;
            dt.position.lpvData = &(m_Vertices[0].m_Point.x);
            dt.position.dwStride = sizeof(SPoint);
            dt.diffuse.lpvData = &(colors[0].c);
            dt.diffuse.dwStride = sizeof(FPcolor);
            CDX( UI.d3d()->DrawIndexedPrimitiveStrided(
                D3DPT_LINELIST, D3DFVF_XYZ|D3DFVF_DIFFUSE,
                &dt, m_Vertices.size(),
                &(indices[0]),indices.size(),
                0));
*/        }
    }else{
        colors.resize(m_Vertices.size(),color);
        indices.resize(m_GFaces.size()*3*2);
        for (DWORD i=0; i<m_GFaces.size(); i++){
            indices[i*3*2+0] = m_GFaces[i].p0;
            indices[i*3*2+1] = m_GFaces[i].p1;
            indices[i*3*2+2] = m_GFaces[i].p1;
            indices[i*3*2+3] = m_GFaces[i].p2;
            indices[i*3*2+4] = m_GFaces[i].p2;
            indices[i*3*2+5] = m_GFaces[i].p0;
        }
        D3DDRAWPRIMITIVESTRIDEDDATA dt;
        dt.position.lpvData = &(m_Vertices[0].m_Point.x);
        dt.position.dwStride = sizeof(SPoint);
        dt.diffuse.lpvData = &(colors[0].c);
        dt.diffuse.dwStride = sizeof(FPcolor);
        CDX( UI.d3d()->DrawIndexedPrimitiveStrided(
            D3DPT_LINELIST, D3DFVF_XYZ|D3DFVF_DIFFUSE,
            &dt, m_Vertices.size(),
            &(indices[0]),indices.size(),
            0));
    }
}

bool Mesh::Pick(
	float *distance,
	Fvector& start,
	Fvector& direction,
	Fmatrix& parent, SPickInfo* pinf )
{
    VERIFY(cf_model);
    XRC.RayPick(parent, 1.0f, cf_model, start, direction);
    VERIFY(distance);
    if (XRC.GetMinRayPickDistance(*distance)){
        if (pinf){
            if ((*distance)<pinf->rp_inf.range){
                XRC.GetMinRayPickInfo(&(pinf->rp_inf));
                pinf->obj  = 0;
                pinf->mesh = 0;
                pinf->pt.mul(direction,pinf->rp_inf.range);
                pinf->pt.add(start);
            }
        }

		return true;
    }
	return false;
}

bool Mesh::BoxPick(
	ICullPlane *planes,
	Fmatrix& parent )
{
	for(DWORD i=0;i<m_GFaces.size();i++){
		Fvector p[3];
		for( int k=0;k<3;k++)
			parent.transform( p[k],
				m_Vertices[m_GFaces[i].p[k]].m_Point );
		if( FrustumPickFace(planes, p[0], p[1], p[2] ) )
			return true;
	}
	return false;
}
//----------------------------------------------------

void Mesh::CloneFrom( Mesh *source ){
	VERIFY(source);
    Clear();
    Append(source);
}
//----------------------------------------------------

void Mesh::Append( Mesh *source ){
	_ASSERTE( source );

	vector<SPoint>	apoints;
	vector<SFace>	afaces;

	apoints.insert(apoints.end(),source->m_Vertices.begin(),source->m_Vertices.end() );
	afaces.insert(afaces.end(),source->m_GFaces.begin(),source->m_GFaces.end() );

	for(DWORD i=0; i<afaces.size(); i++)
		for(DWORD k=0;k<3;k++) afaces[i].p[k] += m_Vertices.size();

	CTextureLayer* layer;
	for(TLayerIt l=source->m_Layers.begin(); l!=source->m_Layers.end();l++){
		layer = AddTextureLayer();
		layer->Copy(*l, m_GFaces.size());
	}

	m_Vertices.insert(m_Vertices.end(),apoints.begin(),apoints.end());
	m_GFaces.insert(m_GFaces.end(),afaces.begin(),afaces.end());

	apoints.clear();
	afaces.clear();

    Update();
}
//----------------------------------------------------

void Mesh::Update(){
    for(TLayerIt l=m_Layers.begin();l!=m_Layers.end();l++) (*l)->UpdateRenderBuffer();
    GenerateCFModel();
}
//----------------------------------------------------

void Mesh::Transform( Fmatrix& parent ){
	for( PointIt pt=m_Vertices.begin(); pt!=m_Vertices.end(); pt++){
		parent.transform((*pt).m_Point);
		parent.shorttransform((*pt).m_Normal);
	}
    Update();
}
//----------------------------------------------------

void Mesh::GetBox( IAABox& box ){
	if( m_Vertices.empty() ){
		box.vmin.set(0,0,0);
		box.vmax.set(0,0,0);
		return; }

	box.vmin.set( m_Vertices[0].m_Point );
	box.vmax.set( m_Vertices[0].m_Point );

	for( PointIt pt=m_Vertices.begin()+1; pt!=m_Vertices.end(); pt++){
		box.growrange(
			(*pt).m_Point.x,
			(*pt).m_Point.y,
			(*pt).m_Point.z );
    }
}
//----------------------------------------------------

bool Mesh::LoadMesh( FSChunkDef *chunk ){
	_ASSERTE( chunk );

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );
    DWORD version;

	while( FS.rinchunk( chunk ) ){
		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case STM_CHUNK_VERSION:
			version = FS.readdword( current.filehandle );
            if (version!=STM_CURRENT_MESH_VERSION){
    			Log.Msg( "Mesh: unsuported file version. Mesh don't load.");
                return false;
            }
			break;

		case STM_CHUNK_HEADER:
			break;

		case STM_CHUNK_POINTLIST:
			m_Vertices.resize( current.chunksize / sizeof(SPoint) );
			FS.read( current.filehandle, m_Vertices.begin(), current.chunksize );
			break;

		case STM_CHUNK_FACELIST:
			m_GFaces.resize( current.chunksize / sizeof(SFace) );
			FS.read( current.filehandle, m_GFaces.begin(), current.chunksize );
			break;

		case STM_CHUNK_LAYER:{
        	CTextureLayer* l = new CTextureLayer(this);
			if( !l->Load( &current ) ){
                SAFE_DELETE(l);
				Log.Msg( "StaticMesh: Invalid layer chunk. Mesh don't load." );
                return false;
            }
        	m_Layers.push_back( l );
			}break;

		default:
			Log.Msg( "StaticMesh: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}
    Update();
	return true;
}
//----------------------------------------------------

bool Mesh::Load( char *filename, bool bRealLoad ){
    _ASSERTE( filename );
	char fullname[MAX_PATH];
	strcpy( fullname, filename );
	FS.m_Meshes.Update( fullname );

	int handle = FS.open( fullname );

	if( handle <= 0 ){
		Log.Msg( "Error: can't open mesh file '%s'", fullname );
    	FS.close( handle );
		return false; }

	FSChunkDef current;
	FS.initchunk( &current, handle );

	while( !FS.eof( handle ) ){
		bool undefined_chunk = false;
		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case STM_CHUNK_MESH:
			if( !LoadMesh( &current ) ){
				Log.Msg( "Error: invalid mesh chunk" );
            	FS.close( handle );
                return false;
            }
			break;

		default:
			undefined_chunk = true;
			break;
		}
		if( undefined_chunk )
			Log.Msg( "EScene: undefined chunk 0x%04X", current.chunkid );

		FS.rclosechunk( &current );
	}

	FS.close( handle );

    return true;
}
//----------------------------------------------------

void Mesh::GenerateCFModel( )
{
    _DELETE(cf_model);
    cf_model = new RAPID_model();
    VERIFY(cf_model);
    cf_model->BeginModel();
    for (FaceIt f=m_GFaces.begin(); f!=m_GFaces.end(); f++)
        cf_model->AddTri(   m_Vertices[(*f).p0].m_Point,
                            m_Vertices[(*f).p1].m_Point,
                            m_Vertices[(*f).p2].m_Point, 0);
    cf_model->EndModel();
}
//----------------------------------------------------

void Mesh::SaveMesh( int handle ){
	FSChunkDef layerchunk;
	FSChunkDef current;
	FS.initchunk            ( &current, handle );
	FS.initchunk            ( &layerchunk, handle );

	FS.wopenchunk           ( &current, STM_CHUNK_VERSION );
	FS.writedword           ( handle, STM_CURRENT_MESH_VERSION );
	FS.wclosechunk          ( &current );

	FS.wopenchunk           ( &current, STM_CHUNK_HEADER );
	FS.wclosechunk          ( &current );

	FS.wopenchunk           ( &current, STM_CHUNK_POINTLIST );
	FS.write                ( handle, m_Vertices.begin(), m_Vertices.size()*sizeof(SPoint));
	FS.wclosechunk          ( &current );

	FS.wopenchunk           ( &current, STM_CHUNK_FACELIST );
	FS.write                ( handle, m_GFaces.begin(), m_GFaces.size()*sizeof(SFace));
	FS.wclosechunk          ( &current );

// Layers
    for(TLayerIt l=m_Layers.begin(); l!=m_Layers.end();l++){
		FS.wopenchunk       ( &current, STM_CHUNK_LAYER );
// layer name
		FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_NAME );
    	FS.writestring      ( handle, (*l)->m_LayerName );
		FS.wclosechunk      ( &layerchunk );
// texture name
		if( (*l)->m_Texture ){
			FS.wopenchunk   ( &layerchunk, STM_CHUNK_LAYER_TNAME );
			FS.writestring  ( handle, (*l)->m_Texture->name() );
			FS.wclosechunk  ( &layerchunk );
		}
// texture class
		FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_TCLASS );
    	FS.write            ( handle, &((*l)->m_CLS), sizeof((*l)->m_CLS) );
		FS.wclosechunk      ( &layerchunk );
// script
		FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_SCRIPT );
		FS.writestring      ( handle, (*l)->m_Script );
		FS.wclosechunk      ( &layerchunk );
// material
		FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_MATERIAL );
		FS.write            ( handle, &((*l)->m_Mat), sizeof((*l)->m_Mat) );
		FS.wclosechunk      ( &layerchunk );
// shader op
		FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_SHADER_OP );
		FS.write            ( handle, &((*l)->m_SOP), sizeof((*l)->m_SOP) );
		FS.wclosechunk      ( &layerchunk );
// layer uv-maps
		FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_UVMAPS );
    	FS.write            ( handle, (*l)->m_UVMaps.begin(), (*l)->m_UVMaps.size()*sizeof(SUV_tri));
		FS.wclosechunk      ( &layerchunk );
// layer geom face refs
		FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_FACE_REFS );
    	FS.write            ( handle, (*l)->m_GFaceIDs.begin(), (*l)->m_GFaceIDs.size()*sizeof(DWORD));
		FS.wclosechunk      ( &layerchunk );

		FS.wclosechunk( &current );
    }
}







/*void Mesh::Convert()
{
//    vector<SScript>    scripts;
    // vertices
    vector<MPoint>::iterator Vsrc;
    for(Vsrc=m_Points.begin(); Vsrc!=m_Points.end();Vsrc++){
        m_Vertices.push_back(SPoint());
        m_Vertices.back().m_Point.set(Vsrc->m_Point);
        m_Vertices.back().m_Normal.set(Vsrc->m_Normal);
    }
    // faces
    vector<MFace>::iterator Fsrc;
    for(Fsrc=m_Faces.begin(); Fsrc!=m_Faces.end();Fsrc++){
        m_GFaces.push_back(SFace());
        CopyMemory(m_GFaces.back().p,Fsrc->p,sizeof(MFace));
    }
    // materials
    vector<MMaterial>::iterator m;
    for(m=m_Materials.begin(); m!=m_Materials.end();m++){
        CTextureLayer* l = AddTextureLayer(m->m_Texture);
        l->m_Mat.diffuse.set(m->m_Diffuse.r, m->m_Diffuse.g, m->m_Diffuse.b, m->m_Diffuse.a);
        l->m_Mat.ambient.set(m->m_Ambient.r, m->m_Ambient.g, m->m_Ambient.b, m->m_Ambient.a);
        l->m_Mat.specular.set(m->m_Specular.r, m->m_Specular.g, m->m_Specular.b, m->m_Specular.a);
        l->m_Mat.emissive.set(m->m_Emission.r, m->m_Emission.g, m->m_Emission.b, m->m_Emission.a);
        l->m_Mat.power = m->m_Power;
        // tfaces
        int st = m->m_FaceStart;
        for(DWORD ti=0; ti<m->m_FaceCount;ti++){
            SUV_tri uv;
            uv.uv[0].tu=m_Points[m_Faces[st+ti].p0].m_TexCoord.x;
            uv.uv[0].tv=m_Points[m_Faces[st+ti].p0].m_TexCoord.y;
            uv.uv[1].tu=m_Points[m_Faces[st+ti].p1].m_TexCoord.x;
            uv.uv[1].tv=m_Points[m_Faces[st+ti].p1].m_TexCoord.y;
            uv.uv[2].tu=m_Points[m_Faces[st+ti].p2].m_TexCoord.x;
            uv.uv[2].tv=m_Points[m_Faces[st+ti].p2].m_TexCoord.y;
            l->AddFace(st+ti, &uv);
        }
    }
}
*/
//----------------------------------------------------

