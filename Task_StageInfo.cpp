//-------------------------------------------------------------------
//Xe[Wξρ^XN
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_StageInfo.h"
#include <assert.h>

namespace StageInfo
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		imgHP = DG::Image::Create("./data/image/health_bar.png");
		imgHPFrame = DG::Image::Create("./data/image/health_bar_decoration.png");
		font = DG::Font::Create("CI", 50, 100);
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		imgHP.reset();
		font.reset();
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

		//^XNΜΆ¬

		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//uQc`ζvPt[Ιs€
	void  Object::Render2D_AF()
	{
		//TODO:@\ͺ
		{
			ML::Box2D textBox(0, 0, 1000, 1000);
			string scoreText = "score:" + to_string(ge->score);
			res->font->DrawF(textBox, scoreText, DG::Font::x1);
		}
		//ΜΝQ[WΜg
		{
			ML::Box2D draw(500, 10, 256, 68);
			ML::Box2D src(0, 0, 256, 68);
			res->imgHPFrame->Draw(draw, src);
		}
		//ΜΝQ[WΜg
		{
			int nowLife, maxLife;
			if (ge->qa_Player == nullptr) {
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