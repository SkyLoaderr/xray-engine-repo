////////////////////////////////////////////////////////////////////////////
//	Module 		: out_stream.h
//	Created 	: 22.10.2004
//  Modified 	: 22.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Simple writer class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ui.h"

class IWriter {
private:
	FILE	*m_handle;

public:
	IC				IWriter		(LPCSTR file_name)
	{
		m_handle	= fopen(file_name,"wb");
		VERIFY		(m_handle);
	}

	virtual			~IWriter	()
	{
		fclose	(m_handle);
	}

			void	w			(const void *dest, const size_t &size)
	{
		fwrite		(dest,size,1,m_handle);
	}

	IC		void	w_u32		(const u32 &value)
	{
		w			(&value,sizeof(value));
	}
};
