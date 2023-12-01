//-------------------------------------------------------------------
//�G00
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Enemy00.h"
#include "Task_Player.h"
#include "Task_Map2D.h"
#include <assert.h>

namespace Enemy00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img[0] = DG::Image::Create("data/image/pipo-simpleenemy01b.png");
		img[1] = DG::Image::Create("data/image/pipo-simpleenemy01c.png");
		img0 = DG::Image::Create("./data/image/boxBlue.bmp");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		for (int i = 0; i < size(img); ++i) {
			img[i].reset();
		}
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
		render2D_Priority[1] = 0.5f;
		state = State::Normal;
		angle = Angle_LR::Left;
		hitBase = CenterBox(32 * 2, 32 * 2);
		moveVec = ML::Vec2(5.f, 5.f);
		src = ML::Box2D(0, 0, 32, 32);
		score = 100;
		jumpPow = -10.f;

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		Move();
		if (state == State::Non) {
			Kill();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		Anim();
		{
			ML::Box2D draw = MultiplyBox2D(drawBase, 2.f).OffsetCopy(pos);
			ge->ApplyCamera2D(draw);
			res->img[static_cast<int>(type)]->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//�G�̓���
	void Object::Move()
	{
		if (!CheckHitCamera2D() || state == State::Non) { return; }//���ł������J�����͈̔͊O�̂Ƃ��A���삵�Ȃ�
		ML::Vec2 est(0.f, 0.f);
		auto inp = ge->in1->GetState();
		//���E�ړ�
		est.x = moveVec.x * angle;
		//�W�����v
		if (type == Type::Jumping) {
			if (isHitFloor) {//���n���̂݃W�����v�J�n�ł���
				//animKind = Anim::Jump;
				fallSpeed = jumpPow;
			}
		}
		est.y += fallSpeed;

		////���S�Ɏ~�܂��Ă���Ƃ��͎~�܂��Ă���Ƃ��̃A�j���[�V����
		////�W�����v���̓W�����v�̃A�j���[�V����������
		//if (est == ML::Vec2() && animKind != Anim::Jump) {
		//	animKind = Anim::Idle;
		//}

		CheckMove(est);

		if (CheckLeftSide()) {
			angle = Angle_LR::Right;
		}
		if (CheckRightSide()) {
			angle = Angle_LR::Left;
		}

		//�����ڐG����
		isHitFloor = CheckFoot();
		if (isHitFloor) {
			////�W�����v�A�j���[�V�����̏ꍇ����
			//if (animKind == Anim::Jump) {
			//	animKind = Anim::Idle;
			//}
			fallSpeed = 0.f;//�������x0
		}
		else {
			fallSpeed += ML::Gravity(32) * 6.f;//�d�͉���
		}

		//���㔻��
		if (fallSpeed < 0) {//�㏸��
			if (CheckHead()) {
				fallSpeed = 0;//�㏸�͂𖳌��ɂ���
			}
		}

		//���ɗ���������ł�����
		if (CheckFallHole()) {
			state = State::Non;
		}
		++moveCnt;
		++animCnt;
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::Anim()
	{
		int animAngleTmp = 32;//draw�ɂ�����h���W ���̏ꍇ32
		if (angle == Angle_LR::Right) {//�E��������64�ɂ���
			animAngleTmp = 64;
		}

		switch (animKind) {
		case Anim::Move:
		{
			int frameInterval = 8;//�A�j���[�V�����̊Ԋu�t���[��
			drawBase = CenterBox(32, 32);
			src = ML::Box2D((animCnt / frameInterval) % 3 * drawBase.w, animAngleTmp, drawBase.w, drawBase.h);
			break;
		}
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