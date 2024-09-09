//-------------------------------------------------------------------
//ステージ情報タスク
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_StageInfo.h"
#include <assert.h>

namespace StageInfo
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imgHP = DG::Image::Create("./data/image/health_bar.png");
		imgHPFrame = DG::Image::Create("./data/image/health_bar_decoration.png");
		font = DG::Font::Create("メイリオ", 50, 100);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		imgHP.reset();
		imgHPFrame.reset();
		font.reset();
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
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		DrawScore();
		DrawGauge();
	}

	//-------------------------------------------------------------------
	//スコア描画
	void  Object::DrawScore() const
	{
		ML::Box2D textBox(0, 0, 1000, 1000);
		string scoreText = "score:" + to_string(ge->score);
		res->font->DrawF(textBox, scoreText, DG::Font::x1);
	}

	//-------------------------------------------------------------------
	//体力ゲージ描画
	void  Object::DrawGauge() const
	{
		DrawGaugeFlame();
		DrawGaugeBar();
	}

	//-------------------------------------------------------------------
	//体力ゲージのフレーム描画
	void  Object::DrawGaugeFlame() const
	{
		ML::Box2D draw(500, 10, 256, 68);
		ML::Box2D src(0, 0, 256, 68);
		res->imgHPFrame->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//体力ゲージの中身描画
	void  Object::DrawGaugeBar() const
	{
		int nowLife, maxLife;
		if (ge->qa_Player == nullptr) {//落下したらプレイヤーはいないので
			nowLife = 0;
			maxLife = 1;
		}
		else {
			nowLife = ge->qa_Player->life.getNow();
			maxLife = ge->qa_Player->life.getMax();
		}

		float lifePer =
			static_cast<float>(nowLife) /
			static_cast<float>(maxLife);
		ML::Box2D src(0, 0, static_cast<int>(196.f * lifePer), 68);
		ML::Box2D draw(56 + 500, 10, static_cast<int>(196.f * lifePer), 68);
		res->imgHP->Draw(draw, src);
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
}