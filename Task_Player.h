//#pragma warning(disable:4996)
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
	public:
		//追加したい変数・メソッドはここに追加する
		enum class Anim {
			Idle, Walk, Jump, Fall, Hurt, Clear, Dead
		};

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
			Life() {};
			//ライフを加える
			void addNow(const int& n) { now += n; }
			//現在のライフ取得
			int getNow() const { return now; }
			//最大ライフ取得
			int getMax() const { return max; }
		};

		void Operation();
		void Animation();
		bool CheckHitEnemyHead();
		bool CheckHitGoalFlag() const;
		void ChangeAnim(const Anim& anim);
		void DamageOperation();
		void Dead();
		//ライフの増減
		void LifeOperation(const int& addLife);

		Invincible invincible;

		Life life;
	private:
		Anim animKind;
		float jumpPow;
	};
}