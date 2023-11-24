//-------------------------------------------------------------------
//敵00
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Enemy00.h"
#include "Task_Player.h"
#include "Task_Map2D.h"
#include <assert.h>

namespace Enemy00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("data/image/pipo-simpleenemy01b.png");
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
		moveVec = ML::Vec2(5.f, 5.f);
		src = ML::Box2D(0, 0, 32, 32);
		score = 100;
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
		if (state == State::Non) {
			Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		{
			ML::Box2D draw = drawBase.OffsetCopy(pos);
			ge->ApplyCamera2D(draw);
			res->img->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//敵の動き
	void Object::Move()
	{
		if (!CheckHitCamera2D() || state == State::Non) { return; }//消滅したかカメラの範囲外のとき、動作しない
		ML::Vec2 est(0.f, 0.f);
		auto inp = ge->in1->GetState();
		//左右移動
		est.x = moveVec.x * angle;
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
		isHitFloor = CheckFoot();
		if (isHitFloor) {
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

		//穴に落ちたら消滅させる
		if (CheckFallHole()) {
			state = State::Non;
		}
		++moveCnt;
		++animCnt;
	}
<<<<<<< HEAD


=======
	//-------------------------------------------------------------------
	//プレイヤとの当たり判定
	void Object::CheckHitPlayer()
	{
		if (state == State::Non) { return; }
		//プレイヤと当たり判定
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		auto player = ge->GetTask<Player::Object>(Player::defGroupName);

		if (player->state != State::Normal) { return; }

		ML::Box2D  you = player->hitBase.OffsetCopy(player->pos);
		if (you.Hit(me)) {
			player->state = State::Dead;
			player->moveCnt = 0;
			player->animCnt = 0;
		}
	}
	//-------------------------------------------------------------------
	//カメラとの当たり判定
	bool Object::CheckHitCamera2D()
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
>>>>>>> 0c9e8fdf07396f966f2aa9e10c915d01e9ee85e6
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