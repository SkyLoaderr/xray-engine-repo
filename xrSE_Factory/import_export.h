////////////////////////////////////////////////////////////////////////////
//	Module 		: import_export.h
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_EXPORT_H
#	define IMPORT_EXPORT_H

//#include "fastdelegate.h"

//class Delegate
//{
//public:
//    template<typename T> Delegate(T* pObject, void (T::*pMemberFunction)())
//        : pUnknownObject_(pObject)
//        , pUnknownMemberFunction_(reinterpret_cast<UnknownMemberFunction>(pMemberFunction))
//    {
//		pProxyFunction_ = proxy<T>;
//	}
//
//    void operator()()
//    {
//        pProxyFunction_(pUnknownObject_, pUnknownMemberFunction_);
//    }
//private:
//    void* pUnknownObject_;
//
//    class Unknown {};
//    typedef void (Unknown::*UnknownMemberFunction)();
//    UnknownMemberFunction pUnknownMemberFunction_;
//
//    void (*pProxyFunction_)(void*, UnknownMemberFunction);
//
//    template<typename T> static void proxy(void* pUnknownObject, UnknownMemberFunction pUnknownMemberFunction)
//    {
//        typedef void (T::*MemberFunction)();
//
//        T* pObject = static_cast<T*>(pUnknownObject);
//        MemberFunction pMemberFunction = reinterpret_cast<MemberFunction>(pUnknownMemberFunction);
//
//        (pObject->*pMemberFunction)();
//    }
//};

//using namespace fastdelegate;

//struct CTestInterface {
//	virtual void							get_string(LPCSTR&) = 0;
//			Delegate						OnGetString;
////			FastDelegate1<LPCSTR&>			OnGetString;
//};

#endif // IMPORT_EXPORT_H

