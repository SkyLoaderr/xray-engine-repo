/////////////////////////////////////////////////////
//
// PdaMsg.h - сообщение посылаемое при помощи PDA
//
/////////////////////////////////////////////////////

#pragma once

//типы сообщений PDA
enum EPdaMsg {
	ePdaMsgTrade  = u32(0),		//торговать
	ePdaMsgGoAway,				//убирайся с территории
	ePdaMsgNeedHelp,			//просьба о помощи
	
	ePdaMsgAccept,				//принять предложение
	ePdaMsgDecline,				//отказаться
	ePdaMsgDeclineRude,			//грубо отказаться

	ePdaMsgInfo,				//сообщение передает некоторую информацию

	ePdaMessageMax
};


//структура для описания сообщения PDA,
//используется для ведения логов
typedef struct tagSPdaMessage 
{
	EPdaMsg			msg;
	
	//true если мы получали сообщение 
	//и false если мы его посылали
	bool			receive;
	
	//true, если сообщение - вопрос
	//и false, если ответ
	bool			question;
	
	//порядковый номер информации
	int info_index;

	//время получения/отправки сообщения
	ALife::_TIME_ID	time;
		
} SPdaMessage;