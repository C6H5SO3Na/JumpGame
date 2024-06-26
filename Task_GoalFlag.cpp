//-------------------------------------------------------------------
//S[๘
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_GoalFlag.h"
#include "Task_Player.h"
#include "Task_Map2D.h"
#include <assert.h>

namespace GoalFlag
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[Xฬ๚ป
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("data/image/Goal.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[Xฬ๐๚
	bool  Resource::Finalize()
	{
		img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//u๚ปv^XNถฌษP๑พฏsค
	bool  Object::Initialize()
	{
		//X[p[NX๚ป
		__super::Initialize(defGroupName, defName, true);
		//\[XNXถฌor\[XคL
		this->res = Resource::Create();

		//f[^๚ป
		render2D_Priority[1] = 0.5f;
		hitBase = drawBase = CenterBox(32 * 2, 32 * 2);
		src = ML::Box2D(0, 0, 32, 32);

		//^XNฬถฌ

		return  true;
	}
	//-------------------------------------------------------------------
	//uIนv^XNมลษP๑พฏsค
	bool  Object::Finalize()
	{
		//f[^^XN๐๚

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//๘ซpฌ^XNฬถฌ
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[ษsค
	void  Object::UpDate()
	{
		Think();
		Move();
	}
	//-------------------------------------------------------------------
	//uQc`ๆvPt[ษsค
	void  Object::Render2D_AF()
	{
		ML::Box2D draw = drawBase.OffsetCopy(pos);
		draw = ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//
	void Object::Oparation()
	{
	}
	//-------------------------------------------------------------------
	//๓ฏg
	void Object::Recieved(const int& power)
	{
		Dead();
	}
	//-------------------------------------------------------------------
	//vl
	void Object::Think()
	{
		//๓ภ
	}
	//-------------------------------------------------------------------
	//sฎ
	void Object::Move()
	{
		//๓ภ
	}
	//-------------------------------------------------------------------
	//S
	void Object::Dead()
	{
		Kill();
	}
	//
	//ศบอ๎{IษฯXsvศ\bh
	//
	//-------------------------------------------------------------------
	//^XNถฌ๛
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWษo^

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYษธsตฝ็Kill
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
	//\[XNXฬถฌ
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
	//^XNถฌ&p[^[w่
	void  Object::Spawn(const ML::Vec2& pos)
	{
		auto enemy = Create(true);
		enemy->pos = pos;
	}
}