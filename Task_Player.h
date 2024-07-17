#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include "BChara.h"

namespace Player
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("プレイヤ");	//グループ名
	const  string  defName("NoName");	//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
		DG::Image::SP img;
	};
	//-------------------------------------------------------------------
	class  Object : public  BChara
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);	
		static void  Spawn(const ML::Vec2& pos);//タスク生成&パラメーター指定
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
		//無敵状態管理クラス
		class Invincible {
			bool doFlash = false;// 点滅フラグ
			bool flag = false;//無敵フラグ
			int cnt = 0;//無敵カウント
		public:
			//無敵か否かを取得
			bool isInvincible() const { return flag; }

			//点滅開始
			void startFlash() { doFlash = true; }

			//点滅中かを取得
			bool isFlash() const { return doFlash; }
			//開始
			void start()
			{
				flag = true;
				cnt = 100;
			}

			//処理
			void operation()
			{
				if (!isInvincible()) { return; }
				if (cnt > 0) {
					--cnt;
				}
				else {
					flag = false;
					doFlash = false;
				}
			}
		};

		//体力管理クラス
		class Life {
			int now = 0;
			int max = 0;
		public:
			//コンストラクタ
			Life(const int& now_, const int& max_)
			{
				now = now_;
				max = max_;
			}
			//デフォルトコンストラクタ禁止
			Life() = delete;

			//ライフを加える
			void addNow(const int& n) { now += n; }
			//現在のライフ取得
			int getNow() const { return now; }
			//最大ライフ取得
			int getMax() const { return max; }
			//生きているか否か
			bool isLive() const { return now >= 0; }
		};
	public:
		Life life;
		bool CheckHitEnemyHead();
	private:
		//ポリモーフィズム
		//状態の抽象インターフェース
		class StateBase {
		public:
			virtual ~StateBase() {}
			virtual void think() = 0;
			virtual void move() = 0;
			virtual void anim() = 0;
		private:
			ML::Box2D animTable_;
		};

		//ニュートラル
		class IdleState :public StateBase {
		public:
			IdleState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//歩行
		class WalkState :public StateBase {
		public:
			WalkState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//ジャンプ
		class JumpState :public StateBase {
		public:
			JumpState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//落下
		class FallState :public StateBase {
		public:
			FallState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//被弾
		class HurtState :public StateBase {
		public:
			HurtState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//被弾
		class ClearState :public StateBase {
		public:
			ClearState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		//被弾
		class DeadState :public StateBase {
		public:
			DeadState(Object* ptr) :owner_(ptr) {}
			void think() override;
			void move() override;
			void anim() override;
		private:
			Object* owner_;
		};

		StateBase* state;

		float jumpPow;
		void Think() override;
		void Move() override;
		void Operation();
		void Animation();
		void ChangeState(StateBase* const state);
		void DamageOperation();
		void Recieved(const int& power) override;
		void Dead() override;
		//ライフの増減
		void LifeOperation(const int& addLife);
		void AdjustCameraPlayer() const;

		Invincible invincible;
		XI::VGamePad input;
	};
}