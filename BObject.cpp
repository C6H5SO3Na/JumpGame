//-------------------------------------------------------------------
//�I�u�W�F�N�g(�������́A�����Ȃ�����)�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include  "BObject.h"
#include  "MyPG.h"
#include  "Task_Map2D.h"

//-------------------------------------------------------------------
//�����ڐG����
bool BObject::CheckFoot() const
{
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D foot(
		hitBase.x,
		hitBase.y + hitBase.h,
		hitBase.w,
		1);
	foot.Offset(pos);

	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�}�b�v�ƐڐG����
	return map->CheckHit(foot);
}

//-------------------------------------------------------------------
//����̓����蔻��
bool BObject::CheckHead() const
{
	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�����蔻�����ɂ��ē����`�𐶐�
	ML::Box2D head(
		hitBase.x,
		hitBase.y - 1,
		hitBase.w,
		1);
	head.Offset(pos);

	//�}�b�v�ƐڐG����
	return  map->CheckHit(head);
}
//-------------------------------------------------------------------
//�����̓����蔻��
bool  BObject::CheckLeftSide() const
{

	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	//�����蔻�����ɂ��č�1�}�X�̋�`�𐶐�
	ML::Box2D leftSide(
		hitBase.x - 1,
		hitBase.y,
		1,
		hitBase.h);
	leftSide.Offset(pos);

	//�}�b�v�ƐڐG����
	return  map->CheckHit(leftSide);
}
//-------------------------------------------------------------------
//�E���̓����蔻��
bool  BObject::CheckRightSide() const
{
	Map2D::Object::SP map = ge->qa_Map;
	if (map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)

	//�����蔻�����ɂ��ĉE1�}�X�̋�`�𐶐�
	ML::Box2D rightSide(
		hitBase.w + hitBase.x,
		hitBase.y,
		1,
		hitBase.h);
	rightSide.Offset(pos);

	//�}�b�v�ƐڐG����
	return  map->CheckHit(rightSide);
}
//-------------------------------------------------------------------
//���ɗ��������̔���
bool BObject::CheckFallHole() const
{
	if (ge->qa_Map == nullptr) { return false; }//�}�b�v���Ȃ���Δ��肵�Ȃ�(�ł��Ȃ�)
	return pos.y > ge->screen2DHeight - hitBase.y;//�L�����N�^�㕔�̍��W����ʉ��̍��W�𒴂����Ƃ�true��Ԃ�
}
//-------------------------------------------------------------------
//��`�̍��W�̒��S�𒆉��ɂ��Ē�`����
ML::Box2D BObject::CenterBox(const int& w, const int& h)
{
	return ML::Box2D(-w / 2, -h / 2, w, h);
}
//-------------------------------------------------------------------
//Box2D�^�̊e�v�f�Ɉ��̒l���|����(�g��p)
ML::Box2D BObject::MultiplyBox2D(const ML::Box2D& box, const float& n)
{
	return ML::Box2D(int(box.x * n), int(box.y * n),
		int(box.w * n), int(box.h * n));
}