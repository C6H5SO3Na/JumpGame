//-------------------------------------------------------------------
//�G00
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Enemy00.h"
#include "Task_Player.h"
#include "Task_Map2D.h"

namespace Enemy00
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		img = DG::Image::Create("./data/image/boxRed.bmp");
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
		src = ML::Box2D(0, 0, 64, 128);
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
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		{
			ML::Box2D draw = drawBase.OffsetCopy(pos);
			ge->ApplyCamera2D(draw);
			res->img0->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//�G�̓���
	void Object::Move()
	{
		if (state == State::Non) { return; }
		ML::Vec2 est(0.f, 0.f);
		auto inp = ge->in1->GetState();
		//���E�ړ�
		if (angle == Angle_LR::Left) {
			est.x = -moveVec.x;
		}
		else {
			est.x = moveVec.x;
		}
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
	//�v���C���Ƃ̓����蔻��
	void Object::CheckHitPlayer()
	{
		if (state == State::Non) { return; }
		//�v���C���Ɠ����蔻��
		ML::Box2D  me = hitBase.OffsetCopy((int)pos.x, (int)pos.y);
		auto player = ge->GetTask<Player::Object>(Player::defGroupName);

		if (player->state != Player::Object::State::Normal) { return; }

		ML::Box2D  you = player->hitBase.OffsetCopy((int)player->pos.x, (int)player->pos.y);
		if (you.Hit(me)) {
			player->state = Player::Object::State::Dead;
			player->moveCnt = 0;
			player->animCnt = 0;
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