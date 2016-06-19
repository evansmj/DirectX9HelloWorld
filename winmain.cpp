/************************************************************************/
/* Author: Matt Guerrette
/* Title: DirectX 9.0c Tutorial 03
/* Date: June 09, 2013
/* Description: This tutorial shows how to initialize DirectX using our application,
			   The basis of all DirectX and Windows programming stems from knowledge
			   of the Win32 API. This should be a good primer for you
/* Youtube: http://www.youtube.com/user/Direct3DTutorials
/* Terms of Use: Free to be used in any project
/************************************************************************/

//Include our D3DApp wrapper class
#include "DXApp.h"

struct VertexPositionColor
{
	VertexPositionColor() {}
	VertexPositionColor(float _x, float _y, float _z, D3DCOLOR c)
	{
		x = _x;
		y = _y;
		z = _z;
		color = c;
	}

	float x, y, z;
	D3DCOLOR color;
	static const DWORD FVF; // flexible vretex format;
};

const DWORD VertexPositionColor::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE; //diffuse means color

class TestApp : public DXApp
{
public:
	//Constructor
	TestApp(HINSTANCE hInstance);
	//Destructor
	~TestApp();
	
	//Methods
	bool Init() override;
	void Update(float dt) override;
	void Render() override;
	void OnLostDevice() override;
	void OnResetDevice() override;
};

IDirect3DVertexBuffer9 * VB; //gpu reads vertices after binded here
IDirect3DIndexBuffer9 * IB; //tells the order to read them^

//Calls the base class (DXApp) constructor
TestApp::TestApp(HINSTANCE hInstance):DXApp(hInstance)
{

}

//Destructor
TestApp::~TestApp()
{

}

//Calls the based class (DXApp) Init()
bool TestApp::Init()
{
	//If it fails return false;
	if(!DXApp::Init())
		return false;

	VertexPositionColor verts[3] =
	{
		VertexPositionColor(0.0f, 0.0f, 0.0f, d3dColors::LIME),
		VertexPositionColor(1.0f, -1.0f, 0.0f, d3dColors::BLUE),
		VertexPositionColor(-1.0f, -1.0f, 0.0f, d3dColors::RED)
	};

	//now create vertex buffer

	m_pDevice3D->CreateVertexBuffer(3 * sizeof(VertexPositionColor), 0, VertexPositionColor::FVF, D3DPOOL_MANAGED,
		&VB, NULL);

	//how do you manage the vb?  how to add stuff to it?

	VOID * pVerts;

	//have to lock the vb before doing anything
	VB->Lock(0, sizeof(verts), (void**)&pVerts, 0);
	memcpy(pVerts, verts, sizeof(verts));
	VB->Unlock();

	//now all 3 vertices are set to the vertex buffer

	//now set up the camera, since we cant just render without knowing where to view from

	D3DXMATRIX view;
	D3DXMATRIX proj;

	//set view
	D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&view, &position, &target, &up);

	m_pDevice3D->SetTransform(D3DTS_VIEW, &view);

	//set projection
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, static_cast<float>(m_ClientWidth)/m_ClientHeight, 1.0f, 1000.0f);
	m_pDevice3D->SetTransform(D3DTS_PROJECTION, &proj);

	//projecection matrix defines how camera view the world, fov 180 degrees etc

	//view matrix is the orientation of that ^ view.  what change sbased on rotation etc.  where up is

	m_pDevice3D->SetRenderState(D3DRS_LIGHTING, false);
	m_pDevice3D->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	return true;
}

//Update test app
void TestApp::Update(float dt)
{

}


//Render test app
void TestApp::Render()
{
	//D3DCOLOR: Cornflower Blue = RGB(100, 149, 237)
	//Clears the back buffer
	m_pDevice3D->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, d3dColors::CORN_FLOWER_BLUE, 1.0f, 0);

	//need to call begin scene and end scene before rendering
	m_pDevice3D->BeginScene();
	m_pDevice3D->SetStreamSource(0, VB, 0, sizeof(VertexPositionColor));
	m_pDevice3D->SetFVF(VertexPositionColor::FVF);
	m_pDevice3D->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	m_pDevice3D->EndScene();

	//Present the backbuffer to our window
	m_pDevice3D->Present(0, 0, 0, 0);
}

void TestApp::OnResetDevice()
{

}

void TestApp::OnLostDevice()
{

}

//Application Entry point

//HINSTANCE hInstance: Basically the handle to the instance of your application.
//HINSTANCE hPrevInstance: DEPRECATED, in other words not used anymore. Ignore it.
//LPSTR lpCmdLine: Basically the input to the command line. (Java and C# equivalent =  string[] args)
//int nCmdShow: Basically defines how the window is first shown, you will NOT be using it. 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Create instance of test app object
	TestApp* tApp = new TestApp(hInstance);

	//Initialize our test app
	if(!tApp->Init())
		return 1; //exit application

	//Otherwise, call our application loop
	return (tApp->Run());
}