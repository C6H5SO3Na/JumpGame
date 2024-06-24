#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BObject : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef  shared_ptr<BObject>	SP;
	typedef  weak_ptr<BObject>	WP;

	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラの状態
	enum class State {
		Normal, Hit, Clear, Dead, Non
	};

	//左右の向き(2D横視点ゲーム専用)
	enum Angle_LR {
		Right = 1, Left = -1
	};

protected:
	ML::Box2D src;//画像の切り取り範囲
	ML::Box2D drawBase;//描画範囲

	bool isHitFloor;//床と当たったか否か
	float fallSpeed;//落下速度

	ML::Vec2 pos;//プレイヤ座標
	ML::Box2D hitBase;//当たり判定範囲

	State state;
	int	moveCnt;//行動処理用カウンタ
	int	animCnt;//アニメーション処理用カウンタ
	int score;//スコア

public:
	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加すること★★
	BObject()//コンストラクタ
		:pos()
		, moveCnt()
		, animCnt()
		, fallSpeed()
		, isHitFloor()
		, state(State::Non)
		, score()
	{
	}
	virtual ~BObject() {}//デストラクタ

	//足元接触判定
	bool CheckFoot() const;
	//頭上接触判定
	bool CheckHead() const;
	//左判定
	bool CheckLeftSide() const;
	//右判定
	bool CheckRightSide() const;
	//穴に落ちたかの判定
	bool CheckFallHole() const;
	//矩形の座標の中心を中央にして定義する
	ML::Box2D CenterBox(const int& w, const int& h);
	//Box2D型の各要素に一定の値を掛ける(拡大用)
	ML::Box2D MultiplyBox2D(const ML::Box2D& box2D, const float& n);
	//座標セット
	void SetPos(const ML::Vec2& p) { pos = p; }
	//座標取得
	ML::Vec2 GetPos() const { return pos; }
	//当たり判定取得
	ML::Box2D GetHitBase() const { return hitBase; }
	//状態取得
	State GetState() const { return state; }
	//スコア取得
	int GetScore()const { return score; }
};