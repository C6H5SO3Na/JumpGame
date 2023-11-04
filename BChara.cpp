//-------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-------------------------------------------------------------------
#include  "BChara.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-------------------------------------------------------------------
//めり込まない移動処理
void BChara::CheckMove(ML::Vec2& e_)
{
	//マップが存在するか調べてからアクセス
	auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
	if (map == nullptr) { return; }//マップがなければ判定しない(できない)

	//横軸に対する移動
	while (e_.x != 0.f) {
		float preX = pos.x;
		if (e_.x >= +1.f) {
			pos.x += 1.f;
			e_.x -= 1.f;
		}
		else if (e_.x <= -1.f) {
			pos.x -= 1.f;
			e_.x += 1.f;
		}
		else {
			pos.x += e_.x;
			e_.x = 0.f;
		}
		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (map->CheckHit(hit)) {
			pos.x = preX;//移動キャンセル
			break;
		}
	}

	//縦軸に対する移動
	while (e_.y != 0.f) {
		float preY = pos.y;
		if (e_.y >= +1.f) {
			pos.y += 1.f;
			e_.y -= 1.f;
		}
		else if (e_.y <= -1.f) {
			pos.y -= 1.f;
			e_.y += 1.f;
		}
		else {
			pos.y += e_.y;
			e_.y = 0.f;
		}
		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (map->CheckHit(hit)) {
			pos.y = preY;//移動キャンセル
			break;
		}
	}
}
//-------------------------------------------------------------------
//足元接触判定
bool BChara::CheckFoot()
{
	//当たり判定を基にして足元矩形を生成
	ML::Box2D foot(
		this->hitBase.x,
		this->hitBase.y + this->hitBase.h,
		this->hitBase.w,
		1);
	foot.Offset(this->pos);

	auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//マップと接触判定
	return map->CheckHit(foot);
}