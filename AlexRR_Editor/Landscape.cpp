//----------------------------------------------------
// file: Landscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Landscape.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "FileSystem.h"
#include "MenuTemplate.rh"


//----------------------------------------------------

SLandscape::SLandscape( char *name )
	:SceneObject()
{
	Construct();
	strcpy( m_Name, name );
}

SLandscape::SLandscape()
	:SceneObject()
{
	Construct();
}

void SLandscape::Construct(){

	m_ClassID = OBJCLASS_LANDSCAPE;
	
	m_Texture = 0;
	m_MapX = m_MapZ = 0;

	m_Ops.Reset();

	m_Ambient.set( 0.2f, 0.2f, 0.2f, 1.f );
	m_Diffuse.set( 1.0f, 1.0f, 1.0f, 1.f );
	m_Specular.set( 0.f, 0.f, 0.f, 1.f );
	m_Emission.set( 0.f, 0.f, 0.f, 1.f );
	m_Power = 0.f;

	CalcDefCenter();
	CalcDefPoints();

	UpdateNormals();
	FillD3DPoints();
}

SLandscape::~SLandscape(){
	SAFE_DELETE( m_Texture );
}

//----------------------------------------------------

void SLandscape::CalcDefCenter(){
	m_Center.set(
		Scene.lx() * m_MapX + Scene.lx()/2.f, 0,
		Scene.lz() * m_MapZ + Scene.lz()/2.f );
}

void SLandscape::CalcDefPoints(){

	for( int i=0;i<5;i++){
		m_Points[i].set( m_Center );
		m_Normals[i].set( 0, 1, 0 );
		m_TexCoord[i].set( 0.5f, 0.5f, 0 );
	}

	m_Points[1].x -= Scene.lx() / 2.f;
	m_Points[2].x -= Scene.lx() / 2.f;
	m_Points[3].x += Scene.lx() / 2.f;
	m_Points[4].x += Scene.lx() / 2.f;

	m_Points[1].z -= Scene.lz() / 2.f;
	m_Points[2].z += Scene.lz() / 2.f;
	m_Points[3].z += Scene.lz() / 2.f;
	m_Points[4].z -= Scene.lz() / 2.f;

	m_TexCoord[1].x -= 0.5f;
	m_TexCoord[2].x -= 0.5f;
	m_TexCoord[3].x += 0.5f;
	m_TexCoord[4].x += 0.5f;

	m_TexCoord[1].y += 0.5f;
	m_TexCoord[2].y -= 0.5f;
	m_TexCoord[3].y -= 0.5f;
	m_TexCoord[4].y += 0.5f;
}

void SLandscape::FillD3DPoints(){
	
	if( m_D3D_Indices.size() != 12 )
		m_D3D_Indices.resize( 12 );

	if( m_D3D_Points.size() != 5 )
		m_D3D_Points.resize( 5 );

	for( int i=0; i<5; i++){
		m_D3D_Points[i].x = m_Points[i].x;
		m_D3D_Points[i].y = m_Points[i].y;
		m_D3D_Points[i].z = m_Points[i].z;
		m_D3D_Points[i].nx = m_Normals[i].x;
		m_D3D_Points[i].ny = m_Normals[i].y;
		m_D3D_Points[i].nz = m_Normals[i].z;
		m_D3D_Points[i].tu = m_TexCoord[i].x;
		m_D3D_Points[i].tv = m_TexCoord[i].y;
	}

	m_D3D_Indices[0] = 0;
	m_D3D_Indices[1] = 1;
	m_D3D_Indices[2] = 2;

	m_D3D_Indices[3] = 0;
	m_D3D_Indices[4] = 2;
	m_D3D_Indices[5] = 3;

	m_D3D_Indices[6] = 0;
	m_D3D_Indices[7] = 3;
	m_D3D_Indices[8] = 4;

	m_D3D_Indices[9] = 0;
	m_D3D_Indices[10] = 4;
	m_D3D_Indices[11] = 1;
}

void SLandscape::SetQuad( int x, int z ){
	m_MapX = x;
	m_MapZ = z;
	CalcDefCenter();
	CalcDefPoints();
	UpdateNormals();
	FillD3DPoints();
}

void SLandscape::SetTexture( ETexture *t ){
	SAFE_DELETE( m_Texture );
	if( t ) m_Texture = new ETexture( t );
}

bool SLandscape::TexCompare( char *texturename ){
	_ASSERTE( texturename );
	if( m_Texture == 0 ) return false;
	return (0==stricmp(texturename,m_Texture->name()));
}

bool SLandscape::GetBox( IAABox& box ){
	box.vmin.set( m_Points[0] );
	box.vmax.set( m_Points[0] );
	for( int i=1; i<5; i++)
		box.growrange(
			m_Points[i].x,
			m_Points[i].y,
			m_Points[i].z );
	return true;
}

void SLandscape::Render( IMatrix& parent ){

	if( m_D3D_Indices.size() == 0 || 
		m_D3D_Points.size() == 0 )
			return;

	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)parent.a) );

	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );

	D3DMATERIAL7 material;

	if( m_Selected ){
		memset(&material,0,sizeof(material));
		material.emissive.r = 1;
		material.emissive.g = 0;
		material.emissive.b = 0;
		material.emissive.a = 1;
	} else{
		m_Diffuse.fillfloat4((float*)&material.diffuse);
		m_Ambient.fillfloat4((float*)&material.ambient);
		m_Specular.fillfloat4((float*)&material.specular);
		m_Emission.fillfloat4((float*)&material.emissive);
		material.power = m_Power;
	}

	CDX( UI.d3d()->SetMaterial( &material ) );

	if( UI.te() ){
		if( m_Texture )
			m_Texture->Bind();
	}

	CDX( UI.d3d()->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,D3DFVF_VERTEX,
		m_D3D_Points.begin(), m_D3D_Points.size(),
		m_D3D_Indices.begin(), m_D3D_Indices.size(),
		0 ));
}

bool SLandscape::BoxPick(
	ICullPlane *planes,
	IMatrix& parent )
{
	IVector p[5];
	for( int i=0;i<5;i++)
		parent.transform( p[i], m_Points[i] );

	if( FrustumPickFace(planes, p[0], p[1], p[2] ) ||
		FrustumPickFace(planes, p[0], p[2], p[3] ) ||
		FrustumPickFace(planes, p[0], p[3], p[4] ) ||
		FrustumPickFace(planes, p[0], p[4], p[1] )    )
			return true;

	return false;
}

bool SLandscape::RTL_Pick(
	float *distance,
	IVector& start,
	IVector& direction,
	IMatrix& parent )
{

	IVector p[5];
	for( int i=0;i<5;i++)
		parent.transform( p[i], m_Points[i] );

	IVector facenormal;
	
	facenormal.mknormal(p[0], p[1], p[2]);
	if( PickFace(distance, start, direction, facenormal, p[0], p[1], p[2] ))
		return true;

	facenormal.mknormal(p[0], p[2], p[3]);
	if( PickFace(distance, start, direction, facenormal,p[0], p[2], p[3] ))
		return true;

	facenormal.mknormal(p[0], p[3], p[4]);
	if( PickFace(distance, start, direction, facenormal,p[0], p[3], p[4] ))
		return true;

	facenormal.mknormal(p[0], p[4], p[1]);
	if( PickFace(distance, start, direction, facenormal,p[0], p[4], p[1] ))
		return true;

	return false;
}

bool SLandscape::QuadPick( int x, int z ){
	return (m_MapX==x && m_MapZ==z);
}

void SLandscape::Move( IVector& amount ){
	m_Center.add( amount );
	for( int i=0; i<5; i++)
		m_Points[i].add( amount );
	FillD3DPoints();
}

void SLandscape::Rotate( IVector& center, IVector& axis, float angle ){
	
	IMatrix m;
	m.r( angle, axis );

	m_Center.sub( center );
	m.shorttransform( m_Center );
	m_Center.add( center );

	for( int i=0; i<5; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
		m.shorttransform( m_Normals[i] );
	}

	UpdateNormals();
	FillD3DPoints();
}

void SLandscape::Scale( IVector& center, IVector& amount ){
	
	IMatrix m;
	m.s( amount );

	m_Center.sub( center );
	m.shorttransform( m_Center );
	m_Center.add( center );

	for( int i=0; i<5; i++){
		m_Points[i].sub( center );
		m.shorttransform( m_Points[i] );
		m_Points[i].add( center );
	}

	UpdateNormals();
	FillD3DPoints();
}

//----------------------------------------------------

void SLandscape::TryConnectTo( SLandscape *to ){

	bool needupdate = false;

	if( to->CompareMapPos(m_MapX+1,m_MapZ+1) ){
		m_Points[3].set(to->m_Points[1]);
		needupdate = true;
	}

	if( to->CompareMapPos(m_MapX-1,m_MapZ-1) ){
		m_Points[1].set(to->m_Points[3]);
		needupdate = true;
	}

	if( to->CompareMapPos(m_MapX+1,m_MapZ-1) ){
		m_Points[4].set(to->m_Points[2]);
		needupdate = true;
	}
		
	if( to->CompareMapPos(m_MapX-1,m_MapZ+1) ){
		m_Points[2].set(to->m_Points[4]);
		needupdate = true;
	}

	if( to->CompareMapPos(m_MapX,m_MapZ+1) ){
		m_Points[2].set(to->m_Points[1]);
		m_Points[3].set(to->m_Points[4]);
		needupdate = true;
	}

	if( to->CompareMapPos(m_MapX,m_MapZ-1) ){
		m_Points[1].set(to->m_Points[2]);
		m_Points[4].set(to->m_Points[3]);
		needupdate = true;
	}

	if( to->CompareMapPos(m_MapX+1,m_MapZ) ){
		m_Points[3].set(to->m_Points[2]);
		m_Points[4].set(to->m_Points[1]);
		needupdate = true;
	}

	if( to->CompareMapPos(m_MapX-1,m_MapZ) ){
		m_Points[2].set(to->m_Points[3]);
		m_Points[1].set(to->m_Points[4]);
		needupdate = true;
	}

	if( needupdate ){

		m_Points[0].set( m_Points[1] );
		m_Points[0].add( m_Points[2] );
		m_Points[0].add( m_Points[3] );
		m_Points[0].add( m_Points[4] );
		m_Points[0].mul( 0.25f );

		FillD3DPoints();
	}
}

void SLandscape::UpdateNormals(){
	m_FaceNormals[0].mknormal( m_Points[0], m_Points[1], m_Points[2]);
	m_FaceNormals[1].mknormal( m_Points[0], m_Points[2], m_Points[3]);
	m_FaceNormals[2].mknormal( m_Points[0], m_Points[3], m_Points[4]);
	m_FaceNormals[3].mknormal( m_Points[0], m_Points[4], m_Points[1]);
}

void SLandscape::ResetPointNormals(){
	m_Normals[0].set(0,1,0);
	m_Normals[1].add( m_FaceNormals[0], m_FaceNormals[3] );
	m_Normals[2].add( m_FaceNormals[0], m_FaceNormals[1] );
	m_Normals[3].add( m_FaceNormals[2], m_FaceNormals[1] );
	m_Normals[4].add( m_FaceNormals[2], m_FaceNormals[3] );
}

void SLandscape::TryAddPointNormals( SLandscape *from ){

	if( from->CompareMapPos(m_MapX+1,m_MapZ+1) ){
		m_Normals[3].add(from->m_FaceNormals[0]);
		m_Normals[3].add(from->m_FaceNormals[3]);
	}

	if( from->CompareMapPos(m_MapX-1,m_MapZ-1) ){
		m_Normals[1].add(from->m_FaceNormals[1]);
		m_Normals[1].add(from->m_FaceNormals[2]);
	}

	if( from->CompareMapPos(m_MapX+1,m_MapZ-1) ){
		m_Normals[4].add(from->m_FaceNormals[0]);
		m_Normals[4].add(from->m_FaceNormals[1]);
	}
		
	if( from->CompareMapPos(m_MapX-1,m_MapZ+1) ){
		m_Normals[2].add(from->m_FaceNormals[3]);
		m_Normals[2].add(from->m_FaceNormals[2]);
	}

	if( from->CompareMapPos(m_MapX,m_MapZ+1) ){
		m_Normals[2].add(from->m_FaceNormals[0]);
		m_Normals[2].add(from->m_FaceNormals[3]);
		m_Normals[3].add(from->m_FaceNormals[2]);
		m_Normals[3].add(from->m_FaceNormals[3]);
	}

	if( from->CompareMapPos(m_MapX,m_MapZ-1) ){
		m_Normals[1].add(from->m_FaceNormals[0]);
		m_Normals[1].add(from->m_FaceNormals[1]);
		m_Normals[4].add(from->m_FaceNormals[1]);
		m_Normals[4].add(from->m_FaceNormals[2]);
	}

	if( from->CompareMapPos(m_MapX+1,m_MapZ) ){
		m_Normals[3].add(from->m_FaceNormals[0]);
		m_Normals[3].add(from->m_FaceNormals[1]);
		m_Normals[4].add(from->m_FaceNormals[0]);
		m_Normals[4].add(from->m_FaceNormals[3]);
	}

	if( from->CompareMapPos(m_MapX-1,m_MapZ) ){
		m_Normals[2].add(from->m_FaceNormals[1]);
		m_Normals[2].add(from->m_FaceNormals[2]);
		m_Normals[1].add(from->m_FaceNormals[2]);
		m_Normals[1].add(from->m_FaceNormals[3]);
	}
}

void SLandscape::NormalizePointNormals(){

	m_Normals[1].normalize();
	m_Normals[2].normalize();
	m_Normals[3].normalize();
	m_Normals[4].normalize();

	m_Normals[0].set(0,0,0);
	for( int i=0; i<4;i++)
		m_Normals[0].add( m_Normals[i+1] );

	m_Normals[0].normalize();
}

//----------------------------------------------------

void SLandscape::LoadData( FSChunkDef *chunk ){

	_ASSERTE( chunk );

	m_MapX = FS.readdword( chunk->filehandle );
	m_MapZ = FS.readdword( chunk->filehandle );

	char b[MAX_PATH];
	FS.readstring( chunk->filehandle, b, MAX_PATH );

	if( 0!=stricmp("<notexture>",b) ){
		m_Texture = new ETexture( b ); }

	FS.readcolor4( chunk->filehandle, m_Ambient.val );
	FS.readcolor4( chunk->filehandle, m_Diffuse.val );
	FS.readcolor4( chunk->filehandle, m_Specular.val );
	FS.readcolor4( chunk->filehandle, m_Emission.val );
	m_Power = FS.readfloat( chunk->filehandle );

	FS.readvector( chunk->filehandle, m_Center.a );
	
	for( int i=0;i<5;i++){
		FS.readvector( chunk->filehandle, m_Points[i].a );
		FS.readvector( chunk->filehandle, m_Normals[i].a );
		FS.readvector( chunk->filehandle, m_TexCoord[i].a );
	}
}

void SLandscape::Load( FSChunkDef *chunk ){

	_ASSERTE( chunk );

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	FS.readstring( current.filehandle, m_Name, sizeof(m_Name) );

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case LAND_CHUNK_DATA:
			LoadData( &current );
			break;

		case LAND_CHUNK_OPS:
			m_Ops.Load( &current );
			break;

		default:
			NConsole.print( "SObject2: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}

	UpdateNormals();
	FillD3DPoints();
}

void SLandscape::Save( int handle ){

	FSChunkDef ck;
	FS.initchunk( &ck, handle );

	// name

	FS.writestring( handle, m_Name );

	// data

	FS.wopenchunk( &ck, LAND_CHUNK_DATA );

	FS.writedword( handle, m_MapX );
	FS.writedword( handle, m_MapZ );

	if( m_Texture ){
		FS.writestring( handle, m_Texture->name() );
	} else {
		FS.writestring( handle, "<notexture>" );
	}

	FS.writecolor4( handle, m_Ambient.val );
	FS.writecolor4( handle, m_Diffuse.val );
	FS.writecolor4( handle, m_Specular.val );
	FS.writecolor4( handle, m_Emission.val );
	FS.writefloat( handle, m_Power );

	FS.writevector( handle, m_Center.a );
	
	for( int i=0;i<5;i++){
		FS.writevector( handle, m_Points[i].a );
		FS.writevector( handle, m_Normals[i].a );
		FS.writevector( handle, m_TexCoord[i].a );
	}

	FS.wclosechunk( &ck );

	// options

	FS.wopenchunk( &ck, LAND_CHUNK_OPS );
	m_Ops.Save( handle );
	FS.wclosechunk( &ck );
}

//----------------------------------------------------

