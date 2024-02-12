#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//キャラクタ（動くもの）汎用スーパークラス
//-------------------------------------------------------------------
#include "BObject.h"

class BChara : public BObject
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef  shared_ptr<BChara>	SP;
	typedef  weak_ptr<BChara>	WP;

protected:
	ML::Vec2 moveVec;//移動速度

public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラの状態
	Angle_LR angle;

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加すること★★
	BChara()//コンストラクタ
		:angle(Angle_LR::Right)
	{
	}
	virtual ~BChara() {}//デストラクタ

	//キャラクタ共通メソッド
	//めり込まない移動処理
	void CheckMove(ML::Vec2& e_);
};