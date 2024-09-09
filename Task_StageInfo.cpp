//-------------------------------------------------------------------
//�X�e�[�W���^�X�N
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_StageInfo.h"
#include <assert.h>

namespace StageInfo
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imgHP = DG::Image::Create("./data/image/health_bar.png");
		imgHPFrame = DG::Image::Create("./data/image/health_bar_decoration.png");
		font = DG::Font::Create("���C���I", 50, 100);
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		imgHP.reset();
		imgHPFrame.reset();
		font.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		res = Resource::Create();

		//���f�[�^������

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

		if (!ge->QuitFlag() && nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		DrawScore();
		DrawGauge();
	}

	//-------------------------------------------------------------------
	//�X�R�A�`��
	void  Object::DrawScore() const
	{
		ML::Box2D textBox(0, 0, 1000, 1000);
		string scoreText = "score:" + to_string(ge->score);
		res->font->DrawF(textBox, scoreText, DG::Font::x1);
	}

	//-------------------------------------------------------------------
	//�̗̓Q�[�W�`��
	void  Object::DrawGauge() const
	{
		DrawGaugeFlame();
		DrawGaugeBar();
	}

	//-------------------------------------------------------------------
	//�̗̓Q�[�W�̃t���[���`��
	void  Object::DrawGaugeFlame() const
	{
		ML::Box2D draw(500, 10, 256, 68);
		ML::Box2D src(0, 0, 256, 68);
		res->imgHPFrame->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//�̗̓Q�[�W�̒��g�`��
	void  Object::DrawGaugeBar() const
	{
		int nowLife, maxLife;
		if (ge->qa_Player == nullptr) {//����������v���C���[�͂��Ȃ��̂�
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
	Resource::~Resource() { Finalize(); }
}