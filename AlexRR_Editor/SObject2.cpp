//----------------------------------------------------
// file: SObject2.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "SObject2.h"
#include "SceneClassList.h"
#include "Library.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "Primitives.h"
#include "Landscape.h"
#include "Texture.h"

//----------------------------------------------------

// mimimal bounding box size
float g_MinBoxSize = 0.05f;



SObject2::SObject2( char *name )
	:SceneObject()
{
	Construct();
	strcpy( m_Name, name );
}

SObject2::SObject2()
	:SceneObject()
{
	Construct();
}

void SObject2::Construct(){

	m_ClassID = OBJCLASS_SOBJECT2;

	m_ClassName[0] = 0;
	m_ClassScript[0] = 0;
	m_Position.identity();
	
	m_MakeUnique = false;
	m_DynamicList = false;

	m_OGFCreated = false;

	m_LibRef = 0;

	m_Box.vmin.set( 0, 0, 0 );
	m_Box.vmax.set( 0, 0, 0 );
}

SObject2::~SObject2(){
	m_Meshes.clear();
}

//----------------------------------------------------

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
				(*m).m_Mesh->GetBox( meshbox );

				if( boxdefined ){
					for( int i=0; i<8; i++){
						IVector pt;
						meshbox.getpoint( i, pt );
						(*m).m_Position.transform(pt);
						m_Box.growrange(pt.x,pt.y,pt.z);
					}
				} else {
					IVector pt;
					meshbox.getpoint( 0, pt );
					(*m).m_Position.transform(pt);
					m_Box.set(pt,pt);
					boxdefined = true;
					for( int i=1; i<8; i++){
						meshbox.getpoint( i, pt );
						(*m).m_Position.transform(pt);
						m_Box.growrange(pt.x,pt.y,pt.z);
					}
				}
			}
		}
	}
}

void SObject2::FillD3DPoints(){
	SObjMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++)
		m->m_Mesh->FillD3DPoints();
}

//----------------------------------------------------

bool SObject2::GetBox( IAABox& box ){
	UpdateBox();
	IVector pt;
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

void SObject2::Render( IMatrix& parent ){
	IMatrix matrix;
	if( IsReference() ){
		matrix.mul( parent, m_Position );
		m_LibRef->Render( matrix );
	} else {
		SObjMeshIt m = m_Meshes.begin();
		for(;m!=m_Meshes.end();m++){
			matrix.mul( parent, m_Position );
			matrix.mul2( (*m).m_Position );
			(*m).m_Mesh->Render( matrix );
		}
	}

	if( Selected() ){
		matrix.mul( parent, m_Position );
		CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)matrix.a) );
		DrawSelectionBox( m_Box );
	}
}

void SObject2::RTL_Update( float dT ){
}

bool SObject2::BoxPick(
	ICullPlane *planes,
	IMatrix& parent )
{
	IMatrix matrix;

	if( IsReference() ){
		matrix.mul( parent, m_Position );
		return m_LibRef->BoxPick( planes, matrix );
	}

	SObjMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
		matrix.mul( parent, m_Position );
		matrix.mul2( (*m).m_Position );
		if( (*m).m_Mesh->BoxPick( planes, matrix ) )
			return true;
	}

	return false;
}

bool SObject2::RTL_Pick(
	float *distance,
	IVector& start,
	IVector& direction,
	IMatrix& parent )
{
	IMatrix matrix;
	bool picked = false;
	float min_dist = 0;

	if( IsReference() ){
		matrix.mul( parent, m_Position );
		picked = m_LibRef->RTL_Pick( &min_dist, start, direction, matrix );

	} else {

		float dist = 0;

		SObjMeshIt m = m_Meshes.begin();
		for(;m!=m_Meshes.end();m++){
			
			matrix.mul( parent, m_Position );
			matrix.mul2( (*m).m_Position );
			
			if( (*m).m_Mesh->Pick( &dist, start, direction, matrix ) ){
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

void SObject2::Move( IVector& amount ){
	m_Position.c.add( amount );
}

void SObject2::Rotate( IVector& center, IVector& axis, float angle ){

	IMatrix m;
	m.r( angle, axis );

	m_Position.c.sub( center );
	m.shorttransform( m_Position.c );
	m_Position.c.add( center );

	m.shorttransform( m_Position.i );
	m.shorttransform( m_Position.j );
	m.shorttransform( m_Position.k );
}

void SObject2::LocalRotate( IVector& axis, float angle ){
	IMatrix m;
	m.r( angle, axis );
	m.shorttransform( m_Position.i );
	m.shorttransform( m_Position.j );
	m.shorttransform( m_Position.k );
}

void SObject2::Scale( IVector& center, IVector& amount ){
	IMatrix m;
	m.s( amount );
	m_Position.c.sub( center );
	m.shorttransform( m_Position.c );
	m_Position.c.add( center );
}

void SObject2::LocalScale( IVector& amount ){
	IMatrix m;
	m.s( amount );
	m.shorttransform( m_Position.i );
	m.shorttransform( m_Position.j );
	m.shorttransform( m_Position.k );
}

void SObject2::LoadMeshDef( FSChunkDef *chunk ){

	_ASSERTE( chunk );

	m_Meshes.push_back(SObject2Mesh());
	m_Meshes.back().m_Mesh=new Mesh();

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){
		
		case SOBJ_CHUNK_MESH_NAME:
			FS.readstring(current.filehandle,
				m_Meshes.back().m_Name,
				sizeof(m_Meshes.back().m_Name) );
			break;

		case SOBJ_CHUNK_MESH_FILE:
			FS.readstring(current.filehandle,
				m_Meshes.back().m_FileName,
				sizeof(m_Meshes.back().m_FileName) );
			break;

		case SOBJ_CHUNK_MESH_POS:
			FS.read(current.filehandle,
				& m_Meshes.back().m_Position,
				sizeof(m_Meshes.back().m_Position) );
			break;

		case SOBJ_CHUNK_MESH_OPS:
			m_Meshes.back().m_Ops.Load( &current );
			break;
			
		case SOBJ_CHUNK_MESH_CORE:
			m_Meshes.back().m_Mesh->LoadMesh( &current );
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
			if( 0==(m_LibRef = (SObject2*)Lib.SearchObject(ClassID(),refbuffer)) )
				NConsole.print( "SObject2: '%s' not found in library", refbuffer );
			break;

		case SOBJ_CHUNK_MESH:
			_ASSERTE( m_LibRef == 0 );
			LoadMeshDef( &current );
			break;

		default:
			NConsole.print( "SObject2: undefined chunk 0x%04X", current.chunkid );
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
				FS.writestring(handle,(*m).m_Name);
				FS.wclosechunk(&chunk_level1);
			
				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_FILE);
				FS.writestring(handle,(*m).m_FileName);
				FS.wclosechunk(&chunk_level1);

				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_POS);
				FS.write(handle,&(*m).m_Position,sizeof((*m).m_Position));
				FS.wclosechunk(&chunk_level1);

				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_OPS);
				(*m).m_Ops.Save( handle );
				FS.wclosechunk(&chunk_level1);

				FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_CORE);
				(*m).m_Mesh->SaveMesh( handle );
				FS.wclosechunk(&chunk_level1);

			FS.wclosechunk(&chunk_level0);
		}

	}
}

//----------------------------------------------------

void SObject2::LibCopy( SceneObject *source ){
	
	_ASSERTE( source );
	_ASSERTE( source->ClassID() == ClassID() );

	SObject2 *obj = (SObject2*)source;

	strcpy( m_ClassName, obj->m_ClassName );
	strcpy( m_ClassScript, obj->m_ClassScript );
	m_Position.identity();

	m_DynamicList = obj->m_DynamicList;
	m_MakeUnique = obj->m_MakeUnique;

	SObjMeshIt m = obj->m_Meshes.begin();
	for(;m!=obj->m_Meshes.end();m++){
		m_Meshes.push_back(SObject2Mesh());
		m_Meshes.back().m_Mesh=new Mesh();
		m_Meshes.back().m_Mesh->Copy( (*m).m_Mesh );
		m_Meshes.back().m_Position.set( (*m).m_Position );
		m_Meshes.back().m_Ops.Copy( (*m).m_Ops );
		m_Meshes.back().CopyNames( (*m) );
	}

	UpdateBox();
}

void SObject2::ResolveReference() {
	if( IsReference() ){
		
		SObjMeshIt m = m_LibRef->m_Meshes.begin();
		for(;m!=m_LibRef->m_Meshes.end();m++){
			m_Meshes.push_back(SObject2Mesh());
			m_Meshes.back().m_Mesh=new Mesh();
			m_Meshes.back().m_Mesh->Copy( (*m).m_Mesh );
			m_Meshes.back().m_Position.set( (*m).m_Position );
			m_Meshes.back().m_Ops.Copy( (*m).m_Ops );
			m_Meshes.back().CopyNames( (*m) );
		}

		m_LibRef = 0;
		UpdateBox();
	}

	TranslateToWorld();
}

void SObject2::TranslateToWorld() {

	_ASSERTE( !IsReference() );

	SObjMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
		m->m_Mesh->Transform( m->m_Position );
		m->m_Mesh->Transform( m_Position );
		m->m_Position.identity();
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

bool SObject2::CreateFromLandscape( SLandscape *landscape ){

	_ASSERTE( landscape );

	// global options

	strcpy( m_ClassName, "Landscape" );
	strcpy( m_ClassScript, "" );
	m_Position.identity();
	m_Meshes.clear();

	m_Meshes.push_back(SObject2Mesh());
	m_Meshes.back().m_Mesh=new Mesh();
	m_Meshes.back().m_Position.identity();
	m_Meshes.back().m_Ops.Copy( landscape->m_Ops );

	// points

	for( int i=0; i<5; i++){
		
		MPoint point;
		point.m_Point.set( landscape->m_Points[i] );
		point.m_Normal.set( landscape->m_Normals[i] );
		point.m_TexCoord.set( landscape->m_TexCoord[i] );

		MPointLighting ltpoint;

		MLMPoint lmtex;
		lmtex.u = landscape->m_TexCoord[i].x;
		lmtex.v = landscape->m_TexCoord[i].y;
		lmtex.m_Valid = true;

		if(!m_Meshes.back().m_Mesh->AddPoint( point, &ltpoint, &lmtex ))
			return false;
	}

	// faces

	MFace face;

	face.p[0] = 0;
	face.p[1] = 1;
	face.p[2] = 2;
	if( !m_Meshes.back().m_Mesh->AddFace( face ) )
		return false;

	face.p[0] = 0;
	face.p[1] = 2;
	face.p[2] = 3;
	if( !m_Meshes.back().m_Mesh->AddFace( face ) )
		return false;

	face.p[0] = 0;
	face.p[1] = 3;
	face.p[2] = 4;
	if( !m_Meshes.back().m_Mesh->AddFace( face ) )
		return false;

	face.p[0] = 0;
	face.p[1] = 4;
	face.p[2] = 1;
	if( !m_Meshes.back().m_Mesh->AddFace( face ) )
		return false;

	// material

	MMaterial material;

	material.m_Ambient.set( landscape->m_Ambient );
	material.m_Diffuse.set( landscape->m_Diffuse );
	material.m_Specular.set( landscape->m_Specular );
	material.m_Emission.set( landscape->m_Emission );
	material.m_Power = landscape->m_Power;

	material.m_FaceStart = 0;
	material.m_FaceCount = 4;

	material.m_Texture = new ETexture( landscape->m_Texture );
	
	if( !m_Meshes.back().m_Mesh->AddMaterial( material ) )
		return false;

	// update & check

	m_Meshes.back().m_Mesh->FillD3DPoints();
	if( !m_Meshes.back().m_Mesh->Verify() )
		return false;

	UpdateBox();

	return true;
}

//----------------------------------------------------

