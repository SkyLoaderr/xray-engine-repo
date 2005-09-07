#include "stdafx.h"
#include "actor_input_handler.h"
#include "actor.h"

void CActorInputHandler::reinit()
{
	m_actor = 0;
}


void CActorInputHandler::install()
{
	m_actor = smart_cast<CActor*>	(Level().CurrentEntity());
	VERIFY(m_actor);

	m_actor->set_input_external_handler(this);
}


void CActorInputHandler::release()
{
	VERIFY(m_actor);
	
	m_actor->set_input_external_handler(0);
	m_actor = 0;
}



