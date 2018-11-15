#include "DxLib.h"
#include <math.h>
#include <vector>

// ���f���n���h��
int ModelHandle;
int ModelHandle2;
int ModelHandle3;

int DirLightHandle;		// �f�B���N�V���i�����C�g�n���h��
VECTOR DirLightRote;	// �f�B���N�V���i�����C�g�̕���

VECTOR CameraPos;
VECTOR CameraRote;
VECTOR LookAt;
float CameraDist;
VECTOR CameraVec;

VECTOR ModelPos;
VECTOR ModelRote;

int MouseX, MouseY;
int PreMouseX, PreMouseY;
int MouseRot;

//int WindowW = 1280;	// �E�B���h�E�̉���
//int WindowH = 720;	// �E�B���h�E�̏c��

int WindowW = 1920;	// �E�B���h�E�̉���
int WindowH = 1080;	// �E�B���h�E�̏c��

void key_inp() {
	float mmrato = 0.2f;

	// �����L�[�ŃJ�����̍��W���ړ�
	if (CheckHitKey(KEY_INPUT_UP) == 1)
	{
		DirLightRote.z += 0.01f;
	}
	if (CheckHitKey(KEY_INPUT_DOWN) == 1)
	{
		DirLightRote.z -= 0.01f;
	}
	if (CheckHitKey(KEY_INPUT_LEFT) == 1)
	{
		DirLightRote.x -= 0.1f;
	}
	if (CheckHitKey(KEY_INPUT_RIGHT) == 1)
	{
		DirLightRote.x += 0.1f;
	}

	if (CheckHitKey(KEY_INPUT_W) == 1) {
		CameraPos.z += 20.0f;
		LookAt.z += 20.0f;
	}
	else if (CheckHitKey(KEY_INPUT_S) == 1) {
		CameraPos.z -= 20.0f;
		LookAt.z -= 20.0f;
	}
	if (CheckHitKey(KEY_INPUT_A) == 1) {
		CameraPos.x -= 20.0f;
		LookAt.x -= 20.0f;
	}
	else if (CheckHitKey(KEY_INPUT_D) == 1) {
		CameraPos.x += 20.0f;
		LookAt.x += 20.0f;
	}

	if (CheckHitKey(KEY_INPUT_NUMPAD5) == 1) {
		ModelRote.y = 0.0f;
	}
	else if (CheckHitKey(KEY_INPUT_NUMPAD6) == 1) {
		ModelRote.y += 45.0f;
	}
	else if (CheckHitKey(KEY_INPUT_NUMPAD4) == 1) {
		ModelRote.y -= 45.0f;
	}

	// �}�E�X
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		// �}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);
		if (PreMouseX > MouseX) {
			//ModelRote.y += 2.0f;
			ModelRote.y += (PreMouseX - MouseX)*mmrato;
			//CameraRote.y += 5.0f;
		}
		else if (PreMouseX < MouseX) {
			//ModelRote.y -= 2.0f;
			ModelRote.y += (PreMouseX - MouseX)*mmrato;
			//CameraRote.y -= 5.0f;
		}
		if (PreMouseY > MouseY) {
			//ModelRote.x += 2.0f;
			ModelRote.x += (PreMouseY - MouseY)*mmrato;
		}
		else if (PreMouseY < MouseY) {
			//ModelRote.x -= 2.0f;
			ModelRote.x += (PreMouseY - MouseY)*mmrato;
		}

		if (ModelRote.y > 360) ModelRote.y = 0.0f;
		else if (ModelRote.y < 0) ModelRote.y = 360.0f;

		if (ModelRote.x > 360) ModelRote.x = 0.0f;
		else if (ModelRote.x < 0) ModelRote.x = 360.0f;


		PreMouseX = MouseX;
		PreMouseY = MouseY;
	}
	if ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) {
		// �}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);
		if (PreMouseX > MouseX) {
			CameraPos.x += 40.0f;
			LookAt.x += 40.0f;
		}
		else if (PreMouseX < MouseX) {
			CameraPos.x -= 40.0f;
			LookAt.x -= 40.0f;
		}
		if (PreMouseY > MouseY) {
			CameraPos.y -= 40.0f;
			LookAt.y -= 40.0f;
		}
		else if (PreMouseY < MouseY) {
			CameraPos.y += 40.0f;
			LookAt.y += 40.0f;
		}

		PreMouseX = MouseX;
		PreMouseY = MouseY;
	}
	// �O�� GetMouseWheelRotVol ���Ă΂�Ă��獡��܂ł̉�]�ʂ𑫂�
	MouseRot += GetMouseWheelRotVol();
	ModelPos.z = MouseRot * 50.0f;
	//printfDx("%f \n", ModelPos.z);
}

float deg2rad(float xx) {
	return xx / (3.14 / 180);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE);

	// ��ʉ𑜓x�ݒ�
	SetGraphMode(WindowW, WindowH, 32);

	// Direct3D9Ex ���g�p����
	//SetUseDirect3DVersion(DX_DIRECT3D_9EX);

	// �c�w���C�u�����̏�����
	if (DxLib_Init() < 0) return -1;

	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// �V�F�[�_�[
	int VSHandle;					// VS�n���h��
	int PSHandle;					// PS�n���h��

	int BlHandle;
	int NsHandle;
	MATRIX camera_view_mat;
	int extend_size = 8;		// �K�E�X�t�B���^���|����摜����ʂ̃T�C�Y�̉����̂P��
	int GaussRatio = 1900;		// �K�E�X�t�B���^�̂ڂ����x����

	// �o�͐�Ɏg�p����O���t�B�b�N�n���h�����쐬
	BlHandle = MakeScreen(WindowW, WindowH, TRUE);
	NsHandle = MakeScreen(WindowW, WindowH, TRUE);

	ModelHandle = MV1LoadModel("untitled.mv1");

	// �R�c���f���̃X�P�[����100�{�ɂ���
	MV1SetScale(ModelHandle, VGet(100.0f, 100.0f, 100.0f));

	// �f�B���N�V���i�����C�g�̕�����������
	DirLightRote = VGet(0.0f, 0.0f, 0.0f);

	GetMousePoint(&PreMouseX, &PreMouseY);

	// �J�����̍��W��������
	CameraPos.x = 0.0f;
	CameraPos.y = 0.0f;
	CameraPos.z = -900.0f;

	LookAt.x = 0.0f;
	LookAt.y = 0.0f;
	LookAt.z = 0.0f;

	DirLightRote.z = 0.5f;

	// ���v�l��������
	MouseRot = 60;

	// ���C�e�B���O�̌v�Z�����Ȃ��悤�ɐݒ��ύX
	SetUseLighting(FALSE);

	while (!ProcessMessage() && !ScreenFlip() && !ClearDrawScreen()) {
		// ��ʂ��N���A
		ClearDrawScreen();
		SetBackgroundColor(5, 5, 5);

		// �L�[����
		key_inp();

		// �쐬�����摜��`��Ώۂɂ���
		SetDrawScreen(NsHandle);

		// ��ʂ��N���A
		ClearDrawScreen();

		// �J�����̈ʒu�ƒ����_���Z�b�g�A�����_�͌��_
		SetCameraPositionAndTarget_UpVecY(CameraPos, LookAt);

		// �R�c���f���̉�]�l���Z�b�g����
		MV1SetRotationXYZ(ModelHandle, VGet(deg2rad(ModelRote.x), deg2rad(ModelRote.y), deg2rad(ModelRote.z)));

		// ���f���̕`��
		MV1DrawModel(ModelHandle);

		// �`��Ώۂ𗠉�ʂɂ���
		SetDrawScreen(DX_SCREEN_BACK);

		// �k���t�B���^�[���{��
		GraphFilterBlt(NsHandle, BlHandle, DX_GRAPH_FILTER_DOWN_SCALE, extend_size);

		// �ڂ���
		GraphFilter(BlHandle, DX_GRAPH_FILTER_GAUSS, 16, GaussRatio);

		// �ʏ�̕`�挋�ʂ�`�悷��
		DrawGraph(0, 0, NsHandle, TRUE);

		// �`�惂�[�h���o�C���j�A�t�B���^�����O�ɂ���( �g�債���Ƃ��Ƀh�b�g���ڂ₯��悤�ɂ��� )
		SetDrawMode(DX_DRAWMODE_BILINEAR);

		// �`��u�����h���[�h�����Z�ɂ���
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

		// ���P�x�������k�����Ăڂ������摜����ʂ����ς��ɂQ��`�悷��( �Q��`�悷��̂͂�薾�邭�݂���悤�ɂ��邽�� )
		DrawExtendGraph(0, 0, WindowW*extend_size, WindowH*extend_size, BlHandle, TRUE);
		DrawExtendGraph(0, 0, WindowW*extend_size, WindowH*extend_size, BlHandle, TRUE);

		// �`��u�����h���[�h���u�����h�����ɖ߂�
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

		// �`�惂�[�h���A���X�g�ɖ߂�
		SetDrawMode(DX_DRAWMODE_NEAREST);

		// ����ʂ̓��e��\��ʂɔ��f
		ScreenFlip();
	}

	// ���f���n���h���̍폜
	MV1DeleteModel(ModelHandle);

	// �c�w���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}
