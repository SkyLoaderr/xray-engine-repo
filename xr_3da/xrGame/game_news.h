///////////////////////////////////////////////////////////////
// game_news.h
// ������ ��������: ������� ��������� + ��������
///////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "string_table_defs.h"
#include "object_interfaces.h"

#define NOT_SIMULATION_NEWS		ALife::_NEWS_ID(-1)
#define DEFAULT_NEWS_SHOW_TIME	5000
//-----------------------------------------------------------------------------/
//	News related routines
//-----------------------------------------------------------------------------/


struct GAME_NEWS_DATA : public IPureSerializeObject<IReader,IWriter>
{	
	GAME_NEWS_DATA();

	virtual void load (IReader&);
	virtual void save (IWriter&);


	//id ������� �� ��������� ALife::_NEWS_ID(-1) �� ������� - ��������
	ALife::_NEWS_ID news_id;
	//����� ��� �������� �������
	STRING_INDEX news_text;
	//����� ����������� ������� �� ������ � ���(-1 - �������������)
	int	show_time;

	//����� ��������� �������
	ALife::_TIME_ID		receive_time;

	//��������� ������ � ��� �������� (���� ������)
	LPCSTR texture_name;
	int x1, y1, x2, y2;

	LPCSTR FullText();

private:
	//������ ����� ��������, ����������� ��� ������ ���������
	std::string full_news_text;

	// ���������� ����������� ����
	typedef struct tagNewsTemplate
	{
		shared_str		str;
	} SNewsTemplate;

	// Array of news templates
	typedef std::map<u32, SNewsTemplate>	NewsTemplates;
	NewsTemplates							m_NewsTemplates;

	void LoadNewsTemplates();
};

DEFINE_VECTOR(GAME_NEWS_DATA, GAME_NEWS_VECTOR, GAME_NEWS_IT);