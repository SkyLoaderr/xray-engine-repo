#ifndef FixedVectorH
#define FixedVectorH
#pragma once

template <class T, const int dim>
class svector 
{
private:
	T			array[dim];
	u32			count;
public:
	typedef T*	iterator;

	svector() : count(0) 
	{}
	svector(T* p, int c) 
	{ assign(p,c); }

	IC T*		begin()						{ return array;							}
	IC T*		end	 ()						{ return array+count;					}
	IC u32		size()		const			{ return count;							}
	IC void		clear()						{ count=0;								}
	IC void		resize(int c)				{ VERIFY(c<=dim); count=c;				}
	IC void		reserve(int c)				{ }

	IC void		push_back(T e)				{ VERIFY(count<dim); array[count++]=e;	}
	IC void		pop_back()					{ VERIFY(count); count--;				}

	IC T&		operator[] (u32 id)			{ VERIFY(id<count); return array[id];	}
	IC const T&	operator[] (u32 id)	const	{ VERIFY(id<count); return array[id];	}

	IC T&		front()						{ return array[0];						}
	IC T&		back()						{ return array[count-1];				}
	IC T&		last()						{ VERIFY(count<dim); return array[count];}
	IC void		inc	()						{ count++; }
	IC bool		empty()		const			{ return 0==count;	}

	IC void		erase(u32 id)				{
		VERIFY(id<count);
		count--;
		for (u32 i=id; i<count; i++)
			array[i] = array[i+1];
	}
	IC void		erase(T* it)				{ erase(u32(it-begin()));	}

	IC void		insert(u32 id, T& V)
	{
		VERIFY(id<count);
		for (int i=count; i>int(id); i--)	array[i] = array[i-1];
		count++;
		array[id] = V;
	}
	IC void		assign(T* p, int c) { VERIFY(c>0 && c<dim); CopyMemory(array,p,c*sizeof(T)); count=c; }
	IC BOOL		equal (svector<T,dim>& base)
	{
		if (size()!=base.size())	return FALSE;
		for (u32 cmp=0; cmp<size(); cmp++)	if ((*this)[cmp]!=base[cmp])	return FALSE;
		return TRUE;
	}
};

#endif
