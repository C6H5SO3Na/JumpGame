#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�G�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "BChara.h"

class BEnemy : public BChara
{
	//�ύX�s����������������������������������������������������
public:
	typedef  shared_ptr<BEnemy>	SP;
	typedef  weak_ptr<BEnemy>	WP;

protected:

public:
	//�ύX������������������������������������������������������
	//�L�����̏��


	//���E�̌���(2D�����_�Q�[����p)



	//�����o�ϐ��ɍŒ���̏��������s��
	//���������o�ϐ���ǉ�������K�����������ǉ����邱�Ɓ���
	BEnemy()//�R���X�g���N�^
	{
	}
	virtual ~BEnemy() {}//�f�X�g���N�^

	//�G���ʃ��\�b�h
	//�J�����Ƃ̓����蔻��
	bool CheckHitCamera2D() const;
	void CheckHitPlayer();
	void Dead();
};