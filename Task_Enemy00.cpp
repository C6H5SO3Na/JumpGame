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
		img[0] = DG::Image::Create("data/image/pipo-simpleenemy01b.png");
		img[1] = DG::Image::Create("data/image/pipo-simpleenemy01c.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		for (DG::Image::SP i : img) {
			i.reset();
		}
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
		hitBase = CenterBox(32 * 2, 32 * 2);
		moveVec = ML::Vec2(5.f, 5.f);
		src = ML::Box2D(0, 0, 32, 32);
		score = 100;
		jumpPow = -10.f;

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

		//死亡判定
		if (state == State::Non) {
			Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		Animation();
		Draw();
	}
	//-------------------------------------------------------------------
	//敵の動き
	void Object::Move()
	{
		if (!CheckHitCamera2D() || state == State::Non) { return; }//消滅したかカメラの範囲外のとき、動作しない

		ML::Vec2 est;
		//左右移動
		est.x = moveVec.x * angle;
		//ジャンプ
		if (type == Type::Jumping) {
			if (isHitFloor) {//着地中のみジャンプ開始できる
				//animKind = Anim::Jump;
				fallSpeed = jumpPow;
			}
		}
		est.y += fallSpeed;
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
			fallSpeed = 0.f;//落下速度0
		}
		else {
			fallSpeed += ML::Gravity(32) * 6.f;//重力加速
		}

		//頭上判定
		if (fallSpeed < 0) {//上昇中
			if (CheckHead()) {
				fallSpeed = 0;//上昇力を無効にする
			}
		}

		//穴に落ちたら消滅させる
		if (CheckFallHole()) {
			Dead();
		}
		++moveCnt;
		++animCnt;
	}
	//-------------------------------------------------------------------
	//アニメーション
	void Object::Animation()
	{
		int animAngleTmp = 32;//drawにおけるh座標 左の場合32
		if (angle == Angle_LR::Right) {
			animAngleTmp = 64;
		}

		int frameInterval = 0;//アニメーションの間隔フレーム
		switch (animKind) {
		case Anim::Move:
			frameInterval = 8;
			drawBase = CenterBox(32, 32);
			src = ML::Box2D((animCnt / frameInterval) % 3 * drawBase.w, animAngleTmp, drawBase.w, drawBase.h);
			break;
		}
	}
	//-------------------------------------------------------------------
	//スプライト描画
	void Object::Draw()
	{
		ML::Box2D draw = MultiplyBox2D(drawBase, 2.f).OffsetCopy(pos);
		draw = ge->ApplyCamera2D(draw);
		res->img[static_cast<int>(type)]->Draw(draw, src);
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