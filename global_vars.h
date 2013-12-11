#define DIRECTINPUT_VERSION  0x0800
#define INITGUID
#define KEYDOWN(name, key) (name[key]&0x80)
#define MOUSE_LB 0
#define MOUSE_RB 1
#define MOUSE_MB 2
#define WINWIDTH 800
#define WINHEIGHT 300
#define ADDRESS_SIZE 100 // ���������� �������� � ������ �������
#define RELEASE TRUE
//==============================================================================
#include <windows.h>
#include <stdio.h>
#include <dinput.h>
#include <d3dx9.h>
//#include <math.h>
//#include <d3d9.h>
//#include <mmsystem.h>
//#include <d3dx9core.h>
//#include <d3dx9mesh.h>
//#include <dmusic.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dinput8.lib")
//#pragma comment(lib, "d3dx9dt.lib")
//#pragma comment(lib, "dinput.lib")
//#pragma comment(lib, "dsound.lib")
//#pragma comment(lib, "dxguid.lib")
//==============================================================================
HRESULT InitDirect3D();
VOID	DeleteDirect3D();
VOID	DeleteDrawText();
VOID	InitInput();
VOID	InitDrawText();
VOID	Matrix();
VOID	LoadArea();
VOID	NewMesh(char*);
VOID	DeleteInput();
VOID	ResumeGame();
VOID	SetSmooth();
VOID	Engine();
VOID	Render();
VOID	GetInput();
//==============================================================================
WNDCLASSEX				windowsclass;	// ����� ����
HWND					hwnd;
LPDIRECT3D9				pDirect3D	= NULL;
LPDIRECT3DDEVICE9		Device		= NULL;
D3DMATERIAL9			Material;
D3DLIGHT9				Light;

LPDIRECTINPUT8			Input		= NULL;
LPDIRECTINPUTDEVICE8	Keyboard	= NULL;
LPDIRECTINPUTDEVICE8	Mouse		= NULL;
BYTE					Key[256];
DIMOUSESTATE			MouseState;

LPD3DXFONT				Font		= NULL; 
RECT					Rect;
HFONT					hFont;

// ���
D3DXMATRIX	World;				// ������� �������
D3DXMATRIX	Wheel1, Wheel2;
D3DXMATRIX  WorldX, WorldY, 
			WorldZ, WorldMove;	// ������� ������������
D3DXMATRIX	Camera;				// ������
D3DXMATRIX	Projection;			// ��������

FLOAT		Correlation;

CHAR		STR_FPS[20]="FPS: ", STR_SPEED[30]="SPEED: ", 
			RENDER_TIME[20]="RENDER: ", HELP[30]="F1 - ������!", 
			DebMODE=0, SmoothMODE=2, 
			INFORMER[]="ESC - �����\n\n�����/������ - ������ �����/������\n"
			"�����/���� - ���/������\n������ - ������ �������\n"
			"F - ����. ����� ������\nF2 - ����� �����\nF3 - ��������\n"
			"F4 - ��������\nF5 - �����������\n\nZ - ������� ����\nC - ������� ������\n";
BOOL		LimitFps = FALSE, MeshMODE=FALSE, NoTexMODE=FALSE, NoMatMODE=FALSE, FullScreen=TRUE;
BYTE		RecMODE, GameMode=1;
UINT		RecCount = 0;

UINT		ScreenWidth, ScreenHeight;

// ������ ���� 

