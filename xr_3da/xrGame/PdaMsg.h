/////////////////////////////////////////////////////
//
// PdaMsg.h - ��������� ���������� ��� ������ PDA
//
/////////////////////////////////////////////////////

#pragma once

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
	int info_index;

	//����� ���������/�������� ���������
	ALife::_TIME_ID	time;
		
} SPdaMessage;