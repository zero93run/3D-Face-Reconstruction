#include "CDxUtil.h"

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam){
							 switch(msg){
							 case WM_DESTROY:
								 ::PostQuitMessage(0);
								 break;
							 }
							 return ::DefWindowProc(hWnd,msg,wParam,lParam);
}

bool Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize)   
{  
	HRESULT hr;  
	while (true)   
	{  
		pDIDevice->Poll();
		pDIDevice->Acquire();
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;  
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;  
		if (FAILED(pDIDevice->Acquire())) return FALSE;  
	}  
	return TRUE;  
}  

CDXUTIL::CDXUTIL(int windowWidth,int windowHeight){

	m_windowWidth=windowWidth;
	m_windowHeight=windowHeight;

	m_device=NULL;
	m_texture=NULL;
	m_vb=NULL;
	m_ib=NULL;
	m_directInput=NULL;
	m_mouseDevice=NULL;
	m_keyboardDevice=NULL;
	speed=0.01f;
	angle=0.0f;
	//get HWND
	HINSTANCE hInstance=GetModuleHandle(0);
	WNDCLASS wc;
	wc.style	= CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc	=WndProc;
	wc.cbClsExtra	=0;
	wc.cbWndExtra	=0;
	wc.hInstance	=hInstance;
	wc.hIcon	=::LoadIcon(0,IDI_APPLICATION);
	wc.hCursor	=::LoadCursor(0,IDC_ARROW);
	wc.hbrBackground	=static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName	=0;
	wc.lpszClassName	=TEXT("D3DApp");
	if(!RegisterClass(&wc)){
		MessageBoxA(0,"RegisterClass - Failed",0,0);
		return;
	}
	HWND hwnd = CreateWindowA(
		"D3DApp",
		"face model",
		WS_DLGFRAME|WS_SYSMENU,//WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowWidth,
		windowHeight,
		0,
		0,
		hInstance,
		0);
	if(hwnd == 0){
		MessageBoxA(0,"CreateWindow - Failed",0,0);
		return;
	}
	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	HRESULT hr = 0;
	IDirect3D9 *d3d9;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d9)
	{
		MessageBoxA(0,"Direct3DCreate9 - Failed",0,0);
		return;
	}
	D3DCAPS9 caps9;
	if( FAILED( d3d9->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps9 ) ) )  
	{  
		return;  
	}  

	int vp;
	if(caps9.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = windowWidth;
	d3dpp.BackBufferHeight = windowHeight;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,vp,&d3dpp,&m_device);
	if(FAILED(hr))
	{
		d3d9->Release();
		MessageBoxA(0,"CreateDevice - Failed",0,0);
		return;
	}
	d3d9->Release();

	hr=DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);  	
	if(FAILED(hr))
	{
		MessageBoxA(0,"CreateInput - Failed",0,0);
		return;
	}
	m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboardDevice, NULL);  
	m_keyboardDevice->SetDataFormat(&c_dfDIKeyboard);  
	m_keyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);  
	m_keyboardDevice->Acquire();  

	m_directInput->CreateDevice(GUID_SysMouse, &m_mouseDevice, NULL);  
	m_mouseDevice->SetDataFormat(&c_dfDIMouse);  
	m_mouseDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
	m_mouseDevice->Acquire();  

	ID3DXBuffer *errorBuffer=0;
	hr=D3DXCreateEffectFromFile(m_device,"./shader/pastis_shader.fx",NULL,NULL,
		D3DXSHADER_DEBUG,NULL,&m_effect,&errorBuffer);
	if(errorBuffer){
		MessageBoxA(0,(char*)errorBuffer->GetBufferPointer(),0,0);
		return;
	}
	if(FAILED(hr)){
		MessageBoxA(0,"CreateShader - Failed",0,0);
		return;
	}

	initRenderState();
}

void CDXUTIL::initRenderState(){

	// Turn off D3D lighting
	m_device->SetRenderState( D3DRS_LIGHTING, FALSE );

	D3DVERTEXELEMENT9 decl[]={
		{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
		{0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0},
		{0,24,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
		D3DDECL_END()
	};
	IDirect3DVertexDeclaration9 * _decl;
	m_device->CreateVertexDeclaration(decl,&_decl);
	m_device->SetVertexDeclaration(_decl);

	D3DXMatrixIdentity( &matWorld );
	//m_device->SetTransform( D3DTS_WORLD, &matWorld );

	vEyePt=D3DXVECTOR3( 0.0f, 0.0f,-1000.0f );
	vLookatPt=D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	vUpVec=D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	//m_device->SetTransform( D3DTS_VIEW, &matView );

	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, float(m_windowWidth)/float(m_windowHeight), 1.0f, 10000.0f );
	//m_device->SetTransform( D3DTS_PROJECTION, &matProj );

	HANDLE hr;
	//get handle
	hWorld = m_effect->GetParameterByName(0,"g_mWorld");
	hWVP = m_effect->GetParameterByName(0,"g_mWorldViewProj");
	hTex = m_effect->GetParameterByName(0,"g_txDiffuse");
	hEye = m_effect->GetParameterByName(0,"g_vEyePos");
	hLight1 = m_effect->GetParameterByName(0,"g_vLightDir1");

	D3DXVECTOR4 eyePt=D3DXVECTOR4(vEyePt,1.0);
	m_effect->SetVector(hEye,&eyePt);

	D3DXVECTOR4 vLightDir1=D3DXVECTOR4(0,0,-1,0);
	m_effect->SetVector(hLight1,&vLightDir1);

	m_effect->SetMatrix(hWorld,&matWorld);
	D3DXMATRIX tmpMat=matWorld*matView*matProj;
	m_effect->SetMatrix(hWVP,&tmpMat);

	//set technique
	D3DXHANDLE hTech = m_effect->GetTechniqueByName("T0");
	m_effect->SetTechnique(hTech);

}

void CDXUTIL::beginScene(){
	m_device->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);
	m_device->BeginScene();
}

void CDXUTIL::endScene(){
	m_device->EndScene();
	m_device->Present(0,0,0,0);
}

void CDXUTIL::go(){
	MSG msg = { 0 }; 
	static float lastTime =(float)GetTickCount();
	float globalTime=0;
	long timeCount=0; 
	while( msg.message != WM_QUIT )
	{  
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{  
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}  
		else  
		{  
			readInput();
			beginScene();
			drawPrim();
			endScene();
		}
	}
}

void CDXUTIL::drawPrim(){

	UINT numPasses=0;
	m_effect->Begin(&numPasses,0);

	for(int i=0;i<numPasses;++i){
		m_effect->BeginPass(i);
		m_device->SetStreamSource(0,m_vb,0,sizeof(CUSTOMVERTEX));
		m_device->SetIndices(m_ib);
		m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,numVertex,0,numPrim);
		//m_device->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,1);
		m_effect->EndPass();
	}
	m_effect->End();
}

CDXUTIL::~CDXUTIL(){
	if(m_device!=NULL)
		m_device->Release();
	if(m_texture!=NULL)
		m_texture->Release();
	if(m_vb!=NULL)
		m_vb->Release();
	if(m_ib!=NULL)
		m_ib->Release();
	if(m_directInput!=NULL)
		m_directInput->Release();
	if(m_mouseDevice!=NULL)
		m_mouseDevice->Release();
	if(m_keyboardDevice!=NULL)
		m_keyboardDevice->Release();
	if(m_effect!=NULL)
		m_effect->Release();
}

void CDXUTIL::readInput(){
	ZeroMemory(&m_diMouseState,sizeof(m_diMouseState));
	Device_Read(m_mouseDevice,(LPVOID)&m_diMouseState,sizeof(m_diMouseState));
	vEyePt.z+=m_diMouseState.lZ*speed*10;
	vEyePt.z=vEyePt.z>-10?-10:vEyePt.z;
	if(m_diMouseState.rgbButtons[0] & 0x80)
	{
		angle-=m_diMouseState.lX*speed;
		angle=angle>(D3DX_PI/2.1)?(D3DX_PI/2.1):angle;
		angle=angle<-(D3DX_PI/2.1)?-(D3DX_PI/2.1):angle;
		D3DXMatrixRotationY(&matWorld,angle);
		m_device->SetTransform( D3DTS_WORLD, &matWorld );
		//D3DXMatrixRotationY(&tempMat,-m_diMouseState.lX*speed);
		//matWorld*=tempMat;
		//m_device->SetTransform( D3DTS_WORLD, &matWorld );

	}
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	m_device->SetTransform( D3DTS_VIEW, &matView );

	m_effect->SetMatrix(hWorld,&matWorld);
	D3DXMATRIX tmpMat=matWorld*matView*matProj;
	m_effect->SetMatrix(hWVP,&tmpMat);
}

void CDXUTIL::createFaceBuffer(vector<float> asm_point,short* asm_index,int numPrim,int texWidth,int texHeight,char* texture_add){
	if(m_vb!=NULL)
		m_vb->Release();
	numVertex=asm_point.size()/3;
	DWORD FVF;
	m_device->GetFVF(&FVF);

	CUSTOMVERTEX* points=new CUSTOMVERTEX[numVertex];
	for(int i=0;i<numVertex;++i){
		points[i].position=D3DXVECTOR3(asm_point[3*i],asm_point[3*i+1],asm_point[3*i+2]);
		points[i].normal=D3DXVECTOR3(0.0,0.0,0.0);
		points[i].texture.x=asm_point[3*i]/texWidth+0.5;
		points[i].texture.y=0.5-asm_point[3*i+1]/texHeight;
	}

	for(int i=0;i<numPrim;++i){
		D3DXVECTOR3 line1=points[int(asm_index[3*i])].position - points[int(asm_index[3*i+1])].position;
		D3DXVECTOR3 line2=points[int(asm_index[3*i])].position - points[int(asm_index[3*i+2])].position;
		D3DXVECTOR3 norm;
		D3DXVec3Cross(&norm,&line1,&line2);
		points[int(asm_index[3*i])].normal+=norm;
		points[int(asm_index[3*i+1])].normal+=norm;
		points[int(asm_index[3*i+2])].normal+=norm;
	}

	for(int i=0;i<numVertex;++i){
		D3DXVec3Normalize(&(points[i].normal),&(points[i].normal));
	}

	m_device->CreateVertexBuffer(numVertex*sizeof(CDXUTIL::CUSTOMVERTEX),0,FVF,D3DPOOL_DEFAULT,&m_vb,NULL);
	CUSTOMVERTEX* cvs;
	m_vb->Lock(0,0,(void**)&cvs,0);
	memcpy(cvs,points,numVertex*sizeof(CDXUTIL::CUSTOMVERTEX));
	m_vb->Unlock();
	delete[] points;

	this->numPrim=numPrim;
	void* pVoid;
	m_device->CreateIndexBuffer(sizeof(short)*numPrim*3,0,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_ib,NULL);
	m_ib->Lock(0,0,(void**)&pVoid,0);
	memcpy(pVoid,asm_index,sizeof(short)*numPrim*3);
	m_ib->Unlock();

	if(texture_add!=NULL){
		D3DXCreateTextureFromFileA(m_device,texture_add,&m_texture);
	}

	m_effect->SetTexture(hTex,m_texture);

	//m_device->SetTexture( 0, m_texture );
	//m_device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	//m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	//m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	//m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
}
