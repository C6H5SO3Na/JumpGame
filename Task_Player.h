#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

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
	class  Object : public  BTask
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
		//状態
		enum class State {
			Normal, Clear, Dead, Non
		};
		//キャラの向き
		enum class Angle_LR {
			Right, Left
		};

		enum class Anim {
			Idle, Walk, Jump, Fall, Clear, Dead
		};
		State state = State::Non;
		ML::Vec2 pos;//プレイヤ座標
		ML::Box2D hitBase;//当たり判定範囲
		int	moveCnt;//行動処理用カウンタ
		int	animCnt;//アニメーション処理用カウンタ

		void Operation();
		void CheckMove(ML::Vec2& e_);
		bool CheckFoot();
		bool CheckHead();
		void Animation();
		ML::Box2D CenterBox(int w, int h);
		ML::Box2D MultiplyBox2D(ML::Box2D box2D, float n);

	private:
		ML::Vec2 moveVec;//移動速度
		ML::Box2D src;//画像の切り取り範囲
		ML::Box2D drawBase;//描画範囲
		ML::Box2D footBase;//足元判定用
		ML::Box2D headBase;//頭上判定用
		Angle_LR angle = Angle_LR::Right;
		Anim animKind = Anim::Idle;
		float fallSpeed = 0.f;
		float jumpPow = 0.f;
		bool isHitFloor = false;
	};
}