#ifndef _FIXEDMAP_H
#define _FIXEDMAP_H
#pragma once

#ifndef SG_REALLOC_ADVANCE
#define SG_REALLOC_ADVANCE	64
#define SG_REALLOC_ALIGN	64
#endif

template<class K, class T>
class FixedMAP
{
public:
	struct TNode {
		K		key;
		T		val;
		TNode	*left,*right;
	};
	typedef void __fastcall callback	(TNode*);
	typedef bool __fastcall callback_cmp(TNode& N1, TNode& N2);
private:
	TNode*		nodes;
	DWORD		pool;
	DWORD		limit;

	IC DWORD	Size(DWORD Count)
	{	return Count*sizeof(TNode);	}

	void		Realloc()
	{
		DWORD	newLimit = limit + SG_REALLOC_ADVANCE;
		VERIFY(newLimit%SG_REALLOC_ADVANCE == 0);
		TNode*	newNodes = (TNode*) _aligned_malloc	(Size(newLimit),SG_REALLOC_ALIGN);
		VERIFY(newNodes);

		ZeroMemory(newNodes, Size(newLimit));
		if (limit) CopyMemory(newNodes, nodes, Size(limit));

		for (DWORD I=0; I<pool; I++)
		{
			VERIFY(nodes);
			TNode*	Nold	= nodes	+ I;
			TNode*	Nnew	= newNodes + I;

			if (Nold->left) {
				DWORD	Lid		= Nold->left  - nodes;
				Nnew->left		= newNodes + Lid;
			}
			if (Nold->right) {
				DWORD	Rid		= Nold->right - nodes;
				Nnew->right		= newNodes + Rid;
			}
		}
		if (nodes) _aligned_free(nodes);

		nodes = newNodes;
		limit = newLimit;
	}

	IC TNode*	Alloc		(const K& key)
	{
		if (pool==limit) Realloc();
		TNode *node = nodes + pool;
		node->key	= key;
		node->right = node->left = 0;
		pool++;
		return node;
	}
	IC TNode*	CreateChild	(TNode* &parent, const K& key)
	{
		DWORD PID	= parent-nodes;
		TNode*	N	= Alloc(key);
		parent		= nodes+PID;
		return	N;
	}

	IC void		recurseLR	(TNode* N, callback CB)
	{
		if (N->left)	recurseLR(N->left,CB);
		CB(N);
		if (N->right)	recurseLR(N->right,CB);
	}
	IC void		recurseRL	(TNode* N, callback CB)
	{
		if (N->right)	recurseRL(N->right,CB);
		CB(N);
		if (N->left)	recurseRL(N->left,CB);
	}
	IC void		getLR		(TNode* N, std::vector<T>&	D)
	{
		if (N->left)	getLR(N->left,D);
		D.push_back		(N->val);
		if (N->right)	getLR(N->right,D);
	}
	IC void		getRL		(TNode* N, std::vector<T>&	D)
	{
		if (N->right)	getRL(N->right,D);
		D.push_back		(N->val);
		if (N->left)	getRL(N->left,D);
	}
	IC void		getLR_P		(TNode* N, std::vector<TNode*>& D)
	{
		if (N->left)	getLR_P(N->left,D);
		D.push_back		(N);
		if (N->right)	getLR_P(N->right,D);
	}
	IC void		getRL_P		(TNode* N, std::vector<TNode*>& D)
	{
		if (N->right)	getRL_P(N->right,D);
		D.push_back		(N);
		if (N->left)	getRL_P(N->left,D);
	}
public:
	FixedMAP() {
		pool	= 0;
		limit	= 0;
		nodes	= 0; 
	}
	~FixedMAP() {
		if (nodes) {
			_aligned_free(nodes);
			nodes	= 0;
		}
	}
	IC TNode*	insert(const K& k) {
		if (pool) {
			TNode*	node = nodes;

			once_more:
			if (k < node->key) {
				if (node->left) {
					node = node->left;
					goto once_more;
				} else {
					TNode* N = CreateChild(node,k);
					node->left = N;
					return N;
				}
			} else if (k > node->key) {
				if (node->right) {
					node = node->right;
					goto once_more;
				} else {
					TNode* N = CreateChild(node,k);
					node->right = N;
					return N;
				}
			} else return node;
			
		} else {
			return Alloc(k);
		}
	}
	IC TNode*	insertInAnyWay(const K& k) {
		if (pool) {
			TNode*	node = nodes;

			once_more:
			if (k <= node->key) {
				if (node->left) {
					node = node->left;
					goto once_more;
				} else {
					TNode* N = CreateChild(node,k);
					node->left = N;
					return N;
				}
			} else {
				if (node->right) {
					node = node->right;
					goto once_more;
				} else {
					TNode* N = CreateChild(node,k);
					node->right = N;
					return N;
				}
			}
		} else {
			return Alloc(k);
		}
	}
	IC TNode*	insert		(const K& k, const T& v)
	{
		TNode*	N	= insert(k);
		N->val		= v;
		return	N;
	}
	IC TNode*	insertInAnyWay(const K& k, const T& v)
	{
		TNode*	N	= insertInAnyWay(k);
		N->val		= v;
		return	N;
	}
	IC void		discard()	{ if (nodes) _aligned_free(nodes); nodes = 0; pool=0; limit=0;	}
	IC DWORD	allocated()	{ return this->limit;				}
	IC void		clear()		{ pool=0;				}
	IC TNode*	begin()		{ return nodes;			}
	IC TNode*	end()		{ return nodes+pool;	}
	IC TNode*	last()		{ return nodes+limit;	}	// for setup only
	IC DWORD	size()		{ return pool;			}
	IC TNode&	operator[] (int v) { return nodes[v]; }

	IC void		traverseLR	(callback CB) 
	{ if (pool) recurseLR(nodes,CB);  }
	IC void		traverseRL	(callback CB) 
	{ if (pool) recurseRL(nodes,CB);  }
	IC void		traverseANY	(callback CB) {
		TNode*	_end = end();
		for (TNode* cur = begin(); cur!=_end; cur++)
			CB(cur);
	}

	IC void		getLR		(std::vector<T>&	D)
	{ if (pool)	getLR(nodes,D); }
	IC void		getLR_P		(std::vector<TNode*>&	D)
	{ if (pool)	getLR_P(nodes,D); }
	IC void		getRL		(std::vector<T>&	D)
	{ if (pool)	getRL(nodes,D); }
	IC void		getRL_P		(std::vector<TNode*>&	D)
	{ if (pool)	getRL_P(nodes,D); }
	IC void		getANY		(std::vector<T>&	D)
	{
		TNode*	_end = end();
		for (TNode* cur = begin(); cur!=_end; cur++) D.push_back(cur->val);
	}
	IC void		getANY_P	(std::vector<TNode*>&	D)
	{
		D.resize			(size());
		TNode** _it			= D.begin();
		TNode*	_end		= end();
		for (TNode* cur = begin(); cur!=_end; cur++,_it++) *it = cur;
	}

	IC void		setup(callback CB) {
		for (int i=0; i<limit; i++)
			CB(nodes+i);
	}
};
#endif