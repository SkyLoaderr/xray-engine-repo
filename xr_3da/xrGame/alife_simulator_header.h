////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_header.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator header
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_interfaces.h"
#include "alife_space.h"

class CALifeSimulatorHeader {
protected:
	u32								m_version;
	ALife::EZoneState				m_state;

public:
	IC								CALifeSimulatorHeader	(LPCSTR				section);
	virtual							~CALifeSimulatorHeader	();
	virtual void					save					(IWriter			&tMemoryStream);
	virtual void					load					(IReader			&tFileStream);
	IC		u32						version					() const;
	IC		ALife::EZoneState		state					() const;
	IC		void					set_state				(const ALife::EZoneState &state);
};

#include "alife_simulator_header_inline.h"