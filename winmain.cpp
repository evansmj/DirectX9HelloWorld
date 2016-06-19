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

	//Else return true
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
	m_pDevice3D->Clear(0, 0, D3DCLEAR_TARGET, d3dColors::CORN_FLOWER_BLUE, 1.0f, 0);



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