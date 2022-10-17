#pragma once
template <class T>
inline void PtrDelete(T*& p) {
	delete p;
	p = nullptr;
}
