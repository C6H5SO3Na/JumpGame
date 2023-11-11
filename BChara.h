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
	ML::Box2D src;//画像の切り取り範囲
	ML::Box2D drawBase;//描画範囲

	bool isHitFloor;//床と当たったか否か
	float fallSpeed;//落下速度
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラの状態
	enum class State {
		Normal, Clear, Dead, Non
	};

	//左右の向き(2D横視点ゲーム専用)
	enum Angle_LR {
		Right = 1, Left = -1
	};

	ML::Vec2 pos;//プレイヤ座標
	ML::Box2D hitBase;//当たり判定範囲

	int	moveCnt;//行動処理用カウンタ
	int	animCnt;//アニメーション処理用カウンタ

	Angle_LR angle;
	State state;
	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加すること★★
	BChara()//コンストラクタ
		:pos()
		, moveCnt(0)
		, animCnt(0)
		, fallSpeed(0.f)
		, isHitFloor(false)
		, angle(Angle_LR::Right)
		, state(State::Non)
	{
	}
	virtual ~BChara() {}//デストラクタ

	//キャラクタ共通メソッド
	//めり込まない移動処理
	void CheckMove(ML::Vec2& e_);
	//足元接触判定
	bool CheckFoot();
	//頭上接触判定
	bool CheckHead();
	//左判定
	bool CheckLeftSide();
	//右判定
	bool CheckRightSide();
	//穴に落ちたかの判定
	bool CheckFallHole();
	//矩形の座標の中心を中央にして定義する
	ML::Box2D CenterBox(int w, int h);
	//Box2D型の各要素に一定の値を掛ける(拡大用)
	ML::Box2D MultiplyBox2D(ML::Box2D box2D, float n);
};