//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
//#define DEBUG
#include "MyPG.h"
#include "Task_Player.h"
#include "Task_Enemy00.h"
#include "Task_Map2D.h"
#include "Task_Effect00.h"
#include "Task_GoalFlag.h"
#include <assert.h>
#include "sound.h"
#include "randomLib.h"

namespace Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/player.png");
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
		angle = Angle_LR::Right;
		hitBase = CenterBox(56, 128);
		maxSpeed = 8.f;
		jumpPow = -17.f;
		attackPower = 1;

		//SE
		se::LoadFile("Jump", "./data/sound/SE/Jump.wav");
		se::LoadFile("Damage", "./data/sound/SE/Damage.wav");
		se::LoadFile("Explosion", "./data/sound/SE/Explosion.wav");
		se::LoadFile("Dead", "./data/sound/SE/Dead.wav");
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
		input = ge->in1->GetState();
		Operation();

		//穴に落ちたら消滅させる
		if (CheckFallHole()) {
			Dead();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (invincible.isFlash() && moveCnt % 2 == 0) { return; }
		Animation();
		ML::Box2D draw = drawBase.OffsetCopy(pos);
		draw = ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
		//デバッグ用矩形
#if defined(DEBUG)
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		me = ge->ApplyCamera2D(me);
		ge->debugRect(me);
#endif
	}

	//-------------------------------------------------------------------
	//プレイヤの操作処理
	void Object::Operation()
	{
		Think();
		Move();
		AdjustCameraPlayer();//カメラの位置を再調整
		++moveCnt;
		++animCnt;
		invincible.operation();
	}
	//-------------------------------------------------------------------
	//カメラの位置を再調整
	void Object::AdjustCameraPlayer() const
	{
		//プレイヤを画面のどこに置くか（今回は画面中央）
		int px = ge->camera2D.w / 2;
		int py = ge->camera2D.h / 2;
		//プレイヤを画面中央に置いたときのカメラの左上座標を求める
		int cpx = static_cast<int>(pos.x) - px;
		int cpy = static_cast<int>(pos.y) - py;
		//カメラの座標を更新
		ge->camera2D.x = cpx;
		ge->camera2D.y = cpy;
		//マップの外側が映らないようにカメラを調整する
		Map2D::Object::SP map = ge->qa_Map;
		if (map != nullptr) {
			map->AdjustCameraPos();
		}
	}
	//-------------------------------------------------------------------
	//思考
	void Object::Think()
	{
		state->think();
	}
	//-------------------------------------------------------------------
	//行動
	void Object::Move()
	{
		if (moveVec.y < 0 || !CheckFoot()) {
			moveVec.y += ML::Gravity(32) * 6.f;//重力加速
		}
		else {
			moveVec.y = 0;
		}

		if (moveVec.x < 0) {
			moveVec.x = min(moveVec.x + 1.f, 0);
		}
		else {
			moveVec.x = max(moveVec.x - 1.f, 0);
		}
		state->move();

		//移動処理
		ML::Vec2 est = moveVec;
		CheckMove(est);
	}
	//ポリモーフィズム
	//止まっている状態
	//-------------------------------------------------------------------
	//思考
	void Object::IdleState::think()
	{
		//足元接触判定
		owner_->isHitFloor = owner_->CheckFoot();
		//ジャンプ
		if (owner_->input.B1.down) {
			if (owner_->isHitFloor) {//着地中のみジャンプ開始できる
				owner_->ChangeState(new JumpState(owner_));
				return;
			}
		}
		//左右移動
		if (owner_->input.LStick.BL.on ||
			owner_->input.LStick.BR.on) {
			owner_->ChangeState(new WalkState(owner_));
			return;
		}

		//ゴール旗の判定
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("オブジェクト", "ゴール旗"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//足場がなかったら落下状態へ
		if (!owner_->isHitFloor) {
			owner_->ChangeState(new FallState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//行動
	void Object::IdleState::move()
	{
	}
	//-------------------------------------------------------------------
	//アニメーション
	void Object::IdleState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 4 * owner_->drawBase.w,
			0,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//ポリモーフィズム
	//歩行状態
	//-------------------------------------------------------------------
	//思考
	void Object::WalkState::think()
	{
		//足元接触判定
		owner_->isHitFloor = owner_->CheckFoot();

		//左右入力がないときはIdleステートへ
		if (owner_->input.LStick.BL.off &&
			owner_->input.LStick.BR.off) {
			owner_->ChangeState(new IdleState(owner_));
			return;
		}

		//ジャンプ
		if (owner_->input.B1.down) {
			if (owner_->isHitFloor) {//着地中のみジャンプ開始できる
				owner_->ChangeState(new JumpState(owner_));
				return;
			}
		}

		//ゴール旗の判定
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("オブジェクト", "ゴール旗"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//落下状態へ
		if (!owner_->isHitFloor) {
			owner_->ChangeState(new FallState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//行動
	void Object::WalkState::move()
	{
		//左右移動
		if (owner_->input.LStick.BL.on) {
			owner_->moveVec.x = -owner_->maxSpeed;
			owner_->angle = Angle_LR::Left;
		}

		if (owner_->input.LStick.BR.on) {
			owner_->moveVec.x = +owner_->maxSpeed;
			owner_->angle = Angle_LR::Right;
		}
	}
	//-------------------------------------------------------------------
	//アニメーション
	void Object::WalkState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 7 * owner_->drawBase.w,
			owner_->drawBase.h,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//ポリモーフィズム
	//ジャンプ状態
	//-------------------------------------------------------------------
	//思考
	void Object::JumpState::think()
	{
		//ゴール判定
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("オブジェクト", "ゴール旗"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//足元接触判定
		owner_->isHitFloor = owner_->CheckFoot();
		if (owner_->moveCnt > 0.f && owner_->isHitFloor) {
			owner_->ChangeState(new IdleState(owner_));
			return;
		}

		//落下状態へ
		if (owner_->moveVec.y >= 0.f) {
			owner_->ChangeState(new FallState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//行動
	void Object::JumpState::move()
	{
		//初速設定&SE
		if (owner_->moveCnt == 0) {
			owner_->moveVec.y = owner_->jumpPow;
			se::Play("Jump");
		}
		//左右移動
		if (owner_->input.LStick.BL.on) {
			owner_->moveVec.x = -owner_->maxSpeed;
			owner_->angle = Angle_LR::Left;
		}

		if (owner_->input.LStick.BR.on) {
			owner_->moveVec.x = +owner_->maxSpeed;
			owner_->angle = Angle_LR::Right;
		}

		//ジャンプ解除
		if (owner_->input.B1.up && owner_->moveVec.y < 0) {//ジャンプボタンが離れた瞬間かつ上昇中の場合
			owner_->moveVec.y = 0.f;//落下速度0(小ジャンプ)
		}

		//頭上接触判定
		if (owner_->CheckHead()) {
			owner_->moveVec.y = 0.f;//ジャンプ解除
		}
	}

	//-------------------------------------------------------------------
	//アニメーション
	void Object::JumpState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 6 * owner_->drawBase.w,
			owner_->drawBase.h * 2,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//ポリモーフィズム
	//落下状態
	//-------------------------------------------------------------------
	//思考
	void Object::FallState::think()
	{
		//ゴール判定
		if (owner_->CheckHit(ge->GetTask<GoalFlag::Object>("オブジェクト", "ゴール旗"))) {
			owner_->ChangeState(new ClearState(owner_));
			return;
		}

		//足元接触判定
		owner_->isHitFloor = owner_->CheckFoot();
		if (owner_->isHitFloor) {
			owner_->ChangeState(new IdleState(owner_));
			return;
		}
	}
	//-------------------------------------------------------------------
	//行動
	void Object::FallState::move()
	{
		//左右移動
		if (owner_->input.LStick.BL.on) {
			owner_->moveVec.x = -owner_->maxSpeed;
			owner_->angle = Angle_LR::Left;
		}

		if (owner_->input.LStick.BR.on) {
			owner_->moveVec.x = +owner_->maxSpeed;
			owner_->angle = Angle_LR::Right;
		}
	}

	//-------------------------------------------------------------------
	//アニメーション
	void Object::FallState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 3 * owner_->drawBase.w + owner_->drawBase.w * 4,
			0,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//ポリモーフィズム
	//被弾状態
	//-------------------------------------------------------------------
	//思考
	void Object::HurtState::think()
	{
		if (owner_->animCnt > 24) {
			owner_->invincible.startFlash();
			owner_->ChangeState(new IdleState(owner_));
		}
	}
	//-------------------------------------------------------------------
	//行動
	void Object::HurtState::move()
	{
		//空実装
	}
	//-------------------------------------------------------------------
	//アニメーション
	void Object::HurtState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->src = ML::Box2D((owner_->animCnt / frameInterval) % 4 * owner_->drawBase.w,
			owner_->drawBase.h * 3,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//ポリモーフィズム
	//ステージクリア状態
	//-------------------------------------------------------------------
	//思考
	void Object::ClearState::think()
	{
		//空実装
	}
	//-------------------------------------------------------------------
	//行動
	void Object::ClearState::move()
	{
		ge->isClear = true;
	}
	//-------------------------------------------------------------------
	//アニメーション
	void Object::ClearState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->animCnt = min(owner_->animCnt, 5 * frameInterval - 1);
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 5 * owner_->drawBase.w,
			owner_->drawBase.h * 5,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//ポリモーフィズム
	//死亡状態
	//-------------------------------------------------------------------
	//思考
	void Object::DeadState::think()
	{
		//空実装
	}
	//-------------------------------------------------------------------
	//行動
	void Object::DeadState::move()
	{
		//死亡時には上にいかないようにする
		owner_->moveVec.y = max(0.f, owner_->moveVec.y);
	}

	//-------------------------------------------------------------------
	//アニメーション
	void Object::DeadState::anim()
	{
		owner_->drawBase = owner_->CenterBox(200, 128);
		int frameInterval = 8;
		owner_->animCnt = min(owner_->animCnt, 5 * frameInterval - 1);
		owner_->src = ML::Box2D(
			(owner_->animCnt / frameInterval) % 5 * owner_->drawBase.w,
			owner_->drawBase.h * 4,
			owner_->drawBase.w,
			owner_->drawBase.h
		);
	}
	//-------------------------------------------------------------------
	//アニメーション
	void Object::Animation()
	{
		state->anim();
		//左向き反転
		if (angle == Angle_LR::Left &&
			drawBase.w >= 0) {
			drawBase.x = -drawBase.x;
			drawBase.w = -drawBase.w;
		}
	}
	//-------------------------------------------------------------------
	//敵の頭との当たり判定
	bool Object::CheckHitEnemyHead()
	{
		bool rtv = false;
		//当たり判定を基にして足元矩形を生成
		ML::Box2D playerFoot(
			hitBase.x,
			hitBase.y + hitBase.h,
			hitBase.w,
			10);
		//敵の頭上と当たり判定
		ML::Box2D  me = playerFoot.OffsetCopy(pos);
		//デバッグ用矩形
#if defined(DEBUG)
		ML::Box2D debugMe = ge->ApplyCamera2D(me);
		ge->debugRect(debugMe, ge->DEBUGRECTMODE::GREEN);
#endif
		auto enemies = ge->qa_Enemies;
		for_each(enemies->begin(), enemies->end(), [&](auto iter) {
			//当たり判定を基にして頭上矩形を生成
			ML::Box2D enemyHead(
				iter->GetHitBase().x,
				iter->GetHitBase().y - 1,
				iter->GetHitBase().w,
				10);
			ML::Box2D  you = enemyHead.OffsetCopy(iter->GetPos());
			//デバッグ用矩形
#if defined(DEBUG)
			ML::Box2D debugYou = ge->ApplyCamera2D(you);
			ge->debugRect(debugYou, ge->DEBUGRECTMODE::RED);
#endif
			if (you.Hit(me)) {
				if (moveVec.y <= 0.f) { return; }//プレイヤが落下中でなければ無効
				iter->Recieved(attackPower);
				moveVec.y = jumpPow / 2.f;//敵を踏んだら自動的にジャンプする
				ChangeState(new JumpState(this));
				rtv = true;
				return;
	}

});
		return rtv;
	}
	//-------------------------------------------------------------------
	//状態変更
	void Object::ChangeState(StateBase* const state_)
	{
		delete this->state;
		this->state = state_;
		moveCnt = 0;
		animCnt = 0;
	}
	//-------------------------------------------------------------------
	//ダメージを受けた時の処理
	void Object::DamageOperation()
	{
		ChangeState(new HurtState(this));
		moveCnt = 0;
		animCnt = 0;
		invincible.start();
		se::Play("Damage");
	}
	//-------------------------------------------------------------------
	//ライフの増減
	void Object::LifeOperation(const int& addLife)
	{
		life.addNow(addLife);
		if (life.getNow() <= 0) {
			Dead();
		}
	}
	//-------------------------------------------------------------------
	//受け身の処理
	void Object::Recieved(const int& power)
	{
		if (invincible.isInvincible()) { return; }
		LifeOperation(-power);
		if (life.getNow() >= 0) {//死んでいないときはダメージを受ける演出を入れる
			DamageOperation();
		}
	}
	//-------------------------------------------------------------------
	//死亡処理
	void Object::Dead()
	{
		Kill();
		ge->isDead = true;
		bgm::Stop("Main");
		se::Play("Dead");
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
	Object::Object() :
		state(new IdleState(this)),
		jumpPow(),
		invincible(),
		life(5, 5) {	}
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
	//-------------------------------------------------------------------
	//タスク生成&パラメーター指定
	void  Object::Spawn(const ML::Vec2& pos)
	{
		auto enemy = Create(true);
		enemy->pos = pos;
	}
}