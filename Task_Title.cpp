//-------------------------------------------------------------------
//^CgζΚ
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
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		font = DG::Font::Create("HGnpv[XEB", 30, 60);
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//uϊ»v^XNΆ¬ΙPρΎ―s€
	bool  Object::Initialize()
	{
		//X[p[NXϊ»
		__super::Initialize(defGroupName, defName, true);
		//\[XNXΆ¬or\[X€L
		this->res = Resource::Create();

		//f[^ϊ»
		render2D_Priority[1] = 1.0f;
		//C[WOZbg
		easing::Set("^CgΆ", easing::EXPOOUT, -100, 100, 60);
		//^XNΜΆ¬
		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ
		ge->stage = 1;
		ge->remaining = 5;
		ge->score = 0;
		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
			auto game = StartGame::Object::Create(true);
			//auto game = StageEditor::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		//iKΜ
		switch (phase)
		{
		case 0:
			easing::Start("^CgΆ");
			++phase;
			break;

		case 1:
			if (easing::GetState("^CgΆ") == easing::EQ_STATE::EQ_END) {
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
	//uQc`ζvPt[Ιs€
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Title");
		ge->Dbg_ToDisplay(100, 120, "Push B1");

		//eLXg{bNX
		ML::Box2D textBox(ge->screen2DWidth / 2 - 200, 0, 1000, 1000);
		string text;
		//iKΜ
		switch (phase)
		{
		case 2:
		case 1:
		case 0:
			text = "βlι";
			textBox.y += static_cast<int>(easing::GetPos("^CgΆ"));
			res->font->DrawF(textBox, text, DG::Font::x1);
			break;
		}
	}

	//
	//ΘΊΝξ{IΙΟXsvΘ\bh
	//
	//-------------------------------------------------------------------
	//^XNΆ¬ϋ
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWΙo^

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYΙΈs΅½ηKill
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
	//\[XNXΜΆ¬
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