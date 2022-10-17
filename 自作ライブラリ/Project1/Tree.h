#pragma once
#include"SmartPtr.h"
#include "Vector.h"
#include<vector>
#include<list>
#include<windows.h>
namespace Tree
{

template <class T>
class TreeSpace;

//分木登録オブジェクト
template <class  T>
class TreeObject
{
public:
	TreeSpace<T>* pSpace;			// 登録空間
	T* pObject;				// 判定対象オブジェクト
	SmartPtr<TreeObject<T> > spPre;	// 前のOBJECT_FOR_TREE構造体
	SmartPtr<TreeObject<T> > spNext;	// 次のOBJECT_FOR_TREE構造体

public://メンバ関数
	//コンストラクタ
	TreeObject()
	{
		pSpace = nullptr;
		pObject = nullptr;
	}
	//デストラクタ
	virtual ~TreeObject(){}

	// 自らリストから外れる
	bool Remove()
	{
		// すでに逸脱している時は処理終了
		if (!pSpace)
			return false;

		// 自分を登録している空間に自身を通知
		if (!pSpace->OnRemove(this))
			return false;

		// 逸脱処理
		// 前後のオブジェクトを結びつける
		if (spPre.GetPtr() != nullptr)
		{
			spPre->spNext = spNext;
			spPre.SetPtr(nullptr);
		}
		if (spNext.GetPtr() != nullptr)
		{
			spNext->spPre = spPre;
			spNext.SetPtr(nullptr);
		}
		pSpace = nullptr;
		return true;
	}

	// 空間を登録
	void RegistCell(TreeSpace<T>* pSpace)
	{
		this->pSpace = pSpace;
	}

	// 次のオブジェクトへのスマートポインタを取得
	SmartPtr<TreeObject<T> >& GetNextObj()
	{
		return spNext;
	}
};

// 線形8分木空間管理クラス
#define CLINER8TREEMANAGER_MAXLEVEL		7//最大空間分割数
template <class T>
class CLiner8TreeManager
{
protected:
	unsigned int m_uiDim;
	TreeSpace<T>** ppCellAry;	// 線形空間ポインタ配列
	unsigned int m_iPow[CLINER8TREEMANAGER_MAXLEVEL + 1];	// べき乗数値配列
	Vector3 m_W;	// 領域の幅
	Vector3 m_RgnMin;	// 領域の最小値
	Vector3 m_RgnMax;	// 領域の最大値
	Vector3 m_Unit;		// 最小領域の辺の長さ
	DWORD m_dwCellNum;		// 空間の数
	unsigned int m_uiLevel;			// 最下位レベル

public:
	// コンストラクタ
	CLiner8TreeManager()
	{
		m_uiDim = 0;
		ppCellAry = nullptr;
		m_W.x = m_W.y = m_W.z = 1.0f;
		m_RgnMin.x = m_RgnMin.y = m_RgnMin.z = 0.0f;
		m_RgnMax.x = m_RgnMax.y = m_RgnMax.z = 1.0f;
		m_Unit.x = m_Unit.y = m_Unit.z = 1.0f;
		m_dwCellNum = 0;
		m_uiLevel = 0;
	}

	// デストラクタ
	virtual ~CLiner8TreeManager()
	{
		DWORD i;
		for (i = 0; i < m_dwCellNum; i++) {
			if (ppCellAry[i] != nullptr)
				delete ppCellAry[i];
		}
		delete[] ppCellAry;
	}

	// 線形8分木配列を構築する
	bool Initialize(const unsigned int Level, const Vector3& Min, const Vector3& Max)
	{
		// 設定最高レベル以上の空間は作れない
		if (Level >= CLINER8TREEMANAGER_MAXLEVEL)
			return false;

		// 各レベルでの空間数を算出
		int i;
		m_iPow[0] = 1;
		for (i = 1; i < CLINER8TREEMANAGER_MAXLEVEL + 1; i++)
			m_iPow[i] = m_iPow[i - 1] * 8;

		// Levelレベル（0基点）の配列作成
		m_dwCellNum = (m_iPow[Level + 1] - 1) / 7;
		ppCellAry = new TreeSpace<T>*[m_dwCellNum];
		ZeroMemory(ppCellAry, sizeof(TreeSpace<T>*) * m_dwCellNum);

		// 領域を登録
		m_RgnMin = Min;
		m_RgnMax = Max;
		m_W = m_RgnMax - m_RgnMin;
		m_Unit = m_W / ((float)(1 << Level));

		m_uiLevel = Level;

		return true;
	}

	// オブジェクトを登録する
	bool Regist(const Vector3& Min, const Vector3& Max, SmartPtr<TreeObject<T> >& spTO)
	{
		// オブジェクトの境界範囲から登録モートン番号を算出
		DWORD Elem = GetMortonNumber(Min, Max);
		if (Elem < m_dwCellNum) {
			// 空間が無い場合は新規作成
			if (!ppCellAry[Elem])
				CreateNewCell(Elem);
			return ppCellAry[Elem]->Push(spTO);
		}
		return false;	// 登録失敗
	}

	// 衝突判定リストを作成する
	inline DWORD GetAllCollisionList(std::vector<T*>& ColVect)
	{
		// リスト（配列）は必ず初期化します
		ColVect.clear();

		// ルート空間の存在をチェック
		if (ppCellAry[0] == nullptr)
			return 0;	// 空間が存在していない

		// ルート空間を処理
		std::list<T*> ColStac;
		GetCollisionList(0, ColVect, ColStac);

		return (DWORD)ColVect.size();
	}

	// 特定の位置から衝突の可能性があるリストを作成する
	DWORD GetTargetCollisionList(std::vector<T*>& ColVect, const Vector3& min,const Vector3& max)
	{
		// リスト（配列）は必ず初期化します
		ColVect.clear();
		auto Elem = GetMortonNumber(min,max);
		// ルート空間の存在をチェック
		if (ppCellAry[Elem] == nullptr)
			return 0;	// 空間が存在していない

		// ルート空間を処理
		GetCollisionList2(Elem, ColVect);

		if (Elem == 0)
			return (DWORD)ColVect.size();
		Elem = (Elem - 1) >> 3;
		if (ppCellAry[Elem] == nullptr)
			return (DWORD)ColVect.size();

		GetBackCollisionList(Elem, ColVect);

		return (DWORD)ColVect.size();
	}


protected:
	// 空間内で衝突リストを作成する
	inline bool GetCollisionList(const DWORD& Elem, std::vector<T*>& ColVect, std::list<T*>& ColStac)
	{
		std::list<T*>::iterator it;
		// ① 空間内のオブジェクト同士の衝突リスト作成
		SmartPtr<TreeObject<T> > spOFT1 = ppCellAry[Elem]->GetFirstObj();
		while (spOFT1.GetPtr() != nullptr)
		{
			SmartPtr<TreeObject<T> > spOFT2 = spOFT1->spNext;
			while (spOFT2 != nullptr) {
				// 衝突リスト作成
				ColVect.push_back(spOFT1->pObject);
				ColVect.push_back(spOFT2->pObject);
				spOFT2 = spOFT2->spNext;
			}
			// ② 衝突スタックとの衝突リスト作成
			for (it = ColStac.begin(); it != ColStac.end(); ++it) {
				ColVect.push_back(spOFT1->pObject);
				ColVect.push_back(*it);
			}
			spOFT1 = spOFT1->spNext;
		}

		bool ChildFlag = false;
		// ③ 子空間に移動
		DWORD ObjNum = 0;
		DWORD i, NextElem;
		for (i = 0; i < 8; i++) {
			NextElem = Elem * 8 + 1 + i;
			if (NextElem < m_dwCellNum && ppCellAry[Elem * 8 + 1 + i]) {
				if (!ChildFlag) {
					// ④ 登録オブジェクトをスタックに追加
					spOFT1 = ppCellAry[Elem]->GetFirstObj();
					while (spOFT1.GetPtr()) {
						ColStac.push_back(spOFT1->pObject);
						ObjNum++;
						spOFT1 = spOFT1->spNext;
					}
				}
				ChildFlag = true;
				GetCollisionList(Elem * 8 + 1 + i, ColVect, ColStac);	// 子空間へ
			}
		}

		// ⑤ スタックからオブジェクトを外す
		if (ChildFlag) {
			for (i = 0; i < ObjNum; i++)
				ColStac.pop_back();
		}

		return true;
	}
	bool GetCollisionList2(DWORD Elem, std::vector<T*>& ColVect)
	{
		std::list<T*>::iterator it;
		// ① 空間内のオブジェクト同士の衝突リスト作成
		SmartPtr<TreeObject<T> > spOFT1 = ppCellAry[Elem]->GetFirstObj();
		while (spOFT1.GetPtr() != NULL)
		{
			ColVect.push_back(spOFT1->pObject);
			spOFT1 = spOFT1->spNext;
		}

		// ③ 子空間に移動
		DWORD ObjNum = 0;
		DWORD i, NextElem;
		for (i = 0; i < 8; i++) {
			NextElem = Elem * 8 + 1 + i;
			if (NextElem < m_dwCellNum && ppCellAry[Elem * 8 + 1 + i]) {
				GetCollisionList2(Elem * 8 + 1 + i, ColVect);	// 子空間へ
			}
		}
		return true;
	}

	// 子空間から遡った衝突リストを作成する
	bool GetBackCollisionList(DWORD Elem, std::vector<T*>& ColVect)
	{
		std::list<T*>::iterator it;
		// ① 空間内のオブジェクト同士の衝突リスト作成
		SmartPtr<TreeObject<T> > spOFT1 = ppCellAry[Elem]->GetFirstObj();
		while (spOFT1.GetPtr() != NULL)
		{
			ColVect.push_back(spOFT1->pObject);
			spOFT1 = spOFT1->spNext;
		}

		// ③ 親空間に移動
		DWORD  NextElem;

		if (Elem > 0)
		{
			NextElem = (Elem - 1) >> 3;
			if (ppCellAry[NextElem]) {
				GetBackCollisionList(NextElem, ColVect);	// 子空間へ
			}
		}

		return true;
	}



	// 空間を生成
	bool CreateNewCell(DWORD Elem)
	{
		// 引数の要素番号
		while (!ppCellAry[Elem])
		{
			// 指定の要素番号に空間を新規作成
			ppCellAry[Elem] = new TreeSpace<T>;

			// 親空間にジャンプ
			Elem = (Elem - 1) >> 3;
			if (Elem >= m_dwCellNum) break;
		}
		return true;
	}

	// 座標から空間番号を算出
	DWORD GetMortonNumber(const Vector3& Min, const Vector3& Max)
	{
		// 最小レベルにおける各軸位置を算出
		DWORD LT = GetPointElem(Min + 100);
		DWORD RB = GetPointElem(Max + 100);

		// 空間番号を引き算して
		// 最上位区切りから所属レベルを算出
		DWORD Def = RB ^ LT;
		unsigned int HiLevel = 1;
		unsigned int i;
		for (i = 0; i < m_uiLevel; i++)
		{
			DWORD Check = (Def >> (i * 3)) & 0x7;
			if (Check != 0)
				HiLevel = i + 1;
		}
		DWORD SpaceNum = RB >> (HiLevel * 3);
		DWORD AddNum = (m_iPow[m_uiLevel - HiLevel] - 1) / 7;
		SpaceNum += AddNum;

		if (SpaceNum > m_dwCellNum)
			return 0xffffffff;

		return SpaceNum;
	}

	// ビット分割関数
	DWORD BitSeparateFor3D(const BYTE n)
	{
		DWORD s = n;
		s = (s | s << 8) & 0x0000f00f;
		s = (s | s << 4) & 0x000c30c3;
		s = (s | s << 2) & 0x00249249;
		return s;
	}

	// 3Dモートン空間番号算出関数
	DWORD Get3DMortonNumber(const BYTE x,const BYTE y,const BYTE z)
	{
		return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
	}

	// 座標→線形8分木要素番号変換関数
	DWORD GetPointElem(const Vector3& p)
	{
		return Get3DMortonNumber(
			(BYTE)((p.x - m_RgnMin.x) / m_Unit.x),
			(BYTE)((p.y - m_RgnMin.y) / m_Unit.y),
			(BYTE)((p.z - m_RgnMin.z) / m_Unit.z)
		);
	}

};

//分木の空間クラス
template <class T>
class TreeSpace
{
protected:
	SmartPtr<TreeObject< T >> smartPtrLatest;//最新登録オブジェクトへのスマートポインタ
public:
	//コンストラクタ
	TreeSpace() {};

	//デストラクタ
	virtual ~TreeSpace()
	{
		if (smartPtrLatest.GetPtr() != nullptr) ResetLink(smartPtrLatest);
	}

	// リンクを全てリセットする
	void ResetLink(SmartPtr<TreeObject<T> >& spTO)
	{
		if (spTO->spNext.GetPtr() != nullptr)
			ResetLink(spTO->spNext);
		spTO.SetPtr(nullptr);		// スマートポインタリセット		
	}

	//オブジェクトをプッシュ
	bool Push(SmartPtr<TreeObject<T> >& spTO)
	{
		if (spTO.GetPtr() == nullptr) return false;	// 無効オブジェクトは登録しない
		if (spTO->pSpace == this) return false;	// 2重登録チェック
		if (smartPtrLatest.GetPtr() == nullptr) {
			smartPtrLatest = spTO;	// 空間に新規登録
		}
		else
		{
			// 最新OFTオブジェクトを更新
			spTO->spNext = smartPtrLatest;
			smartPtrLatest->spPre = spTO;
			smartPtrLatest = spTO;
		}
		spTO->RegistCell(this);	// 空間を登録
		return true;
	}

	inline SmartPtr<TreeObject<T> >& GetFirstObj()
	{
		return smartPtrLatest;
	}

	// 削除されるオブジェクトをチェック
	bool OnRemove(TreeObject<T>* pRemoveObj)
	{
		if (smartPtrLatest.GetPtr() == pRemoveObj) {
			// 次のオブジェクトに挿げ替え
			if (smartPtrLatest.GetPtr() != nullptr)
				smartPtrLatest = smartPtrLatest->GetNextObj();
		}
		return true;
	}

};

}
