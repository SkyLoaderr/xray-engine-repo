#include "StdAfx.h"

#include "MMSound.h"
#include "xrXmlParser.h"

CMMSound::CMMSound(){

}

CMMSound::~CMMSound(){
	m_music.destroy();
	m_whel.destroy();
	m_music_2.destroy();
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
	if (m_play_list.empty())
		return;

	if (m_music._handle())
		m_music.destroy();
	if (m_music_2._handle())
		m_music_2.destroy();

	srand( (unsigned)time( NULL ) );
	int i = rand() % m_play_list.size();

	bool f = true;
	string256 _path;
	string256 _path2;
	strconcat(_path, m_play_list[i].c_str(), "_l.ogg");
	strconcat(_path2, m_play_list[i].c_str(), "_r.ogg");
	f &= FS.exist("$game_sounds$", _path ) ? true : false;	
	f &= FS.exist("$game_sounds$", _path2 ) ? true : false;

	if (f){
		m_music.create(TRUE, _path);
		m_music_2.create(TRUE, _path2);

		if (m_music._handle())
            m_music.play_at_pos(NULL, Fvector().set(-1.f, 0.f, 1.f), sm_2D);

		if (m_music_2._handle())
            m_music_2.play_at_pos(NULL, Fvector().set(1.f, 0.f, 1.f), sm_2D);
	}
	else
	{
        m_music.create	(TRUE, m_play_list[i].c_str());
        if (m_music._handle())
            m_music.play(NULL, sm_2D);
	}
}

void CMMSound::music_Update(){
	if (!m_music._feedback())
		music_Play();
}

void CMMSound::music_Stop(){
    m_music.stop();
}