//----------------------------------------------------
// file: BuilderVis.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "Landscape.h"
#include "SObject2.h"
//----------------------------------------------------

#pragma pack(push,1)
struct _VIS_HEADER2 {
	WORD	wFormatVersion;	// == sizeof(_VIS_HEADER2)
	WORD	wLayers;		// 1..?
	WORD	wSizeX,wSizeZ;	// map size in subdivision 
	float	fSize;			// size of one subdiv.
};
struct _VIS_SLOT2 {			// number of slots equals to wLayers
	DWORD	dwPtr;			// offset in vis data file
	DWORD	dwCnt;			// number of visuals in the list
};
#pragma pack(pop)

//----------------------------------------------------


bool SceneBuilder::BuildVisibility(){

	int vis_x = floorf((m_LevelBox.vmax.x - m_LevelBox.vmin.x) / m_Scene->m_BOPVisCellSize.x );
	int vis_z = floorf((m_LevelBox.vmax.z - m_LevelBox.vmin.z) / m_Scene->m_BOPVisCellSize.z );

	m_VMap.Allocate( vis_x, vis_z );

	float slot_cost = 50.f / (float)(vis_x*vis_z);
	float cur_progress = 0;

	// !!!!!! FIXME !!!!!

	for( int i=0; i<vis_x; i++)
		for( int j=0; j<vis_z; j++){

			VisItem *item = m_VMap.Get(i,j);
			
			SI_ListIt it = m_Statics.begin();
			for(;it!=m_Statics.end();it++)
				item->m_Files.push_back( &(*it) );

			cur_progress += slot_cost;
			BWindow.SetProgress( cur_progress );
		}

	// writing visibility

	char matrixfilename[MAX_PATH];
	strcpy( matrixfilename, m_Scene->m_FNMap );
	m_LevelPath.Update( matrixfilename );
	int hfmap = FS.create( matrixfilename );

	char datafilename[MAX_PATH];
	strcpy( datafilename, m_Scene->m_FNVisibility );
	m_LevelPath.Update( datafilename );
	int hfvis = FS.create( datafilename );

	_VIS_HEADER2 visheader;
	visheader.wFormatVersion= sizeof(visheader);
	visheader.wLayers		= 1;
	visheader.wSizeX		= vis_x;
	visheader.wSizeZ		= vis_z;
	visheader.fSize			= m_Scene->m_BOPVisCellSize.x;

	FS.write( hfmap, &visheader, sizeof(visheader) );

	_VIS_SLOT2 slot;

	for( i=0; i<vis_x; i++)
		for( int j=0; j<vis_z; j++){

			VisItem *item = m_VMap.Get(i,j);
			
			// map
			slot.dwPtr = FS.pos(hfvis);
			slot.dwCnt = item->m_Files.size();
			FS.write(hfmap,&slot,sizeof(slot));

			// vis
			int k=item->m_Files.size();
			while( k-- )
				FS.writeword(hfvis,
					item->m_Files[k]->m_FileIndex );

			cur_progress += slot_cost;
			BWindow.SetProgress( cur_progress );
		}

	FS.close(hfmap);
	FS.close(hfvis);
	
	return true;
}

