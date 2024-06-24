//-------------------------------------------------------------------
//オブジェクト(動くもの、動かないもの)汎用スーパークラス
//-------------------------------------------------------------------
#include  "BObject.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-------------------------------------------------------------------
//足元接触判定
bool BObject::CheckFoot() const
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
bool BObject::CheckHead() const
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
bool  BObject::CheckLeftSide() const
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
bool  BObject::CheckRightSide() const
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
bool BObject::CheckFallHole() const
{
	if (ge->qa_Map == nullptr) { return false; }//マップがなければ判定しない(できない)
	return pos.y > ge->screen2DHeight - hitBase.y;//キャラクタ上部の座標が画面下の座標を超えたときtrueを返す
}
//-------------------------------------------------------------------
//矩形の座標の中心を中央にして定義する
ML::Box2D BObject::CenterBox(const int& w, const int& h)
{
	return ML::Box2D(-w / 2, -h / 2, w, h);
}
//-------------------------------------------------------------------
//Box2D型の各要素に一定の値を掛ける(拡大用)
ML::Box2D BObject::MultiplyBox2D(const ML::Box2D& box, const float& n)
{
	return ML::Box2D(int(box.x * n), int(box.y * n),
		int(box.w * n), int(box.h * n));
}