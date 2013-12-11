// камеры
#define CAM_FOLLOW 0
#define CAM_FIXED  1
//=== Класс объекта ===============================================================
//=== Класс 3D объекта ============================================================
class OBJECT3D;
class CAMERA;
class CAR;
struct MESSAGE;
MESSAGE *Message[25];
OBJECT3D *Obj[1000];
VOID DrawXText(CHAR* String, UINT L, UINT T, UINT W, UINT H, D3DCOLOR Color, DWORD Align);
//==============================================================================
struct MESSAGE
{
    char *Text;
	int Time, CurrTime, Opacity, Offset, NextOffset, StartOffset;
	static int Count;
	MESSAGE(char *text, int time)
	{
		Message[Count] = this;
		Count++;
	    Text = new char[strlen(text)+1];
		strcpy_s(Text, strlen(text)+1, text);
		Time = time*30+15;
		CurrTime=-15;
		NextOffset=15;
	}
	~MESSAGE() {
		bool flag = false;
		for (int i=0; i<Count; i++) {
			if (Message[i] == this) flag = true;
			if (flag) Message[i] = Message[i+1];
		}
		Count--; 
		delete[] Text; 
	}
	void Show(int no, int x,int y)
	{
		CurrTime++;
		if (CurrTime<0) { 
			Offset = 15+CurrTime;
			Opacity = Offset*12;
		}
		if (Time-CurrTime<15) {
			Opacity = (Time-CurrTime)*12;
			NextOffset = Time-CurrTime;
		}
		StartOffset=0;
		for (int i=0; i<no; i++)
			StartOffset += Message[i]->NextOffset;
		DrawXText(Text, 20, StartOffset*2+30-Offset*2+20, x-40, y-40, 
			D3DCOLOR_ARGB(Opacity, 250, 250, 50), DT_RIGHT);
		if (Time == CurrTime) {
			delete this;
		}
	}
};
int MESSAGE::Count=0;
//==============================================================================
class OBJECT3D
{
private:
	LPD3DXBUFFER		MeshBuffer;
	DWORD				Number;
	char				CurAddress[ADDRESS_SIZE];
	void Load(char* Address)
	{
		Mesh = 0;
		MeshBuffer = 0;
		MeshMaterials = 0;

		D3DXLoadMeshFromX(Address, D3DXMESH_SYSTEMMEM, Device,
			NULL, &MeshBuffer, NULL, &Number, &Mesh);
		D3DXMATERIAL* D3DXMeshMaterials;


		D3DXMeshMaterials = (D3DXMATERIAL *)MeshBuffer->GetBufferPointer();
		MeshMaterials	= new D3DMATERIAL9[Number];
		MeshTexture		= new LPDIRECT3DTEXTURE9[Number];
		for (DWORD i=0; i<Number; i++)
		{
			// копируем материал
			MeshMaterials[i] = D3DXMeshMaterials[i].MatD3D;
			// устанавиваем окружающий свет
			MeshMaterials[i].Ambient = MeshMaterials[i].Diffuse;
			// загружаем текстуру
			if (FAILED(D3DXCreateTextureFromFile(Device,
				D3DXMeshMaterials[i].pTextureFilename, &MeshTexture[i])))
				MeshTexture[i] = NULL;
		}
		// уничтожаем материальный буфер
		MeshBuffer->Release();
		strcpy_s(CurAddress, Address);
	}
public:
	float X, Y, Z;
	float AngleX, AngleY, AngleZ;
	LPDIRECT3DTEXTURE9* MeshTexture;
	D3DMATERIAL9*		MeshMaterials;
	LPD3DXMESH			Mesh;
	BOOL				DrawEnabled;
	static int			Count;
	void SetPosition(float x, float y, float z) { X=x; Y=y; Z=z; };
	OBJECT3D(char* Address, bool Visible=true)
	{
		X=Y=Z=0;
		Obj[Count] = this;
		if (Visible) DrawEnabled=true;
		else DrawEnabled=false;
		Load(Address);
		Count++;
	}
	virtual ~OBJECT3D()
	{
		Release();
		// удаляем из списка всех объектов 3D
		bool flag = false;
		for (int i=0; i<Count; i++)
		{
		    if (this == Obj[i])
				flag = true;
			if (flag)
				Obj[i] = Obj[i+1];
		}
		Count--;
	}
	void Release()
	{
		for (DWORD i=0; i<Number; i++)
			if (MeshTexture[i])
				MeshTexture[i]->Release();
		delete[] MeshTexture;
		delete[] MeshMaterials;
		Mesh->Release();
	}
	virtual void Draw()
	{
		for (DWORD i=0; i<Number; i++)
		{

			if (!NoMatMODE)
				Device->SetMaterial(&MeshMaterials[i]);
			if (!NoTexMODE)
				Device->SetTexture(0, MeshTexture[i]);
			Mesh->DrawSubset(i);
		}
	}
	void Refresh()
	{
		ChangeMesh(CurAddress);
	}
	void ChangeMesh(char* Address)
	{
		Load(Address);
	}
	virtual void Physic() {};
	virtual void Control() {};
} *CurrentObject, *CurrentMap;
int OBJECT3D::Count=0;
//==============================================================================
class CAMERA
{
private:   
	float CamX, CamY, CamZ;
	float CamAngleX, CamAngleY, CamAngleZ, TempAngle, Step;
	int CamBlock;
public:
	int Type;
	CAMERA(int type, float x, float y, float z)
	{
		Type = type;
		CamBlock=150;
		CamAngleY=D3DX_PI; 
		CamX=x;
		CamY=y;
		CamZ=z;
	}
	void Set(int type, float ObjAngle, float Speed)
	{
		// применяем управление пользователя
		if (MouseState.lY)
		{
			CamY -= MouseState.lY/50.f;
			CamBlock=-60;
		}
		if (CamY<0.5f) CamY = 0.5f;
		if (CamY>10.f) CamY = 10.f;
		CamAngleY -= MouseState.lX/100.f;
		// переводим градус объекта с диапазона (Pi) - (-Pi)
		// в диапазон 0 - 2Pi
		if (ObjAngle>0) TempAngle = ObjAngle-D3DX_PI;
		else TempAngle = D3DX_PI*3.f+ObjAngle;
		if (Speed<0) TempAngle += D3DX_PI;
		if (TempAngle>D3DX_PI*2.f) TempAngle -= D3DX_PI*2.f;
		// выставляем шаг в зависимости от разници
		Step = fabs(TempAngle-CamAngleY);
		// корректируем разницу если переход после (0, 2Pi)
		if (Step>D3DX_PI) Step = D3DX_PI*2.f - Step;
		// расчет конечного шага
		Step = Step/5.f*(CamBlock/100.f);
		// меняем угол по шагу если камера не управляется пользователем
		if (fabs(TempAngle-CamAngleY)>Step && CamBlock>0)
		{
			if ((TempAngle>CamAngleY && TempAngle-CamAngleY<D3DX_PI) || 
				TempAngle+D3DX_PI < CamAngleY)
				CamAngleY += Step;
			else	CamAngleY -= Step;
		}
		Step = fabs(CamY-2.4f)/25.f;
		if (fabs(CamY-2.4f)>Step && CamBlock>0)
			if (CamY < 2.4f) CamY += Step;	
			else CamY -= Step;
		if (CamBlock < 150)
			CamBlock++;
		// Корректировака выхода за диапазон (0-2Pi)
		if (CamAngleY>D3DX_PI*2.f) CamAngleY = CamAngleY-D3DX_PI*2.f;
		if (CamAngleY<0) CamAngleY += D3DX_PI*2.f;
		if (Type == type)
		{
			// установка камеры
			D3DXMatrixLookAtLH(&Camera, &D3DXVECTOR3(CamX, CamY, CamZ),
				&D3DXVECTOR3(0,0,0),
				&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			D3DXMatrixRotationY(&Wheel1, D3DX_PI-CamAngleY);//-D3DX_PI/2.f);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera);
			if (CurrentObject)
				D3DXMatrixTranslation(&Wheel1, -CurrentObject->X, -CurrentObject->Y, -CurrentObject->Z);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera); 
			Device->SetTransform(D3DTS_VIEW, &Camera);
			// установка роекции
			D3DXMatrixPerspectiveFovLH(&Projection, D3DX_PI/4, 
				 Correlation, 0.1f, 7500.0f);
			Device->SetTransform(D3DTS_PROJECTION, &Projection);
			// end
		}
	}
	void Set(int type, float ObjAngleZ, float ObjAngleY, float Speed)
	{
		// применяем управление пользователя
		if (MouseState.lY)
		{
			CamAngleZ -= MouseState.lY/50.f;
			CamBlock=-60;
		}
		if (CamAngleZ<-D3DX_PI/2.f) CamAngleZ = -D3DX_PI/2.f;
		if (CamAngleZ>D3DX_PI/2.f) CamAngleZ = D3DX_PI/2.f;
		CamAngleY -= MouseState.lX/100.f;
		float az = ObjAngleZ;
		float ay = ObjAngleY;
		// переводим градус объекта с диапазона (Pi) - (-Pi)
		// в диапазон 0 - 2Pi
		if (ObjAngleY>0) TempAngle = ObjAngleY-D3DX_PI;
		else TempAngle = D3DX_PI*3.f+ObjAngleY;
		if (Speed<0) TempAngle += D3DX_PI;
		if (TempAngle>D3DX_PI*2.f) TempAngle -= D3DX_PI*2.f;
		// выставляем шаг в зависимости от разници
		Step = fabs(TempAngle-CamAngleY);
		// корректируем разницу если переход после (0, 2Pi)
		if (Step>D3DX_PI) Step = D3DX_PI*2.f - Step;
		// расчет конечного шага
		Step = Step/5.f*(CamBlock/100.f);
		// меняем угол по шагу если камера не управляется пользователем
		if (fabs(TempAngle-CamAngleY)>Step && CamBlock>0)
		{
			if ((TempAngle>CamAngleY && TempAngle-CamAngleY<D3DX_PI) || 
				TempAngle+D3DX_PI < CamAngleY)
				CamAngleY += Step;
			else	CamAngleY -= Step;
		}

		TempAngle = -az;
		Step = fabs(TempAngle-CamAngleZ)/10.f;
		if (CamBlock>0)
			if (CamAngleZ<TempAngle) CamAngleZ += Step;	
			else CamAngleZ -= Step;
		if (CamBlock < 150)
			CamBlock++;
		// Корректировака выхода за диапазон (0-2Pi);*/
		if (CamAngleY>D3DX_PI*2.f) CamAngleY = CamAngleY-D3DX_PI*2.f;
		if (CamAngleY<0) CamAngleY += D3DX_PI*2.f;
		if (Type == type)
		{
			// установка камеры		
			D3DXMatrixLookAtLH(&Camera, &D3DXVECTOR3(0, 0, 0),
				&D3DXVECTOR3(-10.f,0,0),
				&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			D3DXMatrixRotationZ(&Wheel1, CamAngleX);//-D3DX_PI/2.f);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera);
			D3DXMatrixRotationZ(&Wheel1, CamAngleZ+az);//-D3DX_PI/2.f);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera);
			D3DXMatrixRotationY(&Wheel1, CamAngleY-ay-D3DX_PI);//-D3DX_PI/2.f);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera);
			D3DXMatrixTranslation(&Wheel1, -CamX, -CamY, -CamZ);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera); 
			D3DXMatrixRotationZ(&Wheel1, -az);//-D3DX_PI/2.f);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera);
			D3DXMatrixRotationY(&Wheel1, -ay);//-D3DX_PI/2.f);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera);
			if (CurrentObject)
				D3DXMatrixTranslation(&Wheel1, -CurrentObject->X, -CurrentObject->Y, -CurrentObject->Z);
			D3DXMatrixMultiply(&Camera, &Wheel1, &Camera); 
			Device->SetTransform(D3DTS_VIEW, &Camera);
			// установка роекции
			D3DXMatrixPerspectiveFovLH(&Projection, D3DX_PI/4, 
				 Correlation, 0.1f, 7500.0f);
			Device->SetTransform(D3DTS_PROJECTION, &Projection);
			// end	
		}
	}
};
//==============================================================================
class CAR : public OBJECT3D
{
public:
	OBJECT3D *Wheel[4];
	CAMERA *Camera[4]; 
	CAMERA *CurrentCamera;
	BOOL Hit, WA_FLAG, BREAK_FLAG;
	FLOAT fx, fy, fz, rx, ry, rz;
	FLOAT WheelAngle,
		  RotateSpeed;
	FLOAT WheelOffsetX, 
		  FWheelZ, 
		  RWheelZ, 
		  WheelRadius,  
		  WheelOffset;
	FLOAT OffsetX, 
		  OffsetZ, 
		  Speed,
		  Break,
		  BreakSpeed,
		  Step,
		  GSpeed1,
		  GSpeed2;
	// Временные
	BOOL e,q,c;
	CAR(char* BodyAddress, char* WheelAddress, float woffset, float wradius, 
		float fwlenght, float rwlenght) : OBJECT3D(BodyAddress)
	{
		for (int i=0; i<4; i++) Wheel[i] = new OBJECT3D(WheelAddress, false);
	    Camera[0] = new CAMERA(CAM_FOLLOW, 8.f, 2.4f, 0);
		Camera[1] = new CAMERA(CAM_FIXED, 0.1f, 1.15f, -0.38f);
		CurrentCamera = Camera[1];
		WheelOffsetX=woffset; 
		FWheelZ=-fwlenght; 
		RWheelZ=-rwlenght;
		fx=X-WheelOffsetX;
		rx=X+WheelOffsetX;
		fy=15;
		ry=15;
		fz=0;
		rz=0;
		AngleX=0; 
		AngleY=0; 
		AngleZ=0; 
		WheelRadius=wradius;
		WheelAngle=0;
		WheelOffset=0;
		OffsetX=0; 
		OffsetZ=0;
		Speed=0.1f;
		Break=0;
		BreakSpeed=0.5f;

		// временно
		e=q=c=false;
	}
	~CAR()
	{
	    for (int i=0; i<4; i++) delete Wheel[i];
		for (int i=0; i<4; i++) {if (Camera[i]) delete Camera[i];}
	}
	void Draw()
	{
		// Корпус
	    D3DXMatrixRotationX(&WorldX, AngleX);
		D3DXMatrixRotationY(&WorldY, AngleY);
		D3DXMatrixRotationZ(&WorldZ, AngleZ);
		D3DXMatrixMultiply(&WorldX, &WorldX, &WorldZ);
		D3DXMatrixMultiply(&WorldY, &WorldX, &WorldY);
		D3DXMatrixTranslation(&WorldMove, X, Y, Z);
		D3DXMatrixMultiply(&World, &WorldY, &WorldMove);
		Device->SetTransform(D3DTS_WORLD, &World);
		OBJECT3D::Draw();
		// Колесо переднее левое
		D3DXMatrixRotationZ(&WorldY, -WheelOffset);
		D3DXMatrixRotationY(&WorldMove, WheelAngle+3.15f);
		D3DXMatrixMultiply(&Wheel1, &WorldY, &WorldMove);
		D3DXMatrixTranslation(&WorldMove, -WheelOffsetX, WheelRadius, FWheelZ);
		D3DXMatrixMultiply(&Wheel2, &WorldMove, &World);
		D3DXMatrixMultiply(&Wheel1, &Wheel1, &Wheel2);
		Device->SetTransform(D3DTS_WORLD, &Wheel1);
		Wheel[0]->Draw();
		// Колесо переднее правое
		D3DXMatrixRotationZ(&WorldY, WheelOffset);
		D3DXMatrixRotationY(&WorldMove, WheelAngle);
		D3DXMatrixMultiply(&Wheel1, &WorldY, &WorldMove);
		D3DXMatrixTranslation(&WorldMove, -WheelOffsetX, WheelRadius, -1*FWheelZ);
		D3DXMatrixMultiply(&Wheel2, &WorldMove, &World);
		D3DXMatrixMultiply(&Wheel1, &Wheel1, &Wheel2);
		Device->SetTransform(D3DTS_WORLD, &Wheel1);
		Wheel[1]->Draw();
		// Колесо заднее левое
		D3DXMatrixRotationZ(&WorldY, -WheelOffset);
		D3DXMatrixRotationY(&WorldMove, 3.15f);
		D3DXMatrixMultiply(&Wheel1, &WorldY, &WorldMove);
		D3DXMatrixTranslation(&WorldMove, WheelOffsetX, WheelRadius, RWheelZ);
		D3DXMatrixMultiply(&Wheel2, &WorldMove, &World);
		D3DXMatrixMultiply(&Wheel1, &Wheel1, &Wheel2);
		Device->SetTransform(D3DTS_WORLD, &Wheel1);
		Wheel[2]->Draw();
		// Колесо заднее правое
		D3DXMatrixRotationZ(&WorldY, WheelOffset+1);
		D3DXMatrixTranslation(&WorldMove, WheelOffsetX, WheelRadius, -1*RWheelZ);
		D3DXMatrixMultiply(&Wheel1, &WorldY, &WorldMove);
		D3DXMatrixMultiply(&Wheel1, &Wheel1, &World);
		Device->SetTransform(D3DTS_WORLD, &Wheel1);
		Wheel[3]->Draw();
		if (!RELEASE)
		{
			D3DXMatrixTranslation(&Wheel1, rx, ry, rz);
			Device->SetTransform(D3DTS_WORLD, &Wheel1);
			Obj[2]->Draw();
			D3DXMatrixTranslation(&Wheel1, fx, fy, fz);
			Device->SetTransform(D3DTS_WORLD, &Wheel1);
			Obj[2]->Draw();
		}
	}
	void Physic()
	{
		RotateSpeed = WheelAngle/400.f;
		if (abs(WheelAngle) < 0.01f)
			RotateSpeed = 0;
	    if (WheelAngle)
			if (WA_FLAG)
			{	
				if (WheelAngle > 0)
					WheelAngle -= 0.02f;
				else
					WheelAngle += 0.02f;
				if (fabs(WheelAngle) < 0.02f) 
					WheelAngle = 0;
			}
		if (BREAK_FLAG && Speed)
		{
			if (Speed > 0)
				Speed -= BreakSpeed+Break;
			else
				Speed += BreakSpeed+Break;
			if (fabs(Speed) < 0.5f+Break)
				Speed = 0;
		}
		if (Speed)
		{
			if (AngleY > D3DX_PI)
			    AngleY = -D3DX_PI*2.f + AngleY;
			if (AngleY < -D3DX_PI)
			    AngleY = D3DX_PI*2.f + AngleY;
			OffsetZ = sin(AngleY+WheelAngle)*0.01f;
			OffsetX = -1*cos(AngleY+WheelAngle)*0.01f;
		}
		
		fx += OffsetX*Speed;
		fz += OffsetZ*Speed;

		float tx = fx-rx;
		float tz = fz-rz;
		AngleY = D3DX_PI+acos(tx/sqrt(tx*tx+tz*tz));

		if (fz>rz)
			AngleY *= -1.f;
		rx = fx + sin(D3DX_PI/2.f-AngleY)*WheelOffsetX*2.f;
		rz = fz - sin(AngleY)*WheelOffsetX*2.f;
		
		X=(fx+rx)/2.f;
		Y=(fy+ry)/2.f;
		Z=(fz+rz)/2.f;
		
		//rx = 2.f*x-fx;
		//rz = 2.f*z-fz;
		



		float RayLenght;
//		for (int i=0; i<100; i++)
	//	D3DXIntersect(Obj[2]->Mesh, &D3DXVECTOR3(FWheelX*cos(AngleY)+x,y+WheelRadius,FWheelX*sin(AngleY)-z), 
	//		&D3DXVECTOR3(0,-0.1f,0), &Hit, 0, 0, 0, 0, 0, 0);
		D3DXVECTOR3 vecDir;
		D3DXVec3Normalize(&vecDir, &D3DXVECTOR3(0,-1.f,0));

		D3DXIntersect(CurrentMap->Mesh, &D3DXVECTOR3(rx,ry+0.5f,rz), 
			&vecDir, &Hit, 0, 0, 0, &RayLenght, 0, 0);
		if (Hit && RayLenght < 0.5f)
		{
			ry += 0.5f - RayLenght + 0.001f;
		}
		else
		{
			D3DXIntersect(CurrentMap->Mesh, &D3DXVECTOR3(rx,ry,rz), 
				&vecDir, &Hit, 0, 0, 0, &RayLenght, 0, 0);
			if (Hit && RayLenght < GSpeed1)
			{
				GSpeed1=0.05f;
				if (RayLenght > 0.001f)
					ry -= RayLenght-0.001f;
			}
			else
			{
				ry -= GSpeed1;
				GSpeed1 += 0.032f;
			}
		}

		D3DXIntersect(CurrentMap->Mesh, &D3DXVECTOR3(fx,fy+0.5f,fz), 
			&vecDir, &Hit, 0, 0, 0, &RayLenght, 0, 0);
		if (Hit && RayLenght < 0.5f)
		{
			fy += 0.5f - RayLenght + 0.001f;
		}
		else
		{
			D3DXIntersect(CurrentMap->Mesh, &D3DXVECTOR3(fx,fy,fz), 
				&vecDir, &Hit, 0, 0, 0, &RayLenght, 0, 0);
			if (Hit && RayLenght < GSpeed2)
			{
				GSpeed2=0.05f;
				if (RayLenght > 0.001f)
					fy -= RayLenght-0.001f;
			}
			else
			{
				fy -= GSpeed2;
				GSpeed2 += 0.032f;
			}
		}
		
		Y=(fy+ry)/2.f;

		// розрахунок кута по висотам X
		AngleZ = -asin((fy-ry)/sqrt((fy-ry)*(fy-ry)+(fx-rx)*(fx-rx)+(fz-rz)*(fz-rz)));

		WheelOffset += Speed/50;
		
		WA_FLAG=TRUE;
		BREAK_FLAG=TRUE;

		// Проверка на проваливание в карту
		if (fy < -10.f || ry < -10.f)
		{
			new MESSAGE("Востановление объекта", 3);
			rx=0;
			ry=0;
			rz=0;
			fx=-WheelOffsetX;
			fy=0;
			fz=0;
			Speed = 0;
		}
		if (this == CurrentObject)
			sprintf_s(&STR_SPEED[7], 10, "%d", int(Speed));
	}
	void Control()
	{
		Camera[0]->Set(CurrentCamera->Type, AngleY, Speed);
		Camera[1]->Set(CurrentCamera->Type, AngleZ, AngleY, Speed);

		if (Key[DIK_UP]) {
			if (Speed < 100)
				Speed += 0.4f;
			BREAK_FLAG = FALSE;
		}
		if (Key[DIK_DOWN]) {
			if (Speed > -25)
				Speed -= 1.f;
			BREAK_FLAG = FALSE;
		}
		if (Key[DIK_LEFT]) {
			WA_FLAG = FALSE;
			if (WheelAngle > -0.7f)
				WheelAngle -= 0.02f;
		}
		if (Key[DIK_RIGHT]) {
			WA_FLAG = FALSE;
			if (WheelAngle < 0.7f)
				WheelAngle += 0.02f;
		}
		if (Key[DIK_SPACE]) Break=2.f;
		else Break=0;
		if (Key[DIK_C]) 
			if (!c) { 
				if (Camera[0] == CurrentCamera) CurrentCamera = Camera[1]; 
				else CurrentCamera = Camera[0];
				c = TRUE;
			}else;
		else c = FALSE;
		//тестовые кнопы, их можно стереть
		// end
		if (Key[DIK_Z]) {
			CurrentObject->MeshMaterials[0].Diffuse.r = rand()%100/100.f;
			CurrentObject->MeshMaterials[0].Diffuse.g = rand()%100/100.f;
			CurrentObject->MeshMaterials[0].Diffuse.b = rand()%100/100.f;
			CurrentObject->MeshMaterials[0].Ambient.r = rand()%100/100.f;
			CurrentObject->MeshMaterials[0].Ambient.g = rand()%100/100.f;
			CurrentObject->MeshMaterials[0].Ambient.b = rand()%100/100.f;
		}
	}
} *Car[50];
//==============================================================================
