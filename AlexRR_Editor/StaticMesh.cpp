//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "StaticMesh.h"

#include "UI_Main.h"


//----------------------------------------------------

MMaterial::MMaterial(){
	m_FaceStart = 0;
	m_FaceCount = 0;
	m_Texture = 0;
	m_LightMap = 0;
	m_Ambient.set(0,0,0);
	m_Diffuse.set(1,1,1);
	m_Specular.set(0,0,0);
	m_Emission.set(0,0,0);
	m_Power = 0;
}

MMaterial::MMaterial( const MMaterial& source ){

	m_FaceStart = source.m_FaceStart;
	m_FaceCount = source.m_FaceCount;

	m_Texture = 0;
	if( source.m_Texture )
		m_Texture = new ETexture( source.m_Texture->name() );

	m_LightMap = 0;
	if( source.m_LightMap )
		m_LightMap = new ETexture( source.m_LightMap->name() );

	m_Ambient.set(source.m_Ambient);
	m_Diffuse.set(source.m_Diffuse);
	m_Specular.set(source.m_Specular);
	m_Emission.set(source.m_Emission);
	m_Power = source.m_Power;
}

MMaterial::~MMaterial(){
	SAFE_DELETE( m_Texture );
	SAFE_DELETE( m_LightMap );
}

//----------------------------------------------------

Mesh::Mesh()
{
}

Mesh::~Mesh(){
	m_Materials.clear();
	m_Points.clear();
	m_Faces.clear();
	m_D3D_Points.clear();
	m_Lighting.clear();
	m_LMap.clear();
}

//----------------------------------------------------

bool Mesh::Verify(){
	return true;
}

void Mesh::FillD3DPoints(){
	
	m_D3D_Points.clear();

	if( m_Points.empty() || m_Faces.empty() )
		return;

	m_D3D_Points.resize(m_Points.size());
	for(int i=0;i<m_Points.size();i++){
		m_D3D_Points[i].x = m_Points[i].m_Point.x;
		m_D3D_Points[i].y = m_Points[i].m_Point.y;
		m_D3D_Points[i].z = m_Points[i].m_Point.z;
		m_D3D_Points[i].nx = m_Points[i].m_Normal.x;
		m_D3D_Points[i].ny = m_Points[i].m_Normal.y;
		m_D3D_Points[i].nz = m_Points[i].m_Normal.z;
		if( g_RenderLightMaps ){
			if( m_LMap.empty() ){
				m_D3D_Points[i].tu = m_Points[i].m_TexCoord.x;
				m_D3D_Points[i].tv = m_Points[i].m_TexCoord.y;
			} else {
				m_D3D_Points[i].tu = m_LMap[i].u;
				m_D3D_Points[i].tv = m_LMap[i].v;
			}
		} else {
			m_D3D_Points[i].tu = m_Points[i].m_TexCoord.x;
			m_D3D_Points[i].tv = m_Points[i].m_TexCoord.y;
		}
	}
}

//----------------------------------------------------

void Mesh::Render( IMatrix& parent ){
	
	if( !m_Materials.empty() ){

		CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)parent.a) );
		CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
		CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL) );
		CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL) );
		CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL) );
		CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL) );

		for(int i=0;i<m_Materials.size();i++){

			if( g_RenderLightMaps ){

				D3DMATERIAL7 mat;
				mat.diffuse.r = 0;
				mat.diffuse.g = 0;
				mat.diffuse.b = 0;
				mat.diffuse.a = 0;
				mat.specular.r = 0;
				mat.specular.g = 0;
				mat.specular.b = 0;
				mat.specular.a = 0;
				mat.emissive.r = 1;
				mat.emissive.g = 1;
				mat.emissive.b = 1;
				mat.emissive.a = 1;
				mat.power = 0;

				CDX( UI.d3d()->SetRenderState(
					D3DRENDERSTATE_LIGHTING,FALSE) );
				CDX( UI.d3d()->SetMaterial(
					&mat ) );

				if( UI.te() ){
					if( m_Materials[i].m_LightMap ){
						m_Materials[i].m_LightMap->Bind();
					} else {
						TM.BindNone();
					}
				} else {
					TM.BindNone();
				}

			} else {

				CDX( UI.d3d()->SetMaterial( &m_Materials[i].m_D3D ) );

				if( UI.te() ){
					if( m_Materials[i].m_Texture ){
						m_Materials[i].m_Texture->Bind();
					} else {
						TM.BindNone();
					}
				} else {
					TM.BindNone();
				}
			}

			CDX( UI.d3d()->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,D3DFVF_VERTEX,
				m_D3D_Points.begin(),m_D3D_Points.size(),
				m_Faces.begin()->p+m_Materials[i].m_FaceStart*3,
				m_Materials[i].m_FaceCount*3,
				0 ));
		}
	}
}

//----------------------------------------------------

bool Mesh::Pick(
	float *distance,
	IVector& start,
	IVector& direction,
	IMatrix& parent )
{
	bool picked = false;
	float mindist = 0;

	for( int i=0;i<m_Faces.size();i++){

		IVector p[3];
		IVector facenormal;

		for( int k=0;k<3;k++)
			parent.transform( p[k], m_Points[m_Faces[i].p[k]].m_Point );

		facenormal.mknormal(p[0], p[1], p[2]);
		if( PickFace(distance, start, direction, facenormal, p[0], p[1], p[2] )){
			if( picked ){
				mindist = min((*distance),mindist);
			} else {
				picked = true;
				mindist = (*distance);
			}
		}
	}

	if( picked ){
		(*distance) = mindist;
		return true;
	}

	return false;
}

bool Mesh::BoxPick(
	ICullPlane *planes,
	IMatrix& parent )
{
	for( int i=0;i<m_Faces.size();i++){
		IVector p[3];
		for( int k=0;k<3;k++)
			parent.transform( p[k],
				m_Points[m_Faces[i].p[k]].m_Point );
		if( FrustumPickFace(planes, p[0], p[1], p[2] ) )
			return true;
	}

	return false;
}

//----------------------------------------------------

void Mesh::Append( Mesh *source ){

	_ASSERTE( source );
	
	vector<MMaterial> amaterials;
	vector<MPoint> apoints;
	vector<MFace> afaces;

	amaterials.insert(amaterials.end(),source->m_Materials.begin(),source->m_Materials.end());
	apoints.insert(apoints.end(),source->m_Points.begin(),source->m_Points.end() );
	afaces.insert(afaces.end(),source->m_Faces.begin(),source->m_Faces.end() );

	for( int i=0; i<amaterials.size(); i++)
		amaterials[i].m_FaceStart += m_Faces.size();

	for(i=0; i<afaces.size(); i++)
		for(int k=0;k<3;k++)
			afaces[i].p[k] += m_Points.size();

	m_Materials.insert(m_Materials.end(),amaterials.begin(),amaterials.end());
	m_Points.insert(m_Points.end(),apoints.begin(),apoints.end());
	m_Faces.insert(m_Faces.end(),afaces.begin(),afaces.end());


	amaterials.clear();
	apoints.clear();
	afaces.clear();

	if( !source->m_Lighting.empty() )
		m_Lighting.insert(
			m_Lighting.end(),
			source->m_Lighting.begin(),
			source->m_Lighting.end() );

	if( !source->m_LMap.empty() )
		m_LMap.insert(
			m_LMap.end(),
			source->m_LMap.begin(),
			source->m_LMap.end() );

	FillD3DPoints();
}

bool Mesh::CompareMaterials( int id1, int to_collapse ){

	if( m_Materials[id1].m_Texture == 0 ||
		m_Materials[to_collapse].m_Texture == 0    )
	{
		if( m_Materials[id1].m_Texture !=
			m_Materials[to_collapse].m_Texture )
				return false;

	} else {
		if( 0!=stricmp(
			m_Materials[id1].m_Texture->name(),
			m_Materials[to_collapse].m_Texture->name() ) )
				return false;
	}

	if( m_Materials[id1].m_LightMap == 0 ||
		m_Materials[to_collapse].m_LightMap == 0    )
	{
		if( m_Materials[id1].m_LightMap !=
			m_Materials[to_collapse].m_LightMap )
				return false;

	} else {
		if( 0!=stricmp(
			m_Materials[id1].m_LightMap->name(),
			m_Materials[to_collapse].m_LightMap->name() ) )
				return false;
	}

	if( !m_Materials[id1].m_Diffuse.cmp( m_Materials[to_collapse].m_Diffuse ) ) return false;
	if( !m_Materials[id1].m_Ambient.cmp( m_Materials[to_collapse].m_Ambient ) ) return false;
	if( !m_Materials[id1].m_Specular.cmp( m_Materials[to_collapse].m_Specular ) ) return false;
	if( !m_Materials[id1].m_Emission.cmp( m_Materials[to_collapse].m_Emission ) ) return false;
	if( !cmpflt(m_Materials[id1].m_Power,m_Materials[to_collapse].m_Power ) ) return false;

	return true;
}

bool Mesh::CollapseMaterials( int id1, int to_collapse ){

	_ASSERTE( id1 >= 0 && id1<m_Materials.size() );
	_ASSERTE( to_collapse >= 0 && to_collapse<m_Materials.size() );
	_ASSERTE( to_collapse > id1 );

	// save face data & remove material from list
	int o_facestart1 = m_Materials[id1].m_FaceStart;
	int o_facecount1 = m_Materials[id1].m_FaceCount;
	int o_facestart2 = m_Materials[to_collapse].m_FaceStart;
	int o_facecount2 = m_Materials[to_collapse].m_FaceCount;

	// ----- ??????????  does not work --- why ??????
	// m_Materials.erase( m_Materials.begin() + to_collapse );

	vector<MMaterial> amaterials;
	vector<MFace> afaces;

	for( int j=0;j<m_Materials.size(); j++)
		if( j != to_collapse && j != id1 )
			amaterials.push_back( m_Materials[j] );

	for( j=0; j<amaterials.size(); j++){
		int newstart = afaces.size();
		afaces.insert( afaces.end(),
			m_Faces.begin() + amaterials[j].m_FaceStart,
			m_Faces.begin() + amaterials[j].m_FaceStart + amaterials[j].m_FaceCount );
		amaterials[j].m_FaceStart = newstart;
	}

	amaterials.push_back( m_Materials[id1] );
	amaterials.back().m_FaceStart = afaces.size();
	amaterials.back().m_FaceCount = o_facecount1 + o_facecount2;

	afaces.insert( afaces.end(),
		m_Faces.begin() + o_facestart1,
		m_Faces.begin() + o_facestart1 + o_facecount1 );
	afaces.insert( afaces.end(),
		m_Faces.begin() + o_facestart2,
		m_Faces.begin() + o_facestart2 + o_facecount2 );

	m_Materials.clear();
	m_Materials.insert( m_Materials.end(), amaterials.begin(), amaterials.end() );
	amaterials.clear();

	m_Faces.clear();
	m_Faces.insert( m_Faces.end(), afaces.begin(), afaces.end() );
	afaces.clear();

	m_D3D_Points.clear();
	FillD3DPoints();
	return true;
}

//----------------------------------------------------

void Mesh::Copy( Mesh *source ){
	_ASSERTE( source );
	m_Materials.insert(m_Materials.end(),source->m_Materials.begin(),source->m_Materials.end());
	m_Points.insert(m_Points.end(),source->m_Points.begin(),source->m_Points.end() );
	m_Faces.insert(m_Faces.end(),source->m_Faces.begin(),source->m_Faces.end() );
	if( !source->m_Lighting.empty() )
		m_Lighting.insert(
			m_Lighting.end(),
			source->m_Lighting.begin(),
			source->m_Lighting.end() );
	if( !source->m_LMap.empty() )
		m_LMap.insert(
			m_LMap.end(),
			source->m_LMap.begin(),
			source->m_LMap.end() );
	FillD3DPoints();
}

//----------------------------------------------------

void Mesh::GetMaterial( Mesh *dest, MMaterial *material ){
	_ASSERTE( dest );

	int fstface = 0;
	vector<int> pointidx;

	pointidx.resize( m_Points.size() );
	for(int i=0;i<pointidx.size();i++)
		pointidx[i] = 0;

	for( i=0; i<material->m_FaceCount; i++)
		for(int k=0;k<3;k++)
			pointidx[m_Faces[material->m_FaceStart+i].p[k]]=1;

	for( i=0;i<m_Points.size();i++){
		if( pointidx[i] ){
			pointidx[i] = dest->m_Points.size();
			dest->m_Points.push_back( m_Points[i] );
			if( !m_Lighting.empty() )
				dest->m_Lighting.push_back( m_Lighting[i] );
			if( !m_LMap.empty() )
				dest->m_LMap.push_back( m_LMap[i] );

		} else {
			pointidx[i] = -1;
		}
	}

	fstface = dest->m_Faces.size();
	for( i=0; i<material->m_FaceCount;i++){
		dest->m_Faces.push_back( m_Faces[material->m_FaceStart+i] );
		for(int k=0;k<3;k++)
			dest->m_Faces.back().p[k] = pointidx[dest->m_Faces.back().p[k]];
	}

	dest->m_Materials.push_back( (*material) );
	dest->m_Materials.back().m_FaceStart += fstface;

	dest->FillD3DPoints();
	pointidx.clear();
}

bool Mesh::AddPoint( MPoint& point, MPointLighting *lt, MLMPoint *lpmt ){
	m_Points.push_back( point );
	m_Lighting.push_back( lt ? (*lt) : MPointLighting() );
	m_LMap.push_back( lpmt ? (*lpmt) : MLMPoint() );
	return true;
}

bool Mesh::AddFace( MFace& face ){
	for( int i=0; i<m_Faces.size(); i++){
		if(((face.p0 == m_Faces[i].p0) &&
			(face.p1 == m_Faces[i].p1) &&
			(face.p2 == m_Faces[i].p2))     ||
		   ((face.p0 == m_Faces[i].p1) &&
			(face.p1 == m_Faces[i].p2) &&
			(face.p2 == m_Faces[i].p0))     ||
		   ((face.p0 == m_Faces[i].p2) &&
			(face.p1 == m_Faces[i].p0) &&
			(face.p2 == m_Faces[i].p1))    )
				return false;
	}
	m_Faces.push_back( face );
	return true;
}

bool Mesh::AddMaterial( MMaterial& material ){
	if( material.m_FaceStart < 0 )
		return false;
	if( material.m_FaceStart>=m_Faces.size() )
		return false;
	if( material.m_FaceCount < 0 )
		return false;
	if( material.m_FaceCount >m_Faces.size() )
		return false;
	m_Materials.push_back( material );
	return true;
}

//----------------------------------------------------

void Mesh::Transform( IMatrix& parent ){
	for( int i=0; i<m_Points.size(); i++){
		parent.transform(m_Points[i].m_Point);
		parent.shorttransform(m_Points[i].m_Normal);
	}
	FillD3DPoints();
}

void Mesh::GetBox( IAABox& box ){

	if( m_Points.empty() ){
		box.vmin.set(0,0,0);
		box.vmax.set(0,0,0);
		return; }

	box.vmin.set( m_Points[0].m_Point );
	box.vmax.set( m_Points[0].m_Point );

	for( int i=1; i<m_Points.size(); i++)
		box.growrange( 
			m_Points[i].m_Point.x,
			m_Points[i].m_Point.y,
			m_Points[i].m_Point.z );
}

//----------------------------------------------------

bool Mesh::LoadMaterial( FSChunkDef *chunk ){
	
	char namebuffer[MAX_PATH];
	MMaterial material;

	_ASSERTE( chunk );
	
	material.m_FaceStart = FS.readdword( chunk->filehandle );
	material.m_FaceCount = FS.readdword( chunk->filehandle );

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case STM_CHUNK_MATERIAL_COLOR:
			FS.readcolor4( current.filehandle, material.m_Ambient.val );
			FS.readcolor4( current.filehandle, material.m_Diffuse.val );
			FS.readcolor4( current.filehandle, material.m_Specular.val );
			FS.readcolor4( current.filehandle, material.m_Emission.val );
			material.m_Power = FS.readfloat( current.filehandle );
			break;

		case STM_CHUNK_MATERIAL_TNAME:
			FS.readstring( current.filehandle, namebuffer, MAX_PATH );
			material.m_Texture = new ETexture( namebuffer );
			break;

		default:
			NConsole.print( "EScene: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}

	m_Materials.push_back( material );
	return true;
}

bool Mesh::LoadMesh( FSChunkDef *chunk ){
	
	_ASSERTE( chunk );

	FSChunkDef current;
	FS.initchunk( &current, chunk->filehandle );

	while( FS.rinchunk( chunk ) ){

		FS.ropenchunk( &current );
		switch( current.chunkid ){

		case STM_CHUNK_VERSION:
			break;

		case STM_CHUNK_HEADER:
			break;

		case STM_CHUNK_POINTLIST:
			m_Points.resize( current.chunksize / sizeof(MPoint) );
			FS.read( current.filehandle, m_Points.begin(), current.chunksize );
			break;

		case STM_CHUNK_FACELIST:
			m_Faces.resize( current.chunksize / sizeof(MFace) );
			FS.read( current.filehandle, m_Faces.begin(), current.chunksize );
			break;

		case STM_CHUNK_MATERIAL:
			if( !LoadMaterial( &current ) )
				NConsole.print( "Error: invalid material chunk" );
			break;

		case STM_CHUNK_LMAPCHANNEL:
			m_LMap.resize( current.chunksize / sizeof(MLMPoint) );
			FS.read( current.filehandle, m_LMap.begin(), current.chunksize );
			break;

		default:
			NConsole.print( "EScene: undefined chunk 0x%04X", current.chunkid );
			break; }

		FS.rclosechunk( &current );
	}

	FillD3DPoints();
	return true;
}

bool Mesh::Load( char *filename ){

	_ASSERTE( filename );

	char fullname[MAX_PATH];
	strcpy( fullname, filename );

	FS.m_Meshes.Update( fullname );
	int handle = FS.open( fullname );

	if( handle <= 0 ){
		NConsole.print( "Error: can't open mesh file '%s'", filename );
		return false; }

	FSChunkDef current;
	FS.initchunk( &current, handle );

	while( !FS.eof( handle ) ){
		bool undefined_chunk = false;
		FS.ropenchunk( &current );
		switch( current.chunkid ){
		
		case STM_CHUNK_MESH:
			if( !LoadMesh( &current ) )
				NConsole.print( "Error: invalid mesh chunk" );
			break;
		
		default:
			undefined_chunk = true;
			break;
		}
		if( undefined_chunk )
			NConsole.print( "EScene: undefined chunk 0x%04X", current.chunkid );
		
		FS.rclosechunk( &current );
	}

	FS.close( handle );
	return true;
}

//----------------------------------------------------


void Mesh::SaveMesh( int handle ){

	FSChunkDef matchunk;
	FSChunkDef current;
	FS.initchunk( &current, handle );
	FS.initchunk( &matchunk, handle );

	FS.wopenchunk( &current, STM_CHUNK_VERSION );
	FS.wclosechunk( &current );

	FS.wopenchunk( &current, STM_CHUNK_HEADER );
	FS.wclosechunk( &current );

	FS.wopenchunk( &current, STM_CHUNK_POINTLIST );
	FS.write( handle, m_Points.begin(), m_Points.size()*sizeof(MPoint));
	FS.wclosechunk( &current );

	FS.wopenchunk( &current, STM_CHUNK_FACELIST );
	FS.write( handle, m_Faces.begin(), m_Faces.size()*sizeof(MFace));
	FS.wclosechunk( &current );

	for(int i=0;i<m_Materials.size();i++){
		FS.wopenchunk( &current, STM_CHUNK_MATERIAL );

		FS.writedword( handle, m_Materials[i].m_FaceStart );
		FS.writedword( handle, m_Materials[i].m_FaceCount );

		FS.wopenchunk( &matchunk, STM_CHUNK_MATERIAL_COLOR );
		FS.writecolor4( handle, m_Materials[i].m_Ambient.val );
		FS.writecolor4( handle, m_Materials[i].m_Diffuse.val );
		FS.writecolor4( handle, m_Materials[i].m_Specular.val );
		FS.writecolor4( handle, m_Materials[i].m_Emission.val );
		FS.writefloat( handle, m_Materials[i].m_Power );
		FS.wclosechunk( &matchunk );

		if( m_Materials[i].m_Texture ){
			FS.wopenchunk( &matchunk, STM_CHUNK_MATERIAL_TNAME );
			FS.writestring( handle, m_Materials[i].m_Texture->name() );
			FS.wclosechunk( &matchunk );
		}

		FS.wclosechunk( &current );
	}

	if( !m_LMap.empty() ){
		FS.wopenchunk( &current, STM_CHUNK_LMAPCHANNEL );
		FS.write( handle, m_LMap.begin(), m_LMap.size()*sizeof(MLMPoint));
		FS.wclosechunk( &current );
	}
}


