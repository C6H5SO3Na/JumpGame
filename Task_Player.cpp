//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
//#define isDebugMode
#include "MyPG.h"
#include "Task_Player.h"
#include "Task_Enemy00.h"
#include "Task_Map2D.h"
#include "Task_Effect00.h"
#include "Task_GoalFlag.h"
#include <assert.h>
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
		state = State::Normal;
		angle = Angle_LR::Right;
		hitBase = CenterBox(28 * 2, 64 * 2);
		moveVec = ML::Vec2(8.f, 8.f);
		jumpPow = -17.f;
		life.now = 3;
		life.max = 5;
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
		Operation();
		if (state != State::Dead && !invincible.flag && !CheckHitEnemyHead()) {//敵を踏んでいなければ
			auto enemies = ge->GetTasks<Enemy00::Object>("敵");
			for (auto it = enemies->begin(); it != enemies->end(); ++it) {
				(*it)->CheckHitPlayer();
			}
		}

		//穴に落ちたら消滅させる
		if (CheckFallHole()) {
			ge->isDead = true;
			state = State::Non;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (invincible.doFlash && moveCnt % 2 == 0) { return; }
		Animation();
		ML::Box2D draw = MultiplyBox2D(drawBase, 2.f).OffsetCopy(pos);
		ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
		//デバッグ用矩形
#if defined(isDebugMode)
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		ge->ApplyCamera2D(me);
		ge->debugRect(me);
#endif
	}

	//-------------------------------------------------------------------
	//プレイヤの操作処理
	void Object::Operation()
	{
		if (state == State::Non) { return; }
		ML::Vec2 est(0.f, 0.f);
		auto inp = ge->in1->GetState();
		static int effectNum = 1;
		switch (state) {
			//動いているとき
		case State::Normal:
			//左右移動
			if (inp.LStick.BL.on) {
				est.x -= moveVec.x;
				angle = Angle_LR::Left;

				//ジャンプ中でないとき歩くアニメーション
				if (fallSpeed == 0.f) {
					ChangeAnim(Anim::Walk);
				}
			}
			if (inp.LStick.BR.on) {
				est.x += moveVec.x;
				angle = Angle_LR::Right;

				//ジャンプ中でないとき歩くアニメーション
				if (fallSpeed == 0.f) {
					ChangeAnim(Anim::Walk);
				}
			}
#if defined(isDebugMode)
			//デバッグ用 エフェクトテスト
			if (inp.LStick.BU.down) {
				++effectNum;
			}
			if (inp.LStick.BD.down) {
				--effectNum;
			}
			if (inp.B3.down) {
				ge->CreateEffect(effectNum, ML::Vec2(pos.x, pos.y + static_cast<float>(drawBase.h) / 2));
			}
#endif

			//ジャンプ
			if (inp.B1.down) {
				if (isHitFloor) {//着地中のみジャンプ開始できる
					ChangeAnim(Anim::Jump);
					fallSpeed = jumpPow;
				}
			}
			if (inp.B1.up && fallSpeed < 0) {//ジャンプボタンが離れた瞬間かつ上昇中の場合
				fallSpeed = 0.f;//落下速度0(小ジャンプ)
			}
			est.y += fallSpeed;

			//完全に止まっているときは止まっているときのアニメーション
			//ジャンプ中、被弾中、歩行中はそれぞれのアニメーションをする
			if (est == ML::Vec2() && animKind != Anim::Jump && animKind != Anim::Hurt) {
				ChangeAnim(Anim::Idle);
			}

			if (CheckHitGoalFlag()) {
				state = State::Clear;
				ChangeAnim(Anim::Clear);
				break;
			}

			CheckMove(est);

			//ジャンプでない落下中は落下のアニメーションをする
			if (!isHitFloor && animKind != Anim::Jump) {
				ChangeAnim(Anim::Fall);
			}

			break;

			//被弾時
		case State::Hit:
			//被弾時には上にいかないようにする
			fallSpeed = max(0.f, fallSpeed);

			//被弾時にも重力を働かせる
			est.y += fallSpeed;
			CheckMove(est);
			break;
			//死亡時
		case State::Dead:
			ge->isDead = true;
			//死亡アニメーションでないときは死亡アニメーションにする
			if (animKind != Anim::Dead) {
				ChangeAnim(Anim::Dead);
			}

			//死亡時には上にいかないようにする
			fallSpeed = max(0.f, fallSpeed);

			//死亡時にも重力を働かせる
			est.y += fallSpeed;
			CheckMove(est);

			break;

		//クリア時
		case State::Clear:
			ge->isClear = true;
			//重力を働かせる
			est.y += fallSpeed;
			CheckMove(est);
			break;
		}

		//頭上判定
		if (fallSpeed < 0.f) {//上昇中
			if (CheckHead()) {
				fallSpeed = 0.f;//上昇力を無効にする
			}
		}

		//足元接触判定
		isHitFloor = CheckFoot();
		if (isHitFloor) {
			//ジャンプアニメーションの場合解除
			if (animKind == Anim::Jump) {
				ChangeAnim(Anim::Idle);
			}
			fallSpeed = 0.f;//落下速度0
		}
		else {
			fallSpeed += ML::Gravity(32) * 6.f;//重力加速
		}
		//カメラの位置を再調整
		//関数化できる?
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
		++moveCnt;

		if (invincible.cnt > 0) {
			--invincible.cnt;
		}
		else {
			invincible.flag = false;
			invincible.doFlash = false;
		}
	}
	//-------------------------------------------------------------------
	//アニメーション
	void Object::Animation()
	{
		drawBase = CenterBox(100, 64);//すべてのアニメーションにおいて縦横比は同じ
		switch (animKind) {
		case Anim::Idle:
		{
			int frameInterval = 8;//アニメーションの間隔フレーム
			src = ML::Box2D((animCnt / frameInterval) % 4 * drawBase.w, 0, drawBase.w, drawBase.h);
			break;
		}
		case Anim::Walk:
		{
			int frameInterval = 8;//アニメーションの間隔フレーム
			src = ML::Box2D((animCnt / frameInterval) % 7 * drawBase.w, drawBase.h, drawBase.w, drawBase.h);
			break;
		}
		case Anim::Jump:
		{
			int frameInterval = 11;//アニメーションの間隔フレーム
			src = ML::Box2D((animCnt / frameInterval) % 6 * drawBase.w, drawBase.h * 2, drawBase.w, drawBase.h);
			if (animCnt / frameInterval / 6 >= 1) {
				ChangeAnim(Anim::Fall);
			}
			break;
		}
		case Anim::Fall:
		{
			int frameInterval = 8;//アニメーションの間隔フレーム
			src = ML::Box2D((animCnt / frameInterval) % 3 * drawBase.w + drawBase.w * 4, 0, drawBase.w, drawBase.h);
			break;
		}
		case Anim::Hurt:
		{
			int frameInterval = 8;//アニメーションの間隔フレーム
			src = ML::Box2D((animCnt / frameInterval) % 4 * drawBase.w, drawBase.h * 3, drawBase.w, drawBase.h);
			if (animCnt > frameInterval * 3) {
				animKind = Anim::Idle;
				state = State::Normal;
				invincible.doFlash = true;
			}
			break;
		}
		case Anim::Dead:
		{
			int frameInterval = 8;//アニメーションの間隔フレーム
			animCnt = min(animCnt, 5 * frameInterval - 1);
			src = ML::Box2D((animCnt / frameInterval) % 5 * drawBase.w, drawBase.h * 4, drawBase.w, drawBase.h);
			break;
		}
		case Anim::Clear:
		{
			int frameInterval = 8;//アニメーションの間隔フレーム
			animCnt = min(animCnt, 5 * frameInterval - 1);
			src = ML::Box2D((animCnt / frameInterval) % 5 * drawBase.w, drawBase.h * 5, drawBase.w, drawBase.h);
			break;
		}
		}
		//左向き反転
		if (angle == Angle_LR::Left &&
			drawBase.w >= 0) {
			drawBase.x = -drawBase.x;
			drawBase.w = -drawBase.w;
		}
		++animCnt;
	}
	//-------------------------------------------------------------------
	//敵の頭との当たり判定
	bool Object::CheckHitEnemyHead()
	{
		if (state == State::Non) { return false; }
		//当たり判定を基にして足元矩形を生成
		ML::Box2D playerFoot(
			hitBase.x,
			hitBase.y + hitBase.h,
			hitBase.w,
			10);
		//敵の頭上と当たり判定
		ML::Box2D  me = playerFoot.OffsetCopy(pos);
		//デバッグ用矩形
#if defined(isDebugMode)
		ge->ApplyCamera2D(me);
		ge->debugRect(me, ge->DEBUGRECTMODE::GREEN);
#endif
		shared_ptr<vector<BEnemy::SP>> enemies = ge->qa_Enemies;
		for (auto it = enemies->begin(); it != enemies->end(); ++it) {
			if ((*it)->state != State::Normal) { continue; }
			//当たり判定を基にして頭上矩形を生成
			ML::Box2D enemyHead(
				(*it)->GetHitBase().x,
				(*it)->GetHitBase().y - 1,
				(*it)->GetHitBase().w,
				10);
			ML::Box2D  you = enemyHead.OffsetCopy((*it)->GetPos());
			//デバッグ用矩形
#if defined(isDebugMode)
			ge->ApplyCamera2D(you);
			ge->debugRect(you, ge->DEBUGRECTMODE::RED);
#endif
			if (you.Hit(me)) {
				if (fallSpeed <= 0.f) {//プレイヤが落下中でなければ無効
					return false;
				}
				(*it)->state = State::Non;
				(*it)->moveCnt = 0;
				(*it)->animCnt = 0;
				ge->score += (*it)->score;
				fallSpeed = jumpPow / 2.f;//敵を踏んだら自動的にジャンプする
				ChangeAnim(Anim::Jump);
				ge->CreateEffect(8, ML::Vec2(static_cast<float>(you.x), static_cast<float>(you.y)));
				return true;
			}
		}
		return false;
	}
	//-------------------------------------------------------------------
	//ゴール旗との当たり判定
	bool Object::CheckHitGoalFlag()
	{
		if (state == State::Non) { return false; }
		//プレイヤ
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		//デバッグ用矩形
#if defined(isDebugMode)
		ge->ApplyCamera2D(me);
		ge->debugRect(me, ge->DEBUGRECTMODE::GREEN);
#endif
		GoalFlag::Object::SP goalFlag = ge->GetTask<GoalFlag::Object>("ゴール旗");
		if (goalFlag->state == State::Non) { return false; }
		//ゴール旗
		ML::Box2D  you = goalFlag->GetHitBase().OffsetCopy(goalFlag->GetPos());
		//デバッグ用矩形
#if defined(isDebugMode)
		ge->ApplyCamera2D(you);
		ge->debugRect(you, ge->DEBUGRECTMODE::RED);
#endif
		if (you.Hit(me)) {
			ge->score += goalFlag->score;
			ge->CreateEffect(2, ML::Vec2(static_cast<float>(me.x), static_cast<float>(me.y)));
			return true;
		}
		return false;
	}
	//-------------------------------------------------------------------
	//アニメーションをチェンジ
	void Object::ChangeAnim(Anim anim)
	{
		animKind = anim;
		if (anim == Anim::Jump || anim == Anim::Dead || anim == Anim::Clear) {
			animCnt = 0;//アニメーション用のカウントをリセット
		}
	}
	//-------------------------------------------------------------------
	//ダメージを受けた時の処理
	void Object::DamageOperation()
	{
		state = State::Hit;
		ChangeAnim(Anim::Hurt);
		moveCnt = 0;
		animCnt = 0;
		invincible.flag = true;
		invincible.cnt = 100;
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
		animKind(Anim::Idle),
		jumpPow(),
		invincible() {	}
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