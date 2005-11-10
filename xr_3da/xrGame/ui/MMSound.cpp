#include "StdAfx.h"

#include "MMSound.h"
#include "xrXmlParser.h"

CMMSound::CMMSound(){

}

CMMSound::~CMMSound(){
	m_music.destroy();
	m_whel.destroy();
}

void CMMSound::Init(CUIXml& xml_doc, LPCSTR path){
	string256 _path;	
	m_bRandom = xml_doc.ReadAttribInt(path, 0, "random")? true : false;

	int nodes_num	= xml_doc.GetNodesNum(path, 0, "menu_music");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path,0);
	xml_doc.SetLocalRoot(tab_node);	
	for (int i = 0; i < nodes_num; ++i)
		m_play_list.push_back(xml_doc.Read("menu_music", i, ""));		
	xml_doc.SetLocalRoot(xml_doc.GetRoot());

	
	m_whel.create	(TRUE, xml_doc.Read(strconcat(_path, path,":whel_sound"), 0, "")	);
}

void CMMSound::whell_Play(){
		m_whel.play(NULL, sm_Looped | sm_2D);
}

void CMMSound::whell_Stop(){
		m_whel.stop();
}

void CMMSound::whell_UpdateMoving(float frequency){
	m_whel.set_frequency(frequency);
}

void CMMSound::music_Play(){
	srand( (unsigned)time( NULL ) );

	int i = rand() % m_play_list.size();
	m_music.create	(TRUE, m_play_list[i].c_str());
	bool flag = false;
	if (m_music._handle())
		flag = true;
	m_music.play(NULL, sm_2D);
}

void CMMSound::music_Update(){
	if (!m_music._feedback())
		music_Play();
}

void CMMSound::music_Stop(){
    m_music.stop();
}