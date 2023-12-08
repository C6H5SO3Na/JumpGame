//-------------------------------------------------------------------
//�^�C�g�����
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
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		font = DG::Font::Create("HG�n�p�v���[���XEB", 30, 60);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		render2D_Priority[1] = 1.0f;
		//�C�[�W���O�Z�b�g
		easing::Set("�^�C�g������", easing::EXPOOUT, -100, 100, 60);
		//���^�X�N�̐���
		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->stage = 1;
		ge->remaining = 5;
		ge->score = 0;
		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto game = StartGame::Object::Create(true);
			//auto game = StageEditor::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		//�i�K���̏���
		switch (phase)
		{
		case 0:
			easing::Start("�^�C�g������");
			++phase;
			break;

		case 1:
			if (easing::GetState("�^�C�g������") == easing::EQ_STATE::EQ_END) {
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
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ge->Dbg_ToDisplay(100, 100, "Title");
		ge->Dbg_ToDisplay(100, 120, "Push B1");

		//�e�L�X�g�{�b�N�X
		ML::Box2D textBox(ge->screen2DWidth / 2 - 200, 0, 1000, 1000);
		string text;
		//�i�K���̏���
		switch (phase)
		{
		case 2://fall through
		case 1://fall through
		case 0:
			text = "��l��";
			textBox.y += static_cast<int>(easing::GetPos("�^�C�g������"));
			res->font->DrawF(textBox, text, DG::Font::x1);
			break;
		}
	}

	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^

			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
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
	//���\�[�X�N���X�̐���
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