//-------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include  "BChara.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-------------------------------------------------------------------
//�߂荞�܂Ȃ��ړ�����
void BChara::CheckMove(ML::Vec2& e_)
{
	//�}�b�v�����݂��邩���ׂĂ���A�N�Z�X
	auto map = ge->GetTask<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (map == nullptr) { return; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)

	//�����ɑ΂���ړ�
	while (e_.x != 0.f) {
		float preX = pos.x;
		if (e_.x >= +1.f) {
			pos.x += 1.f;
			e_.x -= 1.f;
		}
		else if (e_.x <= -1.f) {
			pos.x -= 1.f;
			e_.x += 1.f;
		}
		else {
			pos.x += e_.x;
			e_.x = 0.f;
		}
		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (map->CheckHit(hit)) {
			pos.x = preX;//�ړ��L�����Z��
			break;
		}
	}

	//�c���ɑ΂���ړ�
	while (e_.y != 0.f) {
		float preY = pos.y;
		if (e_.y >= +1.f) {
			pos.y += 1.f;
			e_.y -= 1.f;
		}
		else if (e_.y <= -1.f) {
			pos.y -= 1.f;
			e_.y += 1.f;
		}
		else {
			pos.y += e_.y;
			e_.y = 0.f;
		}
		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (map->CheckHit(hit)) {
			pos.y = preY;//�ړ��L�����Z��
			break;
		}
	}
}
//-------------------------------------------------------------------
//�����ڐG����
bool BChara::CheckFoot()
{
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D foot(
		hitBase.x,
		hitBase.y + hitBase.h,
		hitBase.w,
		1);
	foot.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�}�b�v�ƐڐG����
	return map->CheckHit(foot);
}

//-------------------------------------------------------------------
//����̓����蔻��
bool BChara::CheckHead()
{
	//�����蔻�����ɂ��ē����`�𐶐�
	ML::Box2D head(
		hitBase.x,
		hitBase.y - 1,
		hitBase.w,
		1);
	head.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�}�b�v�ƐڐG����
	return  map->CheckHit(head);
}
//-------------------------------------------------------------------
//�����̓����蔻��
bool  BChara::CheckLeftSide()
{
	//�����蔻�����ɂ��č�1�}�X�̋�`�𐶐�
	ML::Box2D leftSide(
		hitBase.x - 1,
		hitBase.y,
		1,
		hitBase.h);
	leftSide.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�}�b�v�ƐڐG����
	return  map->CheckHit(leftSide);
}
//-------------------------------------------------------------------
//�E���̓����蔻��
bool  BChara::CheckRightSide()
{
	//�����蔻�����ɂ��ĉE1�}�X�̋�`�𐶐�
	ML::Box2D rightSide(
		hitBase.w + hitBase.x,
		hitBase.y,
		1,
		hitBase.h);
	rightSide.Offset(pos);

	auto map = ge->GetTask<Map2D::Object>("�t�B�[���h", "�}�b�v");
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�}�b�v�ƐڐG����
	return  map->CheckHit(rightSide);
}
//-------------------------------------------------------------------
//��`�̍��W�̒��S�𒆉��ɂ��Ē�`����
ML::Box2D BChara::CenterBox(int w, int h)
{
	return ML::Box2D(-w / 2, -h / 2, w, h);
}
//-------------------------------------------------------------------
//Box2D�^�̊e�v�f�Ɉ��̒l���|����(�g��p)
ML::Box2D BChara::MultiplyBox2D(ML::Box2D box, float n)
{
	return ML::Box2D(int(box.x * n), int(box.y * n),
		int(box.w * n), int(box.h * n));
}