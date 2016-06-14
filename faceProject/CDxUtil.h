#ifndef _CDXUTIL_H
#define _CDXUTIL_H
#define DIRECTINPUT_VERSION 0x0800
#include <d3dx9.h>
#include <dinput.h>
#include <Windows.h>
#include <vector>
using namespace std;
class CDXUTIL{
public:
	CDXUTIL(int windowWidth=800,int windowHeight=600);
	void createFaceBuffer(vector<float> asm_point,short* asm_index,int numPrim,int texWidth,int texHeight,char* texture_add=NULL);
	void go();
	~CDXUTIL();
private:
	void initRenderState();
	void beginScene();
	void endScene();
	void drawPrim();
	void readInput();
private:
	struct CUSTOMVERTEX{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
		D3DXVECTOR2 texture;
	};
private:
	//windows parameter
	int m_windowWidth;
	int m_windowHeight;

	//dx resources
	IDirect3DDevice9 *m_device;
	IDirect3DVertexBuffer9 *m_vb;
	IDirect3DIndexBuffer9 *m_ib;
	IDirect3DTexture9 *m_texture;
	LPDIRECTINPUT8 m_directInput;
	LPDIRECTINPUTDEVICE8 m_mouseDevice;
	LPDIRECTINPUTDEVICE8 m_keyboardDevice;
	LPD3DXEFFECT m_effect;

	//input data
	char m_pKeyStateBuffer[256];
	DIMOUSESTATE m_diMouseState;

	//view parameter
	D3DXVECTOR3 vEyePt;
	D3DXVECTOR3 vLookatPt;
	D3DXVECTOR3 vUpVec;

	//matrix
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;

	//shader handle
	D3DXHANDLE hWorld;
	D3DXHANDLE hWVP;
	D3DXHANDLE hTex;
	D3DXHANDLE hEye;
	D3DXHANDLE hLight1;

	//other
	float speed;
	int numVertex;
	int numPrim;
	float angle;
};

#endif