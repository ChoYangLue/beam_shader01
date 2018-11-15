#include "DxLib.h"
#include <math.h>
#include <vector>

// モデルハンドル
int ModelHandle;
int ModelHandle2;
int ModelHandle3;

int DirLightHandle;		// ディレクショナルライトハンドル
VECTOR DirLightRote;	// ディレクショナルライトの方向

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

//int WindowW = 1280;	// ウィンドウの横幅
//int WindowH = 720;	// ウィンドウの縦幅

int WindowW = 1920;	// ウィンドウの横幅
int WindowH = 1080;	// ウィンドウの縦幅

void key_inp() {
	float mmrato = 0.2f;

	// 方向キーでカメラの座標を移動
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

	// マウス
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
	{
		// マウスの位置を取得
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
		// マウスの位置を取得
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
	// 前回 GetMouseWheelRotVol が呼ばれてから今回までの回転量を足す
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

	// 画面解像度設定
	SetGraphMode(WindowW, WindowH, 32);

	// Direct3D9Ex を使用する
	//SetUseDirect3DVersion(DX_DIRECT3D_9EX);

	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// シェーダー
	int VSHandle;					// VSハンドル
	int PSHandle;					// PSハンドル

	int BlHandle;
	int NsHandle;
	MATRIX camera_view_mat;
	int extend_size = 8;		// ガウスフィルタを掛ける画像が画面のサイズの何分の１か
	int GaussRatio = 1900;		// ガウスフィルタのぼかし度合い

	// 出力先に使用するグラフィックハンドルを作成
	BlHandle = MakeScreen(WindowW, WindowH, TRUE);
	NsHandle = MakeScreen(WindowW, WindowH, TRUE);

	ModelHandle = MV1LoadModel("untitled.mv1");

	// ３Ｄモデルのスケールを100倍にする
	MV1SetScale(ModelHandle, VGet(100.0f, 100.0f, 100.0f));

	// ディレクショナルライトの方向を初期化
	DirLightRote = VGet(0.0f, 0.0f, 0.0f);

	GetMousePoint(&PreMouseX, &PreMouseY);

	// カメラの座標を初期化
	CameraPos.x = 0.0f;
	CameraPos.y = 0.0f;
	CameraPos.z = -900.0f;

	LookAt.x = 0.0f;
	LookAt.y = 0.0f;
	LookAt.z = 0.0f;

	DirLightRote.z = 0.5f;

	// 合計値を初期化
	MouseRot = 60;

	// ライティングの計算をしないように設定を変更
	SetUseLighting(FALSE);

	while (!ProcessMessage() && !ScreenFlip() && !ClearDrawScreen()) {
		// 画面をクリア
		ClearDrawScreen();
		SetBackgroundColor(5, 5, 5);

		// キー入力
		key_inp();

		// 作成した画像を描画対象にする
		SetDrawScreen(NsHandle);

		// 画面をクリア
		ClearDrawScreen();

		// カメラの位置と注視点をセット、注視点は原点
		SetCameraPositionAndTarget_UpVecY(CameraPos, LookAt);

		// ３Ｄモデルの回転値をセットする
		MV1SetRotationXYZ(ModelHandle, VGet(deg2rad(ModelRote.x), deg2rad(ModelRote.y), deg2rad(ModelRote.z)));

		// モデルの描画
		MV1DrawModel(ModelHandle);

		// 描画対象を裏画面にする
		SetDrawScreen(DX_SCREEN_BACK);

		// 縮小フィルターを施す
		GraphFilterBlt(NsHandle, BlHandle, DX_GRAPH_FILTER_DOWN_SCALE, extend_size);

		// ぼかし
		GraphFilter(BlHandle, DX_GRAPH_FILTER_GAUSS, 16, GaussRatio);

		// 通常の描画結果を描画する
		DrawGraph(0, 0, NsHandle, TRUE);

		// 描画モードをバイリニアフィルタリングにする( 拡大したときにドットがぼやけるようにする )
		SetDrawMode(DX_DRAWMODE_BILINEAR);

		// 描画ブレンドモードを加算にする
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

		// 高輝度部分を縮小してぼかした画像を画面いっぱいに２回描画する( ２回描画するのはより明るくみえるようにするため )
		DrawExtendGraph(0, 0, WindowW*extend_size, WindowH*extend_size, BlHandle, TRUE);
		DrawExtendGraph(0, 0, WindowW*extend_size, WindowH*extend_size, BlHandle, TRUE);

		// 描画ブレンドモードをブレンド無しに戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

		// 描画モードを二アレストに戻す
		SetDrawMode(DX_DRAWMODE_NEAREST);

		// 裏画面の内容を表画面に反映
		ScreenFlip();
	}

	// モデルハンドルの削除
	MV1DeleteModel(ModelHandle);

	// ＤＸライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}
