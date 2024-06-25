#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//キャラクタ（動くもの）汎用スーパークラス
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef  shared_ptr<BChara>	SP;
	typedef  weak_ptr<BChara>	WP;

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

	State state_;
	int	moveCnt;//行動処理用カウンタ
	int	animCnt;//アニメーション処理用カウンタ
	int score;//スコア
	int attackPower;
	float maxSpeed;

	ML::Vec2 moveVec;//移動速度

		//めり込まない移動処理
	void CheckMove(ML::Vec2& e_);

	//当たり判定
	bool  CheckHit(BChara::SP ptr);
	bool  CheckHit(shared_ptr<vector<BChara::SP>> ptr);
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
	//スコア取得
	int GetScore()const { return score; }

	virtual void Dead() = 0;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラの状態
	Angle_LR angle;

	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加すること★★
	BChara()//コンストラクタ
		:pos()
		, moveCnt(0)
		, animCnt(0)
		, fallSpeed(0.f)
		, isHitFloor(false)
		, state_(State::Non)
		, score(0)
		, attackPower(0)
		, maxSpeed(0.f)
		, angle(Angle_LR::Right)
	{
	}
	virtual ~BChara() {}//デストラクタ

	//キャラクタ共通メソッド

	virtual void Recieved(const int& power) = 0;

	//座標取得
	ML::Vec2 GetPos() const { return pos; }
	//当たり判定取得
	ML::Box2D GetHitBase() const { return hitBase; }
	//状態取得
	State GetState() const { return state_; }
};