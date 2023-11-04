//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include "MyPG.h"
#include "Task_Player.h"
#include "Task_Enemy00.h"
#include "Task_Map2D.h"
#include <assert.h>
#include "randomLib.h"

namespace Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/player.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		img.reset();
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
		angle = Angle_LR::Right;
		hitBase = CenterBox(32 * 2, 64 * 2);
		pos = ML::Vec2(200.f, 200.f);
		moveVec = ML::Vec2(8.f, 8.f);
		jumpPow = -17.f;

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
		auto enemies = ge->GetTasks<Enemy00::Object>("�G");
		for (auto it = enemies->begin(); it != enemies->end(); ++it) {
			(*it)->CheckHitPlayer();
		}
		Operation();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		Animation();
		ML::Box2D draw = MultiplyBox2D(drawBase, 2.f).OffsetCopy(pos);
		ge->ApplyCamera2D(draw);
		res->img->Draw(draw, src);
	}

	//-------------------------------------------------------------------
	//�v���C���̑��쏈��
	void Object::Operation()
	{
		if (state == State::Non) { return; }
		ML::Vec2 est(0.f, 0.f);
		auto inp = ge->in1->GetState();
		switch (state) {
			//�����Ă���Ƃ�
		case State::Normal:
			//���E�ړ�
			if (inp.LStick.BL.on) {
				est.x -= moveVec.x;
				angle = Angle_LR::Left;

				//�W�����v���łȂ��Ƃ������A�j���[�V����
				if (fallSpeed == 0.f) {
					animKind = Anim::Walk;
				}
			}
			if (inp.LStick.BR.on) {
				est.x += moveVec.x;
				angle = Angle_LR::Right;

				//�W�����v���łȂ��Ƃ������A�j���[�V����
				if (fallSpeed == 0.f) {
					animKind = Anim::Walk;
				}
			}

			//�f�o�b�O�p�@�����R�}���h
			{
				if (inp.B3.down) {
					state = State::Dead;
				}
			}

			//�W�����v
			if (inp.B1.down) {
				if (isHitFloor) {//���n���̂݃W�����v�J�n�ł���
					animKind = Anim::Jump;
					fallSpeed = jumpPow;
				}
			}
			if (inp.B1.up && fallSpeed < 0) {//�W�����v�{�^�������ꂽ�u�Ԃ��㏸���̏ꍇ
				fallSpeed = 0.f;//�������x0(���W�����v)
			}
			est.y += fallSpeed;

			//���S�Ɏ~�܂��Ă���Ƃ��͎~�܂��Ă���Ƃ��̃A�j���[�V����
			//�W�����v���̓W�����v�̃A�j���[�V����������
			if (est == ML::Vec2() && animKind != Anim::Jump) {
				animKind = Anim::Idle;
			}


			CheckMove(est);

			//�W�����v�łȂ��������͗����̃A�j���[�V����������
			if (!isHitFloor && animKind != Anim::Jump) {
				animKind = Anim::Fall;
			}

			break;

			//���S��
		case State::Dead:
			ge->isGameOver = true;
			animKind = Anim::Dead;

			//���S���ɂ͏�ɂ����Ȃ��悤�ɂ���
			fallSpeed = max(0.f, fallSpeed);

			//���S���ɂ��d�͂𓭂�����
			est.y += fallSpeed;

			CheckMove(est);
			break;

			//�N���A��
		case State::Clear:
			break;
		}

		//�����ڐG����
		isHitFloor = CheckFoot();
		if (isHitFloor) {
			//�W�����v�A�j���[�V�����̏ꍇ����
			if (animKind == Anim::Jump) {
				animKind = Anim::Idle;
			}
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
		//�J�����̈ʒu���Ē���
		{
			//�v���C������ʂ̂ǂ��ɒu�����i����͉�ʒ����j
			int px = ge->camera2D.w / 2;
			int py = ge->camera2D.h / 2;
			//�v���C������ʒ����ɒu�����Ƃ��̃J�����̍�����W�����߂�
			int cpx = int(this->pos.x) - px;
			int cpy = int(this->pos.y) - py;
			//�J�����̍��W���X�V
			ge->camera2D.x = cpx;
			ge->camera2D.y = cpy;
			//�}�b�v�̊O�����f��Ȃ��悤�ɃJ�����𒲐�����
			if (auto map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName)) {
				map->AdjustCameraPos();
			}
		}
		++moveCnt;
		++animCnt;
	}
	//-------------------------------------------------------------------
	//�A�j���[�V����
	void Object::Animation()
	{
		switch (animKind) {
		case Anim::Idle:
			drawBase = CenterBox(100, 64);
			src = ML::Box2D((animCnt / 8) % 4 * drawBase.w, 0, drawBase.w, drawBase.h);
			break;

		case Anim::Walk:
			drawBase = CenterBox(100, 64);
			src = ML::Box2D((animCnt / 8) % 7 * drawBase.w, drawBase.h, drawBase.w, drawBase.h);
			break;

		case Anim::Jump:
			drawBase = CenterBox(100, 64);
			src = ML::Box2D((animCnt / 8) % 6 * drawBase.w, drawBase.h * 2, drawBase.w, drawBase.h);
			break;

		case Anim::Fall:
			drawBase = CenterBox(100, 64);
			src = ML::Box2D((animCnt / 8) % 3 * drawBase.w + drawBase.w * 4, 0, drawBase.w, drawBase.h);
			break;
		case Anim::Dead:
			drawBase = CenterBox(100, 64);
			if (ge->GameOverCnt < 5 * 8 - 1) {
				++ge->GameOverCnt;
			}
			src = ML::Box2D(ge->GameOverCnt / 8 * drawBase.w, drawBase.h * 4, drawBase.w, drawBase.h);
			break;

		case Anim::Clear:
			drawBase = ML::Box2D(-16, -40, 32, 64);
			switch ((animCnt / 16) % 2) {
			case 0:		src = ML::Box2D(0, 128, 32, 64);		break;
			case 1:		src = ML::Box2D(32, 128, 32, 64);	break;
			}
			break;
		}
		//���������]
		if (angle == Angle_LR::Left &&
			drawBase.w >= 0) {
			drawBase.x = -drawBase.x;
			drawBase.w = -drawBase.w;
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