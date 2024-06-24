//-------------------------------------------------------------------
//敵汎用スーパークラス
//-------------------------------------------------------------------
#include  "BEnemy.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-------------------------------------------------------------------
//カメラとの当たり判定
bool BEnemy::CheckHitCamera2D() const
{
	if (state == State::Non) { return false; }
	//プレイヤと当たり判定
	ML::Box2D  me = hitBase.OffsetCopy(pos);
	int n = 400;//カメラ矩形より指定した数両端に広げる
	ML::Box2D  you(
		ge->camera2D.x - n,
		ge->camera2D.y,
		ge->camera2D.w + n * 2,
		ge->camera2D.h
	);
	if (you.Hit(me)) {
		return true;
	}
	return false;
}
//-------------------------------------------------------------------
//プレイヤとの当たり判定
void BEnemy::CheckHitPlayer()
{
	if (state == State::Non) { return; }
	//プレイヤと当たり判定
	ML::Box2D  me = hitBase.OffsetCopy(pos);
	Player::Object::SP player = ge->qa_Player;

	if (player->GetState() != State::Normal || player->invincible.isInvincible()) { return; }

	ML::Box2D  you = player->GetHitBase().OffsetCopy(player->GetPos());
	if (you.Hit(me)) {
		player->LifeOperation(-1);
		if (player->GetState() != State::Dead) {//死んでいないときはダメージを受ける演出を入れる
			player->DamageOperation();
		}
	}
}
//-------------------------------------------------------------------
//死亡処理
void BEnemy::Dead()
{
	state = State::Non;
	moveCnt = 0;
	animCnt = 0;
}