#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef  shared_ptr<BChara>	SP;
	typedef  weak_ptr<BChara>	WP;

protected:
	ML::Vec2 moveVec;//移動速度
	ML::Box2D hitBase;//当たり判定範囲
	ML::Box2D src;//画像の切り取り範囲
	ML::Box2D drawBase;//描画範囲
	ML::Box2D footBase;//足元判定用
	ML::Box2D headBase;//頭上判定用
	ML::Box2D leftSideBase;//左判定用
	ML::Box2D rightSideBase;//左判定用

public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラの状態
	enum class State {
		Normal, Clear, Dead, Non
	};

	//左右の向き(2D横視点ゲーム専用)
	enum class Angle_LR {
		Right, Left
	};

	ML::Vec2 pos;//プレイヤ座標

	int	moveCnt;//行動処理用カウンタ
	int	animCnt;//アニメーション処理用カウンタ

	Angle_LR angle_LR;

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加すること★★
	BChara()//コンストラクタ
		:pos()
		, hitBase()
		, moveVec()
		, moveCnt(0)
		, animCnt(0)
		, angle_LR(Angle_LR::Left)
	{
	}
	virtual ~BChara() {}//デストラクタ

	//キャラクタ共通メソッド
	//めり込まない移動処理
	void CheckMove(ML::Vec2& e_);
	//足元接触判定
	bool CheckFoot();
	//左判定
	bool CheckLeftSide();
	//右判定
	bool CheckRightSide();
	//天井判定
	void CheckHitPlayer();
	//矩形の座標の中心を中央にして定義する
	ML::Box2D CenterBox(int w, int h);
	//Box2D型の各要素に一定の値を掛ける(拡大用)
	ML::Box2D MultiplyBox2D(ML::Box2D box2D, float n);
};