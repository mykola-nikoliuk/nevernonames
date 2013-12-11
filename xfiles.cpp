//=== PROJECT FILES ============================================================
#include "global_vars.h"
#include "classes.h"
//==============================================================================
VOID ResumeGame()
{
	try {
		// звільняємо прийстрій D3D вводу та тексту
		for (int i=0; i<OBJECT3D::Count; i++)
			Obj[i]->Release();
		DeleteDrawText();
		DeleteInput();
		DeleteDirect3D();
	}
	catch(...) 
	{ MessageBox(hwnd, "Ошибка при очистке памяти.", "ResumeGame(); Ошибка восстановления", 0);}	
	try {
		// відновити прийстрій D3D вводу та тексту
		InitDirect3D();
		InitInput();
		InitDrawText();
	}
	catch(...) 
	{ MessageBox(hwnd, "Ошибка при возобновлении D3D, устройств ввода.", "ResumeGame(); Ошибка восстановления", 0);}	
	try {
		// Перезагружаем все объекты
		for (int i=0; i<OBJECT3D::Count; i++)
			Obj[i]->Refresh();
	}
	catch(...) 
	{ MessageBox(hwnd, "Ошибка при перезагрузке объектов.", "ResumeGame(); Ошибка восстановления", 0);}
	GameMode = 1;
}
//==============================================================================
VOID InitDrawText()
{
	AddFontResourceExA("Bin/Fonts/DexterC.ttf", FR_PRIVATE, 0);
	hFont = CreateFont(30, 10, 0, 0, FW_NORMAL, FALSE, FALSE, 0, 1, 0,
		0, 0, DEFAULT_PITCH|FF_MODERN, "DexterC");
	D3DXCreateFont(Device, 30, 10, 1, 0, FALSE, TRUE,30,0,0, "DexterC", &Font);   
}
//==============================================================================
VOID DeleteDrawText()
{
	Font->Release();
	DeleteObject(hFont);
	DeleteObject(Font);
}
//==============================================================================
VOID DrawXText(CHAR* String, UINT L, UINT T, UINT W, UINT H, D3DCOLOR Color, DWORD Align=DT_LEFT)
{
	Rect.left = L;
	Rect.top = T;
	Rect.right = L+W;
	Rect.bottom = T+H;
	Font->DrawText(0, String, -1, &Rect, Align, Color);
}
//==============================================================================
VOID SetSmooth()
{
	if (!SmoothMODE) {
		Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
		Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
		return;
	}
	if (SmoothMODE > 1)
		Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	else
		Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	if (SmoothMODE > 2) {
		Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	}
	else {
		Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}
}
//==============================================================================
VOID LoadArea()
{
	new OBJECT3D("world.x");
	CurrentMap = new OBJECT3D("map.x");
	new OBJECT3D("cyl.x", false);

//	Car[0] = new CAR("Bin/Vehicle/VAZ21099/body.x", "wheel.x", 1.245f, 0.28f, 0.69f, 0.69f);
//	Car[0] = new CAR("Bin/Vehicle/GMCRTS/body.x", "bus_wheel.x", 2.58f, 0.36f, 0.8f, 0.8f);
	Car[0] = new CAR("Bin/Vehicle/EVOX/body.x", "wheel2.x", 1.3f, 0.34f, 0.8f, 0.85f);
//	Car[1] = new CAR("Bin/Vehicle/FORDGT/body.x", "wheel2.x", 1.36f, 0.34f, 0.78f, 0.78f);
//	Car[0] = new CAR("Bin/Vehicle/GMCSAVANA/body.x", "wheel.x", 1.29f, 0.28f, 0.62f, 0.62f);
	
	CurrentObject = Car[0];

	new MESSAGE("Добро пожаловать в игру!", 3);
	new MESSAGE("Для справки нажмите F1!", 10);
	new MESSAGE("Приятного тестирования!", 4);
}
//==============================================================================
/*VOID Music(char* MusicPath)
{
	CoInitialize(NULL);
	CoCreateInstance(CLSID_DirectMusicPerfomance, NULL, CLSCTX_INPROC,
		IID_IDirectMusicPerfomance8, (void**)&Perform);
	CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC,
		IID_IDirectMusicLoader8, (void**)&Load);
//	Perform->Init
}*/
//==============================================================================
VOID InitInput()
{
	DirectInput8Create(windowsclass.hInstance, DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**) &Input, NULL);
	HRESULT hr;
	hr = Input->CreateDevice(GUID_SysKeyboard, &Keyboard, NULL);
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось установить устройство клавиатуры", 
			"Клавиатура", MB_OK); return;
		InitInput();
	}
	hr = Keyboard->SetCooperativeLevel(hwnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось установить уровень взаемодействия"
			"с клавиатурой", "Клавиатура", MB_OK); return;
		InitInput();
	}
	hr = Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось установить формат данных клавиатуры", 
			"Клавиатура", MB_OK); return;
		InitInput();
	} 
	hr = Keyboard->Acquire();
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось захватить клавиатуру", 
			"Клавиатура", MB_OK); return;
		InitInput();
	}
	hr = Input->CreateDevice(GUID_SysMouse, &Mouse, NULL);
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось установить устройство мыши", 
			"Мышь", MB_OK); return;
		InitInput();
	}
	hr = Mouse->SetCooperativeLevel(hwnd,DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось установить уровень взаемодействия"
			"с мышей", "Мышь", MB_OK); return;
		InitInput();
	} 
	hr = Mouse->SetDataFormat(&c_dfDIMouse);
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось установить формат данных мыши", 
			"Мышь", MB_OK); return;
		InitInput();
	}
	hr = Mouse->Acquire();
	if FAILED(hr) {
		DeleteInput();
		MessageBox(hwnd, "Не удалось захватить мышь", 
			"Мышь", MB_OK); return;
		InitInput();
	}
}
//==============================================================================
VOID DeleteInput()
{
	if (Input)
	{
		if(Keyboard)
		{
			if (Keyboard != NULL)
				Keyboard->Unacquire();
			if (Keyboard != NULL)
				Keyboard->Release();
			Keyboard = NULL;
		}
		if(Mouse)
		{
			if (Mouse != NULL)
				Mouse->Unacquire();
			if (Mouse != NULL)
				Mouse->Release();
			Mouse = NULL;
		}
		if (Input != NULL)
			Input->Release();
		Input = NULL;
	}
}
//=== Захват уствойств ввода ===================================================
VOID GetInput()
{
    static bool LostFocus=false;
	// не захоплюємо керування якщо вікно не в фокусі
	if (hwnd != GetFocus()) {LostFocus=true; return;}
	if (LostFocus) {DeleteInput(); InitInput(); LostFocus=false;}

	HRESULT hr=0;	
	hr = Keyboard->GetDeviceState(256, (LPVOID)&Key);
	if FAILED(hr) 
	{
		DeleteInput();
		MessageBox(hwnd, "Не удалось считать данные с клавиатуры", 
			"Клавиатура", MB_OK);
		return;
	}
	hr = Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&MouseState);
	if FAILED(hr) 
	{
		DeleteInput();
		MessageBox(hwnd, "Не удалось считать данные с мыши", 
			"Мышь", MB_OK);
		return;
	}
}
//=== Движок ===========================================================================
VOID Engine()
{
	static bool f1=false, f2=false, f3=false, f4=false, f5=false, m=false;
	// берем данные с устройств ввода
	GetInput();
	if (Key[DIK_ESCAPE]) exit(0);
	// лимит FPS (удерживать)
	if (Key[DIK_F])	LimitFps = TRUE;
	else LimitFps = FALSE;
	if (Key[DIK_F1]) { 
	// режим дебага
	if (!f1) DebMODE = !DebMODE; f1 = TRUE;}
	else f1 = FALSE;
	// режим сетки
	if (Key[DIK_F2]) { 
		if (!f2) {if (MeshMODE) Device->SetRenderState(D3DRS_FILLMODE, NULL); 
		else Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); MeshMODE = !MeshMODE; f2 = TRUE;}}
	else f2 = FALSE;
	if (Key[DIK_F3])  
	{ if (!f3) NoMatMODE = !NoMatMODE; f3 = TRUE;}
	else f3 = FALSE;
	if (Key[DIK_F4])  
	{ if (!f4) NoTexMODE = !NoTexMODE; f4 = TRUE;}
	else f4 = FALSE;
	if (Key[DIK_F5])  
	{ if (!f5) {SmoothMODE++; if (SmoothMODE>3) SmoothMODE = 0;} SetSmooth(); f5 = TRUE;}
		else f5 = FALSE;
	//левая кнопка мыши
	//if (MouseState.rgbButtons[MOUSE_LB]&0x80)
//	if (Key[DIK_M])  
//	{ if (!m) new MESSAGE("132132131", rand()%10); m = TRUE;}
//	else m = FALSE;
	
	for (int i=0; i<OBJECT3D::Count; i++)
		Obj[i]->Physic();
	if (CurrentObject)
		CurrentObject->Control();
	
	

	// Стереть нахер
	if (Key[DIK_W]) {
		if (Car[1]->Speed < 100)
			Car[1]->Speed += 0.4f;
		Car[1]->BREAK_FLAG = FALSE;
	}
	if (Key[DIK_S]) {
		if (Car[1]->Speed > -25)
			Car[1]->Speed -= 1.f;
		Car[1]->BREAK_FLAG = FALSE;
	}
	if (Key[DIK_A]) {
		Car[1]->WA_FLAG = FALSE;
		if (Car[1]->WheelAngle > -0.7f)
			Car[1]->WheelAngle -= 0.02f;
	}
	if (Key[DIK_D]) {
		Car[1]->WA_FLAG = FALSE;
		if (Car[1]->WheelAngle < 0.7f)
			Car[1]->WheelAngle += 0.02f;
	}

	// test zone
}
//=== Установка освещения ======================================================
VOID LightMaterial()
{
	ZeroMemory(&Material, sizeof(D3DMATERIAL9));
	Material.Diffuse.r = Material.Ambient.r = 1.0f;
	Material.Diffuse.g = Material.Ambient.g = 1.0f;
	Material.Diffuse.b = Material.Ambient.b = 1.0f;
	Material.Diffuse.a = Material.Ambient.a = 1.0f;
	Device->SetMaterial(&Material);
	D3DXVECTOR3 VectorDir;

	ZeroMemory(&Light, sizeof(D3DMATERIAL9));
	Light.Type = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r = 
	Light.Diffuse.g = 
	Light.Diffuse.b = 
	Light.Ambient.r =
	Light.Ambient.g = 
	Light.Ambient.b = 0.5f;
	VectorDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &VectorDir);
	Device->SetLight(0, &Light);

	Light.Diffuse.r = 
	Light.Diffuse.g = 
	Light.Diffuse.b = 1.f;
	Light.Ambient.r =
	Light.Ambient.g = 
	Light.Ambient.b = 0.1f;
	VectorDir = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &VectorDir);
	Device->SetLight(1, &Light);
	
/*	Light.Diffuse.r = 
	Light.Diffuse.g = 
	Light.Diffuse.b =
	Light.Ambient.r =
	Light.Ambient.g = 
	Light.Ambient.b = -0.2f;
	VectorDir = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &VectorDir);
	Device->SetLight(2, &Light);
*/
	// ФОНАРИК
/*	Light.Type = D3DLIGHT_SPOT;
	Light.Diffuse = D3DXCOLOR(10.0f, 10.0f, 10.0f, 0.0f);
	Light.Position = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	D3DXVec3Normalize( (D3DXVECTOR3*)&Light.Direction, &direction );
	Light.Attenuation0 = 1.0f;
	Light.Range = 50;
	Light.Phi = D3DX_PI/2;
	Light.Theta = D3DX_PI/3;
	Light.Falloff = 1.0f;
	Device->SetLight(2, &Light);
	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(0.0f,-0.3f,0.5);
	D3DXVec3Normalize( (D3DXVECTOR3*)&Light.Direction, &vecDir );
	Device->SetLight(2, &Light);
*/

	Device->LightEnable(0, TRUE);
	Device->LightEnable(1, TRUE);
//	Device->LightEnable(2, FALSE);

	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	Device->SetRenderState(D3DRS_AMBIENT, 0);	 
}
//==============================================================================
HRESULT InitDirect3D()
{
//	FullScreen = false;
//	if (MessageBox(hwnd, "Включить полноэкранный режим? м?", "Полноэкранка ;)",
//		MB_YESNO|MB_ICONQUESTION) != 7)
	FullScreen = true;
	if (NULL == (pDirect3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;
	D3DDISPLAYMODE Display;
	if (FAILED(pDirect3D->
			GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display)))
		return E_FAIL;
	D3DPRESENT_PARAMETERS Direct3DParametr;
	ZeroMemory(&Direct3DParametr, sizeof(Direct3DParametr));
	Direct3DParametr.Windowed = !FullScreen;
	Direct3DParametr.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Direct3DParametr.BackBufferFormat = Display.Format;
	
	//полноэкранное сглаживание
//	if (RELEASE)
	{
		if(SUCCEEDED(pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			Direct3DParametr.BackBufferFormat, FALSE, D3DMULTISAMPLE_2_SAMPLES, NULL)))
			Direct3DParametr.MultiSampleType=D3DMULTISAMPLE_2_SAMPLES;
//		if(SUCCEEDED(pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
//			Direct3DParametr.BackBufferFormat, FALSE, D3DMULTISAMPLE_4_SAMPLES, NULL)))
//			Direct3DParametr.MultiSampleType=D3DMULTISAMPLE_4_SAMPLES;
//		Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,DWORD(TRUE) );
//		Device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE,DWORD(TRUE) );
	}

	Direct3DParametr.EnableAutoDepthStencil = TRUE;	//установка Z-буфера
	Direct3DParametr.AutoDepthStencilFormat = D3DFMT_D24S8;

	if (FullScreen)
	{
		ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		Direct3DParametr.BackBufferWidth = ScreenWidth;
		Direct3DParametr.BackBufferHeight = ScreenHeight;
		Direct3DParametr.BackBufferCount = 3;
		Direct3DParametr.FullScreen_RefreshRateInHz = Display.RefreshRate;
	}
	else {
		ScreenWidth = WINWIDTH;
		ScreenHeight = WINHEIGHT;
	}	
	Correlation = ScreenWidth/float(ScreenHeight);
	// пробуем установить обработку аппаратно
	if (FAILED(pDirect3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, hwnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&Direct3DParametr, &Device))) {
		// при неудаче устанавливает программно
		new MESSAGE("Ваша карта не поппдерживает апаратное ускорение :(", 5);
		if (FAILED(pDirect3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, hwnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&Direct3DParametr, &Device)))
			// закрываем приложение если не удалось согдать устройство
			return E_FAIL;
	}

	// отключить освещение
	//Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	Device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	// туман
	DWORD Color = 0x808080; // Цвет - белый 
	DWORD Mode = D3DFOG_LINEAR; // Вид
	float Start = 50.f;
	float End = 200.f;
	float Density = 0.05f;
	Device->SetRenderState(D3DRS_FOGCOLOR, Color);
	Device->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
	Device->SetRenderState(D3DRS_FOGTABLEMODE, Mode);
    // Ставим началное и конечное расстояние для тумана
    Device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
    Device->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));

	// для прозрачных текстур
	Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );    
    Device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	LightMaterial();
	return S_OK;	
}
//==============================================================================
VOID Render()
{
	if (Device == NULL)
		return;
	HRESULT hr=0;
	// Тест потери D3D-устройства
    if (FAILED(hr=Device->TestCooperativeLevel()))
    {
		GameMode = 0; // Зупинка гри
		return;
    }
	Device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	
	// Начало сцены
	Device->BeginScene();
	Device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	
/*	IDirect3DTexture9* RenderTexture;
	D3DXCreateTexture(Device, 512, 512, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, &RenderTexture);
    IDirect3DSurface9* BackBuffer = 0;
    Device->GetRenderTarget(0, &BackBuffer);
    IDirect3DSurface9* RenderSurface = NULL;
    RenderTexture->GetSurfaceLevel(0, &RenderSurface);
    Device->SetRenderTarget(0, RenderSurface);
    
	
	
	
	
	// Проекция
	D3DXMatrixPerspectiveFovLH(&Projection, D3DX_PI/2, 
		 Correlation, 0.1f, 3000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &Projection);
	// end


	// Установка камеры
	//D3DXMatrixLookAtLH(&Camera, &D3DXVECTOR3(0.0f, CamAngleX, CamAngleY),
	D3DXMatrixLookAtLH(&Camera, &D3DXVECTOR3(0.00001f, 0.0f, 0),
        &D3DXVECTOR3(0.0f, 1.0f, 0.0f),
        &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixRotationY(&Wheel1, -CurrentObject->AngleY);
	D3DXMatrixMultiply(&Camera, &Wheel1, &Camera);
	D3DXMatrixReflect(&Wheel1, &D3DXPLANE(0,1,0,0.3));
	D3DXMatrixMultiply(&Camera, &Camera, &Wheel1);
	
	//if (CurrentObject)
	//	D3DXMatrixTranslation(&Wheel1, -CurrentObject->X, -CurrentObject->Y, -CurrentObject->Z);
	//D3DXMatrixMultiply(&Camera, &Wheel1, &Camera); 
	Device->SetTransform(D3DTS_VIEW, &Camera);
	D3DXMatrixTranslation(&World, -CurrentObject->X, -CurrentObject->Y, -CurrentObject->Z);
	Device->SetTransform(D3DTS_WORLD, &World);
        // тут рендерится сцена
		Obj[0]->Draw();
		Obj[1]->Draw();
    Device->SetRenderTarget(0, BackBuffer);
	if (Obj[3]->MeshTexture[0])
		Obj[3]->MeshTexture[0]->Release();
	Obj[3]->MeshTexture[0] = RenderTexture;
	RenderSurface->Release();
	BackBuffer->Release();

	Device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
*/

	
	// Установка камеры
//	if (CurrentObject->AngleY>CamAngleY)
//		CamAngleY+=0.01f;
//	else CamAngleY-=0.01f;


	D3DXMatrixTranslation(&World, CurrentObject->X, 0, CurrentObject->Z);
	Device->SetTransform(D3DTS_WORLD, &World);
	Obj[0]->Draw();
	Device->SetRenderState(D3DRS_FOGENABLE, TRUE);
	D3DXMatrixTranslation(&World, 0, 0, 0);
	Device->SetTransform(D3DTS_WORLD, &World);
	// Отрисовка сцены
	for (int i=1; i<OBJECT3D::Count; i++)
		if (Obj[i]->DrawEnabled && Obj[i]!=CurrentObject)
			Obj[i]->Draw();
	// end
	if (CurrentObject)
		D3DXMatrixTranslation(&WorldMove, CurrentObject->X,  CurrentObject->Y,  CurrentObject->Z);
	Device->SetTransform(D3DTS_WORLD, &WorldMove);
	
	// Отрисовка текущего объекта
//	Device->SetRenderState(D3DRS_FOGENABLE, TRUE);
	if (CurrentObject)
		CurrentObject->Draw();
	for (int i=0; i<MESSAGE::Count; i++)
		Message[i]->Show(i, ScreenWidth, ScreenHeight);
	// Вывод текста
    DrawXText(STR_FPS, 20, 20, 200, 100, D3DCOLOR_ARGB(125, 250, 250, 50));
	DrawXText(RENDER_TIME, 20, 50, 200, 150, D3DCOLOR_ARGB(125, 250, 250, 50));
	DrawXText(STR_SPEED, 20, 80, 200, 200, D3DCOLOR_ARGB(125, 250, 250, 50));
	if (DebMODE)
		DrawXText(INFORMER, 20, 140, 800, 600, D3DCOLOR_ARGB(125, 250, 250, 250));
	else
		DrawXText(HELP, 20, 140, 300, 100, D3DCOLOR_ARGB(125, 250, 250, 250));

	// Конец сцены
	Device->EndScene();
	Device->Present(NULL, NULL, NULL, NULL);
}
//==============================================================================
VOID DeleteDirect3D()
{
	if (Device != NULL)
		Device->Release();
	if (pDirect3D != NULL)
		pDirect3D->Release();
}
//==============================================================================
LRESULT CALLBACK MainWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}
//==============================================================================
int WINAPI WinMain(HINSTANCE	hinstance,
				   HINSTANCE	hprevinstance,
				   LPSTR		lpcmdline,
				   int			ncmdshow)
{
	MSG msg;
	UINT StartTime=0, FPS=0, FPSTime=0, FrapsTime;
	windowsclass.cbSize = sizeof(WNDCLASSEX);										// размер класса
	windowsclass.style =  CS_OWNDC | CS_DBLCLKS;												// стили окна
	windowsclass.lpfnWndProc = MainWinProc;											// указание функции обработчика
	windowsclass.cbClsExtra = 0;													// дополнительные поля
	windowsclass.cbWndExtra = 0;
	windowsclass.hInstance = hinstance;												// экземпляр приложения
	windowsclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);															// иконка
	windowsclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);															// иконка на панели
	windowsclass.hCursor = LoadCursor(NULL, IDC_ARROW);																	// курсор
	windowsclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);											// цвет фона
	windowsclass.lpszMenuName = NULL;												// менушка
	windowsclass.lpszClassName = "WINDOWSCLASS";									// имя формы
	if (!RegisterClassEx(& windowsclass))	// регестрируем класс
		return 0;
	if (!(hwnd = CreateWindowEx(NULL,
		"WINDOWSCLASS","Awesome game : by B&A Games Corporation",
			//  без рамки
			WS_POPUP | 
			WS_OVERLAPPED | 
			WS_VISIBLE,
			 GetSystemMetrics(SM_CXSCREEN)/2-400,
			 GetSystemMetrics(SM_CYSCREEN)/2-150,
			WINWIDTH,WINHEIGHT,NULL,NULL,
			hinstance,NULL)))
	   return 0;
	if (SUCCEEDED(InitDirect3D()))
	{
		InitInput();
		InitDrawText();
		LoadArea();
		
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
		ZeroMemory(&msg, sizeof(msg));
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) break;
				switch(msg.message)
				{
					case WM_ACTIVATEAPP : ; ResumeGame(); break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				if (GameMode) 
				{
					if (timeGetTime() - StartTime > 33 || LimitFps)	
					{
						StartTime = timeGetTime();					
						Engine();
						Render();
						FrapsTime = timeGetTime() - StartTime;
						if (FPS%10 == 0)
							sprintf_s(&RENDER_TIME[8], 10, "%d", FrapsTime);
						FPS++;
					}
					if (FPSTime+1000 < timeGetTime())
					{
						sprintf_s(&STR_FPS[5], 10, "%d", FPS);
						FPSTime = timeGetTime();
						FPS=0;
					}
				}
		}
	}
	//DeleteDrawText();
	//DeleteInput();
	//DeleteDirect3D();
	return 0;
}
//==============================================================================