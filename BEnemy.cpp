//-------------------------------------------------------------------
//�G�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include  "BEnemy.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-------------------------------------------------------------------
//�J�����Ƃ̓����蔻��
bool BEnemy::CheckHitCamera2D() const
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
//-------------------------------------------------------------------
//�v���C���Ƃ̓����蔻��
void BEnemy::CheckHitPlayer()
{
	if (state == State::Non) { return; }
	//�v���C���Ɠ����蔻��
	ML::Box2D  me = hitBase.OffsetCopy(pos);
	Player::Object::SP player = ge->qa_Player;

	if (player->GetState() != State::Normal || player->invincible.isInvincible()) { return; }

	ML::Box2D  you = player->GetHitBase().OffsetCopy(player->GetPos());
	if (you.Hit(me)) {
		player->LifeOperation(-1);
		if (player->GetState() != State::Dead) {//����ł��Ȃ��Ƃ��̓_���[�W���󂯂鉉�o������
			player->DamageOperation();
		}
	}
}
//-------------------------------------------------------------------
//���S����
void BEnemy::Dead()
{
	state = State::Non;
	moveCnt = 0;
	animCnt = 0;
}