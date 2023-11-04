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
		hitBase.x,
		hitBase.y + hitBase.h,
		hitBase.w,
		1);
	foot.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//マップと接触判定
	return map->CheckHit(foot);
}

//-------------------------------------------------------------------
//頭上の当たり判定
bool BChara::CheckHead()
{
	//当たり判定を基にして頭上矩形を生成
	ML::Box2D head(
		hitBase.x,
		hitBase.y - 1,
		hitBase.w,
		1);
	head.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//マップと接触判定
	return  map->CheckHit(head);
}
//-------------------------------------------------------------------
//左側の当たり判定
bool  BChara::CheckLeftSide()
{
	//当たり判定を基にして左1マスの矩形を生成
	ML::Box2D leftSide(
		hitBase.x - 1,
		hitBase.y,
		1,
		hitBase.h);
	leftSide.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//マップと接触判定
	return  map->CheckHit(leftSide);
}
//-------------------------------------------------------------------
//右側の当たり判定
bool  BChara::CheckRightSide()
{
	//当たり判定を基にして右1マスの矩形を生成
	ML::Box2D rightSide(
		hitBase.w + hitBase.x,
		hitBase.y,
		1,
		hitBase.h);
	rightSide.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//マップと接触判定
	return  map->CheckHit(rightSide);
}
//-------------------------------------------------------------------
//矩形の座標の中心を中央にして定義する
ML::Box2D BChara::CenterBox(int w, int h)
{
	return ML::Box2D(-w / 2, -h / 2, w, h);
}
//-------------------------------------------------------------------
//Box2D型の各要素に一定の値を掛ける(拡大用)
ML::Box2D BChara::MultiplyBox2D(ML::Box2D box, float n)
{
	return ML::Box2D(int(box.x * n), int(box.y * n),
		int(box.w * n), int(box.h * n));
}