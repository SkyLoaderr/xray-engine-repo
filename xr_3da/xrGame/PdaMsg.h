/////////////////////////////////////////////////////
//
// PdaMsg.h - ��������� ���������� ��� ������ PDA
//
/////////////////////////////////////////////////////

#pragma once
#include "alife_space.h"

//���� ��������� PDA
enum EPdaMsg {
	ePdaMsgTrade  = u32(0),		//���������
	ePdaMsgGoAway,				//�������� � ����������
	ePdaMsgNeedHelp,			//������� � ������

	ePdaMsgAccept,				//������� �����������
	ePdaMsgDecline,				//����������
	ePdaMsgDeclineRude,			//����� ����������
	ePdaMsgILeave,				//�� ���� ����� � �����

	ePdaMsgInfo,				//��������� �������� ��������� ����������

	ePdaMsgMax
};





//��������� ��� �������� ��������� PDA,
//������������ ��� ������� �����
typedef struct tagSPdaMessage 
{
	EPdaMsg			msg;
	
	//true ���� �� �������� ��������� 
	//� false ���� �� ��� ��������
	bool			receive;
	
	//true, ���� ��������� - ������
	//� false, ���� �����
	bool			question;
	
	//���������� ����� ����������
	INFO_INDEX info_index;

	//����� ���������/�������� ���������
	ALife::_TIME_ID	time;
		
} SPdaMessage;


//���������� � ��������� ���������� �� PDA � �� ����� �������
struct TALK_CONTACT_DATA
{
	TALK_CONTACT_DATA():id(u16(-1)),time(0){};
	TALK_CONTACT_DATA(u16 contact_id, ALife::_TIME_ID contact_time):id(contact_id),time(contact_time){};
	//id ��������� � ������� ��������
	u16				id;
	//����� ��������
	ALife::_TIME_ID	time;
};

DEFINE_VECTOR(TALK_CONTACT_DATA, TALK_CONTACT_VECTOR, TALK_CONTACT_VECTOR_IT);
