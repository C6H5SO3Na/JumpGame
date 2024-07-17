//-------------------------------------------------------------------
//敵00
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Enemy00.h"
#include "Task_Player.h"
#include "Task_Map2D.h"
#include "sound.h"
#include <assert.h>

namespace Enemy00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		img[0] = DG::Image::Create("data/image/enemy1.png");
		img[1] = DG::Image::Create("data/image/enemy2.png");
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
		res = Resource::Create();

		//★データ初期化
		render2D_Priority[1] = 0.5f;
		render2D_Priority[1] = 0.5f;
		angle = Angle_LR::Left;
		hitBase = CenterBox(64, 64);
		moveVec = ML::Vec2(5.f, 5.f);
		src = ML::Box2D(0, 0, 64, 64);
		score = 100;
		jumpPow = -10.f;
		attackPower = 1;

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		if (!ge->QuitFlag() && nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		Think();
		Move();
		if (ge->qa_Player != nullptr) {
			if (!ge->qa_Player->CheckHitEnemyHead()) {
				CheckHit(ge->qa_Player);
			}
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
	//思考
	void Object::Think()
	{
		//空実装
	}
	//-------------------------------------------------------------------
	//行動
	void Object::Move()
	{
		if (!CheckHitCamera2D()) { return; }//カメラの範囲外のとき、動作しない
		ML::Vec2 est;
		//左右移動
		est.x = moveVec.x * angle;
		//ジャンプ
		if (type == Type::Jumping) {
			if (isHitFloor) {//着地中のみジャンプ開始できる
				fallSpeed = jumpPow;
			}
		}
		est.y += fallSpeed;
		CheckMove(est);

		//方向転換
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
		int animAngleTmp = 64;//drawにおけるh座標 左の場合32
		if (angle == Angle_LR::Right) {
			animAngleTmp = 128;
		}

		int frameInterval = 0;//アニメーションの間隔フレーム
		switch (animKind) {
		case Anim::Move:
			frameInterval = 8;
			drawBase = CenterBox(64, 64);
			src = ML::Box2D((animCnt / frameInterval) % 3 * drawBase.w, animAngleTmp, drawBase.w, drawBase.h);
			break;
		}
	}
	//-------------------------------------------------------------------
	//スプライト描画
	void Object::Draw()
	{
		ML::Box2D draw = drawBase.OffsetCopy(pos);
		draw = ge->ApplyCamera2D(draw);
		res->img[static_cast<int>(type)]->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//受け身の処理
	void Object::Recieved(const int& power)
	{
		Dead();
		ge->score += GetScore();
		se::Play("Explosion");
		ge->CreateEffect(8, pos);//爆発エフェクト
	}
	//-------------------------------------------------------------------
	//カメラとの当たり判定
	bool Object::CheckHitCamera2D() const
	{
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
	//受け身の処理
	void Object::Dead()
	{
		Kill();
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
		return  Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = Finalize();
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
	Resource::~Resource() { Finalize(); }
	//-------------------------------------------------------------------
	//タスク生成&パラメーター指定
	void Object::Spawn(const ML::Vec2& pos, const int& kind)
	{
		auto enemy = Enemy00::Object::Create(true);
		enemy->pos = pos;
		enemy->type = static_cast<Type>(kind);
	}
}