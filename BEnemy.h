#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//敵汎用スーパークラス
//-------------------------------------------------------------------
#include "BChara.h"

class BEnemy : public BChara
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef  shared_ptr<BEnemy>	SP;
	typedef  weak_ptr<BEnemy>	WP;

protected:

public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラの状態


	//左右の向き(2D横視点ゲーム専用)



	//メンバ変数に最低限の初期化を行う
	//★★メンバ変数を追加したら必ず初期化も追加すること★★
	BEnemy()//コンストラクタ
	{
	}
	virtual ~BEnemy() {}//デストラクタ

	//敵共通メソッド
	//カメラとの当たり判定
	bool CheckHitCamera2D() const;
	void CheckHitPlayer();
	void Dead();
};