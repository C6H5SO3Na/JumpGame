#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"

class BChara : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef  shared_ptr<BChara>	SP;
	typedef  weak_ptr<BChara>	WP;

protected:
	ML::Vec2 moveVec;//�ړ����x
	ML::Box2D hitBase;//�����蔻��͈�
	ML::Box2D src;//�摜�̐؂���͈�
	ML::Box2D drawBase;//�`��͈�
	ML::Box2D footBase;//��������p
	ML::Box2D headBase;//���㔻��p
	ML::Box2D leftSideBase;//������p
	ML::Box2D rightSideBase;//������p

public:
	//�ύX������������������������������������������������������
	//�L�����̏��
	enum class State {
		Normal, Clear, Dead, Non
	};

	//���E�̌���(2D�����_�Q�[����p)
	enum class Angle_LR {
		Right, Left
	};

	ML::Vec2 pos;//�v���C�����W

	int	moveCnt;//�s�������p�J�E���^
	int	animCnt;//�A�j���[�V���������p�J�E���^

	Angle_LR angle_LR;

	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����邱�Ɓ���
	BChara()//�R���X�g���N�^
		:pos()
		, hitBase()
		, moveVec()
		, moveCnt(0)
		, animCnt(0)
		, angle_LR(Angle_LR::Left)
	{
	}
	virtual ~BChara() {}//�f�X�g���N�^

	//�L�����N�^���ʃ��\�b�h
	//�߂荞�܂Ȃ��ړ�����
	void CheckMove(ML::Vec2& e_);
	//�����ڐG����
	bool CheckFoot();
	//������
	bool CheckLeftSide();
	//�E����
	bool CheckRightSide();
	//�V�䔻��
	void CheckHitPlayer();
	//��`�̍��W�̒��S�𒆉��ɂ��Ē�`����
	ML::Box2D CenterBox(int w, int h);
	//Box2D�^�̊e�v�f�Ɉ��̒l���|����(�g��p)
	ML::Box2D MultiplyBox2D(ML::Box2D box2D, float n);
};