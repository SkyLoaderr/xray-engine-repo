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
	typedef T*			iterator;
	typedef const T*	const_iterator;

	svector() : count(0) 
	{}
	svector(iterator p, int c) 
	{ assign(p,c); }

	IC iterator	begin()						{ return array;							}
	IC iterator	end	 ()						{ return array+count;					}
	IC const_iterator	begin()	const		{ return array;							}
	IC const_iterator	end	 ()	const		{ return array+count;					}
	IC u32		size()		const			{ return count;							}
	IC void		clear()						{ count=0;								}
	IC void		resize(int c)				{ VERIFY(c<=dim); count=c;				}
	IC void		reserve(int c)				{ }

	IC void		push_back(T e)				{ VERIFY(count<dim); array[count++]=e;	}
	IC void		pop_back()					{ VERIFY(count); count--;				}

	IC iterator&		operator[] (u32 id)			{ VERIFY(id<count); return array[id];	}
	IC const_iterator&	operator[] (u32 id)	const	{ VERIFY(id<count); return array[id];	}

	IC iterator&front()						{ return array[0];						}
	IC iterator&back()						{ return array[count-1];				}
	IC iterator&last()						{ VERIFY(count<dim); return array[count];}
	IC const_iterator&front() const			{ return array[0];						}
	IC const_iterator&back()  const			{ return array[count-1];				}
	IC const_iterator&last()  const			{ VERIFY(count<dim); return array[count];}
	IC void		inc	()						{ count++; }
	IC bool		empty()		const			{ return 0==count;	}

	IC void		erase(u32 id)				{
		VERIFY(id<count);
		count--;
		for (u32 i=id; i<count; i++)
			array[i] = array[i+1];
	}
	IC void		erase(iterator it)				{ erase(u32(it-begin()));	}

	IC void		insert(u32 id, T& V)
	{
		VERIFY(id<count);
		for (int i=count; i>int(id); i--)	array[i] = array[i-1];
		count++;
		array[id] = V;
	}
	IC void		assign(iterator p, int c) { VERIFY(c>0 && c<dim); CopyMemory(array,p,c*sizeof(T)); count=c; }
	IC BOOL		equal (const svector<T,dim>& base) const
	{
		if (size()!=base.size())	return FALSE;
		for (u32 cmp=0; cmp<size(); cmp++)	if ((*this)[cmp]!=base[cmp])	return FALSE;
		return TRUE;
	}
};

#endif