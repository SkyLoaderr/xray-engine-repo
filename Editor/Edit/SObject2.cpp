//----------------------------------------------------
// file: SObject2.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "SObject2.h"
#include "SceneClassList.h"
#include "Library.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "Primitives.h"
#include "Scene.h"
#include "StaticMesh.h"
#include "VisibilityTester.h"

//----------------------------------------------------
#define SOBJ_CHUNK_CLASS_NAME    0xA911
#define SOBJ_CHUNK_CLASS_SCRIPT  0xA912
#define SOBJ_CHUNK_POSITION      0xA913
#define SOBJ_CHUNK_REFERENCE     0xA914
#define SOBJ_CHUNK_MESH          0xA915
#define   SOBJ_CHUNK_MESH_NAME   0xA921
#define   SOBJ_CHUNK_MESH_FILE   0xA922
#define   SOBJ_CHUNK_MESH_POS    0xA923
#define   SOBJ_CHUNK_MESH_CORE   0xA924
#define   SOBJ_CHUNK_MESH_OPS    0xA926
#define SOBJ_CHUNK_BOP           0xA917
//----------------------------------------------------

// mimimal bounding box size
float g_MinBoxSize = 0.05f;

void SObject2Mesh::FullCopy( const SObject2Mesh* source ){
    VERIFY(source);
    strcpy( m_Name, source->m_Name );
    strcpy( m_FileName, source->m_FileName );
    m_Position.set( (Fmatrix &)source->m_Position );
    SAFE_DELETE(m_Mesh);
    m_Mesh = new Mesh(source->m_Mesh);
}
//----------------------------------------------------

SObject2::SObject2( char *name, bool bLib ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
    bLibItem = bLib;
}

SObject2::SObject2( bool bLib ):SceneObject(){
	Construct();
    bLibItem = bLib;
}

SObject2::SObject2(SObject2* source):SceneObject((SceneObject*)source)
{
    CloneFrom(source);
}

void SObject2::CloneFrom(SObject2* source)
{
    VERIFY(source);
	m_OGFCreated = source->m_OGFCreated;
	strcpy(m_OGFName, source->m_OGFName);
	m_MakeUnique = source->m_MakeUnique;
	m_DynamicList= source->m_DynamicList;
	m_Position.set(source->m_Position);
	m_Box.vmin.set(source->m_Box.vmin);
	m_Box.vmax.set(source->m_Box.vmax);
    m_fRadius = source->m_fRadius;
    m_vDeltaOffs.set(source->m_vDeltaOffs);
    m_Center.set(source->m_Center);
	m_LibRef = source->m_LibRef;
    bLibItem = source->bLibItem;

	strcpy(m_ClassName,source->m_ClassName);
	strcpy(m_ClassScript,source->m_ClassScript);

    ClearMeshes();
    CopyMeshes(source);
}

bool SObject2::ContainsMesh( SObject2Mesh* m )
{
    VERIFY(m);
    for(SObjMeshIt m_def = m_Meshes.begin();m_def!=m_Meshes.end();m_def++)
        if (m==(*m_def)) return true;
    return false;
}

void SObject2::ClearMeshes (){
// clear all previous created meshes
    for(SObjMeshIt m_src = m_Meshes.begin();m_src!=m_Meshes.end();m_src++) _DELETE(*m_src);
	m_Meshes.clear();
}

void SObject2::CopyMeshes (SObject2* source){
// full copy meshes from source object
    for(SObjMeshIt m = source->m_Meshes.begin();m!=source->m_Meshes.end();m++)
        m_Meshes.push_back(new SObject2Mesh(*m));
}

void SObject2::Construct(){
	m_ClassID = OBJCLASS_SOBJECT2;

    m_FolderName="";

	m_ClassName[0] = 0;
	m_ClassScript[0] = 0;
	m_Position.identity();
    m_Center.set(0,0,0);

	m_MakeUnique = false;
	m_DynamicList = false;

	m_OGFCreated = false;

	m_LibRef = 0;

	m_Box.vmin.set( 0, 0, 0 );
	m_Box.vmax.set( 0, 0, 0 );

    m_fRadius = 0;
    m_vDeltaOffs.set( 0, 0, 0 );
}

SObject2::~SObject2(){
    ClearMeshes();
}

//----------------------------------------------------
int SObject2::GetFaceCount()
{
    if( m_Meshes.empty() ){
        return 0;
    } else {
        SObjMeshIt m = m_Meshes.begin();
        float cnt=0;
        for(;m!=m_Meshes.end();m++)
            cnt += (*m)->m_Mesh->GetFaceCount();
        return cnt;
    }
}

int SObject2::GetVertexCount()
{
    if( m_Meshes.empty() ){
        return 0;
    } else {
        SObjMeshIt m = m_Meshes.begin();
        float cnt=0;
        for(;m!=m_Meshes.end();m++)
            cnt += (*m)->m_Mesh->GetVertexCount();
        return cnt;
    }
}

int SObject2::GetLayerCount()
{
    if( m_Meshes.empty() ){
        return 0;
    } else {
        SObjMeshIt m = m_Meshes.begin();
        float cnt=0;
        for(;m!=m_Meshes.end();m++)
            cnt += (*m)->m_Mesh->GetLayerCount();
        return cnt;
    }
}

void SObject2::UpdateClassScript(){
	char *p = m_ClassScript;
	while( *p ){
		if( *p == '\\' ){
			if( *(p+1) == 'n' ){
				*p++ = '\r';
				*p++ = '\n';
			} else {
				p++;
			}
		} else {
			p++;
		}
	}
}

void SObject2::UpdateBox(){
    UI.UpdateScene();
	if( IsReference() ){
		m_LibRef->UpdateBox();
		m_Box.set( m_LibRef->m_Box );
	} else {
		if( m_Meshes.empty() ){
			m_Box.set( m_Position.c, m_Position.c );
			m_Box.grow( g_MinBoxSize );
		} else {
			bool boxdefined = false;
			SObjMeshIt m = m_Meshes.begin();
			for(;m!=m_Meshes.end();m++){

				IAABox meshbox;
				(*m)->m_Mesh->GetBox( meshbox );

				if( boxdefined ){
					for( int i=0; i<8; i++){
						Fvector pt;
						meshbox.getpoint( i, pt );
						(*m)->m_Position.transform(pt);
						m_Box.growrange(pt.x,pt.y,pt.z);
					}
				} else {
					Fvector pt;
					meshbox.getpoint( 0, pt );
					(*m)->m_Position.transform(pt);
					m_Box.set(pt,pt);
					boxdefined = true;
					for( int i=1; i<8; i++){
						meshbox.getpoint( i, pt );
						(*m)->m_Position.transform(pt);
						m_Box.growrange(pt.x,pt.y,pt.z);
					}
				}
			}
		}
	}
    m_Box.getsize(m_vDeltaOffs);
    m_fRadius = m_vDeltaOffs.magnitude()/2;
    m_vDeltaOffs.mul(0.5f);
    m_vDeltaOffs.add(m_Box.vmin);
    m_Center.add(*((Fvector*)&m_Position.c),*((Fvector*)&m_vDeltaOffs));
//    m_vDeltaOffs.sub(m_Position.c);
}
//----------------------------------------------------

bool SObject2::GetBox( IAABox& box ){
	UpdateBox();
	Fvector pt;
	m_Box.getpoint( 0, pt );
	m_Position.transform( pt );
	box.set(pt,pt);
	for( int i=1; i<8; i++){
		m_Box.getpoint( i, pt );
		m_Position.transform( pt );
		box.growrange( pt.x, pt.y, pt.z );
	}
	return true;
}

void SObject2::Render( Fmatrix& parent ){
    if (bLibItem || UI.frustum->visibleSphere(m_Center,m_fRadius)){
        if (!bLibItem) Scene.TurnLightsForObject(this);
        Fmatrix matrix;
        if( IsReference() ){
            matrix.mul( parent, m_Position );
            m_LibRef->Render( matrix );
        } else {
            SObjMeshIt m = m_Meshes.begin();
            for(;m!=m_Meshes.end();m++){
                matrix.mul( parent, m_Position );
                matrix.mul2( (*m)->m_Position );
                (*m)->m_Mesh->Render( matrix, !!(m_DynamicList) );
            }
        }

        if( Selected() ){
            matrix.mul( parent, m_Position );
            CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,matrix.d3d()) );
            DrawSelectionBox( m_Box );
        }
    }
}

void SObject2::RenderEdge( Fmatrix& parent, SObject2Mesh* m, CTextureLayer* l ){
    VERIFY(m);
    if (bLibItem || UI.frustum->visibleSphere(m_Center,m_fRadius)){
        Fmatrix matrix;
        if( IsReference() ){
            matrix.mul( parent, m_Position );
            m_LibRef->RenderEdge( matrix, m, l );
        } else {
            matrix.mul( parent, m_Position );
            matrix.mul2( m->m_Position );
            FPcolor c;
            if (l) c.set(0,0,255,255); else c.set(192,192,192,255);
            m->m_Mesh->RenderEdge( matrix, c, l );
        }
    }
}

void SObject2::RTL_Update( float dT ){
    UI.UpdateScene();
}

bool SObject2::BoxPick(
	ICullPlane *planes,
	Fmatrix& parent )
{
    UI.UpdateScene();

	Fmatrix matrix;

	if( IsReference() ){
		matrix.mul( parent, m_Position );
		return m_LibRef->BoxPick( planes, matrix );
	}

	SObjMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
		matrix.mul( parent, m_Position );
		matrix.mul2( (*m)->m_Position );
		if( (*m)->m_Mesh->BoxPick( planes, matrix ) )
			return true;
	}

	return false;
}

bool SObject2::RTL_Pick(
	float *distance,
	Fvector& start,
	Fvector& direction,
	Fmatrix& parent, SPickInfo* pinf )
{
    UI.UpdateScene();
	Fmatrix matrix;
	bool picked = false;
	float min_dist = 0;

	if( IsReference() ){
		matrix.mul( parent, m_Position );
		picked = m_LibRef->RTL_Pick( &min_dist, start, direction, matrix, pinf );
	} else {
		float dist = 0;
		SObjMeshIt m = m_Meshes.begin();
		for(;m!=m_Meshes.end();m++){

			matrix.mul( parent, m_Position );
			matrix.mul2( (*m)->m_Position );

			if( (*m)->m_Mesh->Pick( &dist, *((Fvector*)&start), *((Fvector*)&direction), *((Fmatrix*)&matrix), pinf ) ){
                if (pinf){
                    pinf->obj = this;
                    pinf->mesh = (*m);
                }
				if( picked ){
					min_dist = min( dist, min_dist );
				} else {
					picked = true;
					min_dist = dist;
				}
			}
		}
	}

	if( picked ){
		(*distance) = min_dist;
		return true; }

	return false;
}

void SObject2::Move( Fvector& amount ){
    UI.UpdateScene();
	m_Position.c.add( amount );
    m_Center.add(*((Fvector*)&amount));
}

void SObject2::Rotate( Fvector& center, Fvector& axis, float angle ){
    UI.UpdateScene();
	Fmatrix m;
	m.rotation( axis, angle );

	m_Position.c.sub( center );
	m.shorttransform( m_Position.c );
	m_Position.c.add( center );

	m.shorttransform( m_Position.i );
	m.shorttransform( m_Position.j );
	m.shorttransform( m_Position.k );
}

void SObject2::LocalRotate( Fvector& axis, float angle ){
    UI.UpdateScene();

	Fmatrix m;
	m.rotation( axis, angle );
	m.shorttransform( m_Position.i );
	m.shorttransform( m_Position.j );
	m.shorttransform( m_Position.k );
}

void SObject2::Scale( Fvector& center, Fvector& amount ){
    UI.UpdateScene();

	Fmatrix m;
	m.scale( amount );
	m_Position.c.sub( center );
	m.shorttransform( m_Position.c );
	m_Position.c.add( center );
}

void SObject2::LocalScale( Fvector& amount ){
    UI.UpdateScene();

	Fmatrix m;
	m.scale( amount );
	m.shorttransform( m_Position.i );
	m.shorttransform( m_Position.j );
	m.shorttransform( m_Position.k );
}

void SObject2::LoadMeshDef( FSChunkDef *chunk ){

	_ASSERTE( chunk );

	m_Meshes.push_back(new SObject2Mesh());
	m_Meshes.back()->m_Mesh=new Mesh();

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case SOBJ_CHUNK_MESH_NAME:
			FS.readstring(current.filehandle,
				m_Meshes.back()->m_Name,
				sizeof(m_Meshes.back()->m_Name) );
			break;

		case SOBJ_CHUNK_MESH_FILE:
			FS.readstring(current.filehandle,
				m_Meshes.back()->m_FileName,
				sizeof(m_Meshes.back()->m_FileName) );
			break;

		case SOBJ_CHUNK_MESH_POS:
			FS.read(current.filehandle,
				& m_Meshes.back()->m_Position,
				sizeof(m_Meshes.back()->m_Position) );
			break;

		case SOBJ_CHUNK_MESH_CORE:
			m_Meshes.back()->m_Mesh->LoadMesh( &current );
			break;
		}


		FS.rclosechunk( &current );
	}

}

void SObject2::Load( FSChunkDef *chunk ){

	_ASSERTE( chunk );

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	char refbuffer[MAX_OBJ_NAME]="";

	FS.readstring( current.filehandle, m_Name, sizeof(m_Name) );

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case SOBJ_CHUNK_CLASS_NAME:
			FS.readstring(current.filehandle,m_ClassName,sizeof(m_ClassName));
			break;

		case SOBJ_CHUNK_CLASS_SCRIPT:
			FS.readstring(current.filehandle,m_ClassScript,sizeof(m_ClassScript));
			break;

		case SOBJ_CHUNK_POSITION:
			FS.read(current.filehandle,&m_Position,sizeof(m_Position));
			break;

		case SOBJ_CHUNK_BOP:
			m_MakeUnique = FS.readword(current.filehandle);
			m_DynamicList = FS.readword(current.filehandle);
			break;

		case SOBJ_CHUNK_REFERENCE:
			_ASSERTE( m_LibRef == 0 && m_Meshes.size()==0 );
			FS.readstring(chunk->filehandle,refbuffer,sizeof(refbuffer));
			if( 0==(m_LibRef = (SObject2*)Lib.SearchObject(ClassID(),refbuffer)) ){
				Log.Msg( "SObject2: '%s' not found in library", refbuffer );
            }
			break;

		case SOBJ_CHUNK_MESH:
			_ASSERTE( m_LibRef == 0 );
			LoadMeshDef( &current );
			break;

		default:
			Log.Msg( "SObject2: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}

	UpdateBox();
}

void SObject2::Save( int handle ){

	FSChunkDef chunk_level0;
	FSChunkDef chunk_level1;
	FS.initchunk(&chunk_level0,handle);
	FS.initchunk(&chunk_level1,handle);

	FS.writestring( handle, m_Name );

	FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_CLASS_NAME);
	FS.writestring(handle,m_ClassName);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_CLASS_SCRIPT);
	FS.writestring(handle,m_ClassScript);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_POSITION);
	FS.write(handle,&m_Position,sizeof(m_Position));
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_BOP);
	FS.writeword(handle,m_MakeUnique);
	FS.writeword(handle,m_DynamicList);
	FS.wclosechunk(&chunk_level0);

	if( IsReference() ){

		FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_REFERENCE);
		FS.writestring(handle,m_LibRef->m_Name);
		FS.wclosechunk(&chunk_level0);

	} else {

		SObjMeshIt m = m_Meshes.begin();
		for(;m!=m_Meshes.end();m++){
			FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_MESH);

				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_NAME);
				FS.writestring(handle,(*m)->m_Name);
				FS.wclosechunk(&chunk_level1);

				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_FILE);
				FS.writestring(handle,(*m)->m_FileName);
				FS.wclosechunk(&chunk_level1);

				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_POS);
				FS.write(handle,&(*m)->m_Position,sizeof((*m)->m_Position));
				FS.wclosechunk(&chunk_level1);

				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_CORE);
				(*m)->m_Mesh->SaveMesh( handle );
				FS.wclosechunk(&chunk_level1);

			FS.wclosechunk(&chunk_level0);
		}

	}
}

//----------------------------------------------------

void SObject2::CloneFromLib( SceneObject *source ){
	_ASSERTE( source );
	_ASSERTE( source->ClassID() == ClassID() );
	SObject2 *obj = (SObject2*)source;
    CloneFrom(obj);
    bLibItem = false;
}

void SObject2::ResolveReference() {
	if( IsReference() ){
        Fmatrix m_Pos;   m_Pos.set(m_Position);
        CloneFrom(m_LibRef);
        m_Position.set(m_Pos);
		m_LibRef = 0;
        bLibItem = false;
		UpdateBox();
	}
}

void SObject2::TranslateToWorld() {
    UI.UpdateScene();
	_ASSERTE( !IsReference() );
	SObjMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
		(*m)->m_Mesh->Transform( (*m)->m_Position );
		(*m)->m_Mesh->Transform( m_Position );
		(*m)->m_Position.identity();
	}
	m_Position.identity();
	UpdateBox();
}

void SObject2::LibReference( SceneObject *source ){

	_ASSERTE( source );
	_ASSERTE( source->ClassID() == ClassID() );

	SObject2 *obj = (SObject2*)source;
	m_LibRef = obj;

	strcpy( m_ClassName, obj->m_ClassName );
	strcpy( m_ClassScript, obj->m_ClassScript );
	m_Position.identity();
	m_Meshes.clear();

	m_DynamicList = obj->m_DynamicList;
	m_MakeUnique = obj->m_MakeUnique;

	UpdateBox();
}

