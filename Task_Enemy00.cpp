//-------------------------------------------------------------------
//敵00
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Enemy00.h"
#include "Task_Player.h"
#include "Task_Map2D.h"

namespace Enemy00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/boxRed.bmp");
		img0 = DG::Image::Create("./data/image/boxBlue.bmp");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		render2D_Priority[1] = 0.5f;
		state = State::Normal;
		angle = Angle_LR::Left;
		hitBase = drawBase = CenterBox(32 * 2, 32 * 2);
		footBase = ML::Box2D(hitBase.x, hitBase.h + hitBase.y, hitBase.w, 1);
		headBase = ML::Box2D(hitBase.x, hitBase.y - 1, hitBase.w, 1); //1マス上
		leftSideBase = ML::Box2D(hitBase.x - 1, hitBase.y, 1, hitBase.h);//左判定用
		rightSideBase = ML::Box2D(hitBase.w + hitBase.x, hitBase.y, 1, hitBase.h);//右判定用
		moveVec = ML::Vec2(5.f, 5.f);
		src = ML::Box2D(0, 0, 64, 128);
		//jumpPow = -17.f;

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		Move();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		{
			ML::Box2D draw = drawBase.OffsetCopy(pos);
			ge->ApplyCamera2D(draw);
			res->img0->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//敵の動き
	void Object::Move()
	{
		if (state == State::Non) { return; }
		ML::Vec2 est(0.f, 0.f);
		auto inp = ge->in1->GetState();
		//左右移動
		if (angle == Angle_LR::Left) {
			est.x = -moveVec.x;
		}
		else {
			est.x = moveVec.x;
		}
		////ジャンプ
		//if (inp.B1.down) {
		//	if (hitFlag) {//着地中のみジャンプ開始できる
		//		animKind = Anim::Jump;
		//		fallSpeed = jumpPow;
		//	}
		//}
		//if (inp.B1.up && fallSpeed < 0) {//ジャンプボタンが離れた瞬間かつ上昇中の場合
		//	fallSpeed = 0.f;//落下速度0(小ジャンプ)
		//}
		est.y += fallSpeed;

		////完全に止まっているときは止まっているときのアニメーション
		////ジャンプ中はジャンプのアニメーションをする
		//if (est == ML::Vec2() && animKind != Anim::Jump) {
		//	animKind = Anim::Idle;
		//}

		CheckMove(est);

		if (CheckLeftSide()) {
			angle = Angle_LR::Right;
		}
		if (CheckRightSide()) {
			angle = Angle_LR::Left;
		}

		//足元接触判定
		hitFlag = CheckFoot();
		if (hitFlag) {
			////ジャンプアニメーションの場合解除
			//if (animKind == Anim::Jump) {
			//	animKind = Anim::Idle;
			//}
			fallSpeed = 0.f;//落下速度0
		}
		else {
			fallSpeed += ML::Gravity(32) * 6.f;//重力加速
		}

		//頭上判定
		//if (fallSpeed < 0) {//上昇中
		//	if (CheckHead()) {
		//		fallSpeed = 0;//上昇力を無効にする
		//	}
		//}
		//カメラの位置を再調整
		{
			//プレイヤを画面のどこに置くか（今回は画面中央）
			int px = ge->camera2D.w / 2;
			int py = ge->camera2D.h / 2;
			//プレイヤを画面中央に置いたときのカメラの左上座標を求める
			int cpx = int(this->pos.x) - px;
			int cpy = int(this->pos.y) - py;
			//カメラの座標を更新
			ge->camera2D.x = cpx;
			ge->camera2D.y = cpy;
			//マップの外側が映らないようにカメラを調整する
			if (auto map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName)) {
				map->AdjustCameraPos();
			}
		}
		++moveCnt;
		++animCnt;
	}
	//-------------------------------------------------------------------
	//めり込まない移動処理
	void Object::CheckMove(ML::Vec2& e_)
	{
		//マップが存在するか調べてからアクセス
		auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
		if (map == nullptr) { return; }//マップがなければ判定しない(できない)

		//横軸に対する移動
		while (e_.x != 0.f) {
			float preX = pos.x;
			if (e_.x >= +1.f) { pos.x += 1.f;		e_.x -= 1.f; }
			else if (e_.x <= -1.f) { pos.x -= 1.f;		e_.x += 1.f; }
			else { pos.x += e_.x;	e_.x = 0.f; }
			ML::Box2D hit = hitBase.OffsetCopy(pos);
			if (map->CheckHit(hit)) {
				pos.x = preX;//移動キャンセル
				break;
			}
		}

		//縦軸に対する移動
		while (e_.y != 0.f) {
			float preY = pos.y;
			if (e_.y >= +1.f) { pos.y += 1.f;		e_.y -= 1.f; }
			else if (e_.y <= -1.f) { pos.y -= 1.f;		e_.y += 1.f; }
			else { pos.y += e_.y;	e_.y = 0.f; }
			ML::Box2D hit = hitBase.OffsetCopy(pos);
			if (map->CheckHit(hit)) {
				pos.y = preY;//移動キャンセル
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//足元の当たり判定
	bool  Object::CheckFoot()
	{
		//マップが存在するか調べてからアクセス
		auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
		ML::Box2D  hit = footBase.OffsetCopy(pos);
		return  map->CheckHit(hit);
	}
	//-------------------------------------------------------------------
	//左側の当たり判定
	bool  Object::CheckLeftSide()
	{
		//マップが存在するか調べてからアクセス
		auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
		ML::Box2D  hit = leftSideBase.OffsetCopy(pos);
		return  map->CheckHit(hit);
	}
	//-------------------------------------------------------------------
	//右側の当たり判定
	bool  Object::CheckRightSide()
	{
		//マップが存在するか調べてからアクセス
		auto map = ge->GetTask<Map2D::Object>("フィールド", "マップ");
		ML::Box2D  hit = rightSideBase.OffsetCopy(pos);
		return  map->CheckHit(hit);
	}
	//プレイヤとの当たり判定
	void Object::CheckHitPlayer()
	{
		if (state == State::Non) { return; }
		//プレイヤと当たり判定
		ML::Box2D  me = hitBase.OffsetCopy((int)pos.x, (int)pos.y);
		auto player = ge->GetTask<Player::Object>(Player::defGroupName);

		if (player->state != Player::Object::State::Normal) { return; }

		ML::Box2D  you = player->hitBase.OffsetCopy((int)player->pos.x, (int)player->pos.y);
		if (you.Hit(me)) {
			player->state = Player::Object::State::Dead;
			player->moveCnt = 0;
			player->animCnt = 0;
		}
	}
	//-------------------------------------------------------------------
	//矩形の座標の中心を中央にして定義する
	ML::Box2D Object::CenterBox(int w, int h)
	{
		return ML::Box2D(-w / 2, -h / 2, w, h);
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}