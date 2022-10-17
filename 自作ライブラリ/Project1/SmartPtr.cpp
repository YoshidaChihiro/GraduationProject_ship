#include"SmartPtr.h"

//template<class T>
//void SmartPtr<T>::Release()
//{
//	if (--(pRefCount) == 0)
//	{
//		delete* ppPtr;
//		delete ppPtr;
//		delete pRefCount;
//	}
//}

//template<class T>
//SmartPtr<T>::SmartPtr(T* src, int add)
//{
//	pRefCount = new unsigned int;
//	*pRefCount = add;
//	ppPtr = new T*;
//	NullPtr = nullptr;
//	if (src)
//		*ppPtr = src;
//	else
//		*ppPtr = NullPtr;
//	AddRef();
//}
//
//template<class T>
//SmartPtr<T>::SmartPtr(const SmartPtr<T>& src)
//{
//	// 相手のポインタをすべてコピー
//	pRefCount = src.pRefCount;
//	ppPtr = src.ppPtr;
//
//	// 自分自身の参照カウンタを増加
//	AddRef();
//
//}

//template<class T>
//template<class T2> SmartPtr<T>::SmartPtr(SmartPtr<T2>& src)
//{
//	// 相手のダブルポインタをコピー
//	pRefCount = src.GetRefPtr();
//	ppPtr = (T**)src.GetPtrPtr();
//
//	// 型チェックコピー
//	*ppPtr = src.GetPtr();
//
//	// 自分自身の参照カウンタを増加
//	AddRef();
//}
//
//template<class T>
//SmartPtr<T>::SmartPtr(const int nullval)
//{
//	pRefCount = new UINT;
//	*pRefCount = 0;
//	ppPtr = new T*;
//	*ppPtr = NullPtr;
//	AddRef();       // 参照カウンタ増加
//}
//
//template<class T>
//void SmartPtr<T>::SetPtr(T* src, int add)
//{
//	// 参照カウンタを減らした後に再初期化
//	Release();
//	pRefCount = new unsigned int;
//	*pRefCount = add;
//	ppPtr = new T*;
//	if (src)
//		*ppPtr = src;
//	else
//		*ppPtr = NullPtr;
//	AddRef();
//}

//template<class T>
//inline void SmartPtr<T>::SwapPtr(SmartPtr<T>& src)
//{
//	T* pTmp = src.GetPtr();
//	*src.ppPtr = *ppPtr;   // ポインタの交換
//	*ppPtr = pTmp;
//}


//template<class T>
//template<class T2>
//inline bool SmartPtr<T>::DownCast(SmartPtr<T2>& src)
//{
//	{
//		// 引数のスマートポインタの持つポインタが、
//		// 自分の登録しているポインタに
//		// ダウンキャスト可能な場合はダウンキャストコピーを実行
//		T* castPtr = dynamic_cast<T*>(src.GetPtr());
//		if (castPtr) {
//			// ダウンキャスト成功
//			// 自分は違う人になるので
//			// 既存の参照カウンタを1つ減少
//			Release();
//			// 新しいポインタと参照カウンタを共有
//			ppPtr = (T**)src.GetPtrPtr();
//			*ppPtr = castPtr;
//			pRefCount = src.GetRefPtr();
//			// 参照カウンタ増加
//			AddRef();
//			return true;
//		}
//
//		return false;
//	}
//}
//template<class T>
//SmartPtr<T>& SmartPtr<T>::operator =(const SmartPtr<T>& src)
//{
//	// 自分自身への代入は不正で意味が無いので
//	// 行わない。
//	if (*src.ppPtr == *ppPtr)
//		return (*this);
//
//	// 自分は他人になってしまうので
//   // 参照カウンタを1つ減少
//	Release();
//
//	// 相手のポインタをコピー
//	pRefCount = src.pRefCount;
//	ppPtr = src.ppPtr;
//
//	// 新しい自分自身の参照カウンタを増加
//	AddRef();
//
//	return (*this);
//}

//template<class T>
//template<class T2> SmartPtr<T>& SmartPtr<T>::operator =(SmartPtr<T2>& src)
//{
//	// 自分自身への代入は不正で意味が無いので
//	// 行わない。
//	if (src.GetPtr() == *ppPtr)
//		return (*this);
//
//	// 自分は他人になってしまうので
//   // 参照カウンタを1つ減少
//	Release();
//
//	// 相手のポインタをコピー
//	pRefCount = src.GetRefPtr();
//	ppPtr = (T**)src.GetPtrPtr();
//
//	// 型チェックコピー
//	*ppPtr = src.GetPtr();
//
//	// 新しい自分自身の参照カウンタを増加
//	AddRef();
//
//	return (*this);
//}


//template<class T>
//SmartPtr<T>& SmartPtr<T>::operator=(const int nullval)
//{
//	// 自分は空な人になってしまうので
//	// 参照カウンタを1つ減少
//	Release();
//
//	// 新規に自分自身を作る
//	pRefCount = new unsigned int(1);
//	ppPtr = new T*;
//	*ppPtr = NullPtr;
//
//	return (*this);
//}

//template<class T>
//bool SmartPtr<T>::operator==(T* val)
//{
//	if (*ppPtr == val)
//		return true;
//	return false;
//}

//template<class T>
//bool SmartPtr<T>::operator!=(T* val)
//{
//	if (*ppPtr != val)
//		return true;
//	return false;
//}
