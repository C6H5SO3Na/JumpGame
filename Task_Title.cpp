//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Title.h"
#include "Task_StartGame.h"
#include "Task_StageEditor.h"
//#include "randomLib.h"
#include "easing.h"

namespace Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		font = DG::Font::Create("HG創英プレゼンスEB", 30, 60);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		render2D_Priority[1] = 1.0f;
		//イージングセット
		easing::Set("タイトル文字", easing::EXPOOUT, -100, 100, 60);
		//★タスクの生成
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->stage = 1;
		ge->remaining = 5;
		ge->score = 0;
		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto game = StartGame::Object::Create(true);
			//auto game = StageEditor::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		//段階毎の処理
		switch (phase)
		{
		case 0:
			easing::Start("タイトル文字");
			++phase;
			break;

		case 1:
			if (easing::GetState("タイトル文字") == easing::EQ_STATE::EQ_END) {
				++phase;
			}
			break;
		case 2:
			if (inp.B1.down) {
				++phase;
				ge->CreateEffect(99, ML::Vec2());
			}
			break;
		case 3:
			++mainCnt;
			if (mainCnt > 45) {
				++phase;
			}
			break;

		case 4:
			Kill();
		}
		easing::UpDate();
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Title");
		ge->Dbg_ToDisplay(100, 120, "Push B1");

		//テキストボックス
		ML::Box2D textBox(ge->screen2DWidth / 2 - 200, 0, 1000, 1000);
		string text;
		//段階毎の処理
		switch (phase)
		{
		case 2://fall through
		case 1://fall through
		case 0:
			text = "鯛獲る";
			textBox.y += static_cast<int>(easing::GetPos("タイトル文字"));
			res->font->DrawF(textBox, text, DG::Font::x1);
			break;
		}
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