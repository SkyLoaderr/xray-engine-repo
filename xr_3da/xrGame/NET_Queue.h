#pragma once

class	NET_Event
{
public:
	u32					timestamp;
	u16					type;
	u16					destination;
	vector<u8>			data;
public:
	void				import		(NET_Packet& P)
	{
		data.clear		();
		u16				ID;	
		P.r_begin		(ID			);	VERIFY(M_EVENT==ID);
		P.r_u32			(timestamp	);
		P.r_u16			(type		);
		P.r_u16			(destination);

		u32 size		= P.r_elapsed();
		if (size)	
		{
			data.resize		(size);
			P.r				(data.begin(),size);
		}
	}
	void				export		(NET_Packet& P)
	{
		u16	ID			=	M_EVENT;
		P.w_begin		(ID			);
		P.w_u32			(timestamp	);
		P.w_u16			(type		);
		P.w_u16			(destination);
		if (data.size())	P.w(data.begin(),data.size());
	}
	void				implication	(NET_Packet& P) const
	{
		PSGP.memCopy	(P.B.data,data.begin(),data.size());
		P.B.count		= data.size();
		P.r_pos			= 0;
	}
};

IC bool operator < (const NET_Event& A, const NET_Event& B)	{ return A.timestamp<B.timestamp; }

class	NET_Queue_Event
{
	multiset<NET_Event>	queue;	
public:
	IC void				insert		(NET_Packet& P)
	{
		NET_Event		E;
		E.import		(P);
		queue.insert	(E);
	}
	IC BOOL				available	(u32 T)
	{
		if (queue.empty() || (T<queue.begin()->timestamp))	return FALSE;
		else												return TRUE;
	}
	IC void				get			(u16& dest, u16& type, NET_Packet& P)
	{
		const NET_Event& E	= *queue.begin();
		dest				= E.destination;
		type				= E.type;
		E.implication		(P);
		queue.erase			(queue.begin());
	}
};
