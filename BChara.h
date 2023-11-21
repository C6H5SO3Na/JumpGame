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
	ML::Box2D src;//�摜�̐؂���͈�
	ML::Box2D drawBase;//�`��͈�

	bool isHitFloor;//���Ɠ����������ۂ�
	float fallSpeed;//�������x
	struct Life {
		int now;
		int max;
	};
	Life life;
public:
	//�ύX������������������������������������������������������
	//�L�����̏��
	enum class State {
		Normal, Hit, Clear, Dead, Non
	};

	//���E�̌���(2D�����_�Q�[����p)
	enum Angle_LR {
		Right = 1, Left = -1
	};

	ML::Vec2 pos;//�v���C�����W
	ML::Box2D hitBase;//�����蔻��͈�

	int	moveCnt;//�s�������p�J�E���^
	int	animCnt;//�A�j���[�V���������p�J�E���^

	int score;//�X�R�A

	Angle_LR angle;
	State state;
	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����邱�Ɓ���
	BChara()//�R���X�g���N�^
		:pos()
		, moveCnt()
		, animCnt()
		, fallSpeed()
		, isHitFloor()
		, angle(Angle_LR::Right)
		, state(State::Non)
		, life()
		, score()
	{
	}
	virtual ~BChara() {}//�f�X�g���N�^

	//�L�����N�^���ʃ��\�b�h
	//�߂荞�܂Ȃ��ړ�����
	void CheckMove(ML::Vec2& e_);
	//�����ڐG����
	bool CheckFoot();
	//����ڐG����
	bool CheckHead();
	//������
	bool CheckLeftSide();
	//�E����
	bool CheckRightSide();
	//���ɗ��������̔���
	bool CheckFallHole();
	//��`�̍��W�̒��S�𒆉��ɂ��Ē�`����
	ML::Box2D CenterBox(int w, int h);
	//Box2D�^�̊e�v�f�Ɉ��̒l���|����(�g��p)
	ML::Box2D MultiplyBox2D(ML::Box2D box2D, float n);
	//���C�t�̑���
	void LifeOperation(int addLife);
};