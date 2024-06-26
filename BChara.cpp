//-------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-------------------------------------------------------------------
#include  "BChara.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

bool BChara::CheckFoot() const
{
	//当たり判定を基にして足元矩形を生成
	ML::Box2D foot(
		hitBase.x,
		hitBase.y + hitBase.h,
		hitBase.w,
		1);
	foot.Offset(pos);

	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//マップと接触判定
	return map->CheckHit(foot);
}

//-------------------------------------------------------------------
//頭上の当たり判定
bool BChara::CheckHead() const
{
	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//当たり判定を基にして頭上矩形を生成
	ML::Box2D head(
		hitBase.x,
		hitBase.y - 1,
		hitBase.w,
		1);
	head.Offset(pos);

	//マップと接触判定
	return  map->CheckHit(head);
}
//-------------------------------------------------------------------
//左側の当たり判定
bool  BChara::CheckLeftSide() const
{

	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)
	//当たり判定を基にして左1マスの矩形を生成
	ML::Box2D leftSide(
		hitBase.x - 1,
		hitBase.y,
		1,
		hitBase.h);
	leftSide.Offset(pos);

	//マップと接触判定
	return  map->CheckHit(leftSide);
}
//-------------------------------------------------------------------
//右側の当たり判定
bool  BChara::CheckRightSide() const
{
	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//マップがなければ判定しない(できない)

	//当たり判定を基にして右1マスの矩形を生成
	ML::Box2D rightSide(
		hitBase.w + hitBase.x,
		hitBase.y,
		1,
		hitBase.h);
	rightSide.Offset(pos);

	//マップと接触判定
	return  map->CheckHit(rightSide);
}
//-------------------------------------------------------------------
//穴に落ちたかの判定
bool BChara::CheckFallHole() const
{
	if (ge->qa_Map == nullptr) { return false; }//マップがなければ判定しない(できない)
	return pos.y > ge->screen2DHeight - hitBase.y;//キャラクタ上部の座標が画面下の座標を超えたときtrueを返す
}
//-------------------------------------------------------------------
//矩形の座標の中心を中央にして定義する
ML::Box2D BChara::CenterBox(const int& w, const int& h)
{
	return ML::Box2D(-w / 2, -h / 2, w, h);
}
//-------------------------------------------------------------------
//めり込まない移動処理
void BChara::CheckMove(ML::Vec2& e_)
{
	shared_ptr<Map2D::Object> map = ge->qa_Map;
	//マップが存在するか調べてからアクセス
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
//当たり判定(単体)
bool  BChara::CheckHit(BChara::SP ptr)
{
	if (ptr == nullptr) { return false; }
	ML::Box2D me = this->GetHitBase().OffsetCopy(this->GetPos());
	ML::Box2D you = ptr->GetHitBase().OffsetCopy(ptr->GetPos());
#if defined(DEBUG)
	ML::Box2D debugYou = ge->ApplyCamera2D(you);
	ge->debugRect(debugYou, ge->DEBUGRECTMODE::RED);
#endif
	if (you.Hit(me)) {
		ptr->Recieved(this->attackPower);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------
//当たり判定(複数)
bool  BChara::CheckHit(shared_ptr<vector<BChara::SP>> iters)
{
	bool rtv = false;
	for_each(iters->begin(), iters->end(),
		[&](auto iter) {
			ML::Box2D me = this->GetHitBase().OffsetCopy(this->GetPos());
			ML::Box2D you = iter->GetHitBase().OffsetCopy(iter->GetPos());
#if defined(DEBUG)
			ML::Box2D debugYou = ge->ApplyCamera2D(you);
			ge->debugRect(debugYou, ge->DEBUGRECTMODE::RED);
#endif
			if (you.Hit(me)) {
				iter->Recieved(this->attackPower);
				rtv = true;
				return;
			}
		});
	return rtv;
}