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
		img = DG::Image::Create("data/image/pipo-simpleenemy01b.png");
		img0 = DG::Image::Create("./data/image/boxBlue.bmp");
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
		angle = Angle_LR::Left;
		hitBase = drawBase = CenterBox(32 * 2, 32 * 2);
		moveVec = ML::Vec2(5.f, 5.f);
		src = ML::Box2D(0, 0, 32, 32);
		score = 100;
		//jumpPow = -17.f;

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
		{
			ML::Box2D draw = drawBase.OffsetCopy(pos);
			ge->ApplyCamera2D(draw);
			res->img->Draw(draw, src);
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
		////�W�����v
		//if (inp.B1.down) {
		//	if (hitFlag) {//���n���̂݃W�����v�J�n�ł���
		//		animKind = Anim::Jump;
		//		fallSpeed = jumpPow;
		//	}
		//}
		//if (inp.B1.up && fallSpeed < 0) {//�W�����v�{�^�������ꂽ�u�Ԃ��㏸���̏ꍇ
		//	fallSpeed = 0.f;//�������x0(���W�����v)
		//}
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
		//if (fallSpeed < 0) {//�㏸��
		//	if (CheckHead()) {
		//		fallSpeed = 0;//�㏸�͂𖳌��ɂ���
		//	}
		//}

		//���ɗ���������ł�����
		if (CheckFallHole()) {
			state = State::Non;
		}
		++moveCnt;
		++animCnt;
	}
<<<<<<< HEAD


=======
	//-------------------------------------------------------------------
	//�v���C���Ƃ̓����蔻��
	void Object::CheckHitPlayer()
	{
		if (state == State::Non) { return; }
		//�v���C���Ɠ����蔻��
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		auto player = ge->GetTask<Player::Object>(Player::defGroupName);

		if (player->state != State::Normal) { return; }

		ML::Box2D  you = player->hitBase.OffsetCopy(player->pos);
		if (you.Hit(me)) {
			player->state = State::Dead;
			player->moveCnt = 0;
			player->animCnt = 0;
		}
	}
	//-------------------------------------------------------------------
	//�J�����Ƃ̓����蔻��
	bool Object::CheckHitCamera2D()
	{
		if (state == State::Non) { return false; }
		//�v���C���Ɠ����蔻��
		ML::Box2D  me = hitBase.OffsetCopy(pos);
		int n = 400;//�J������`���w�肵�������[�ɍL����
		ML::Box2D  you(
			ge->camera2D.x - n,
			ge->camera2D.y,
			ge->camera2D.w + n * 2,
			ge->camera2D.h
		);
		if (you.Hit(me)) {
			return true;
		}
		return false;
	}
>>>>>>> 0c9e8fdf07396f966f2aa9e10c915d01e9ee85e6
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