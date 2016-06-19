#include "DXApp.h"

namespace
{
	//Create global pointer to a DXApp object
	//This will be used to forward messages from a global space to
	//our user define message procedure. This is necessary due to the fact that
	//we cant create a method defined as WNDPROC.
	DXApp* g_pApp = NULL;
}

//GLOBAL MESSAGE PROCEDURE
//This is used to forward messages to our used defined procedure function
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Forward messages
	if(g_pApp)
		return (g_pApp->MsgProc(hwnd, msg, wParam, lParam));
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

DXApp::DXApp(HINSTANCE hInstance)
{
	//Initialize members
	m_hAppInstance = hInstance;
	m_hAppWindow = NULL;
	m_AppTitle = "DIRECTX APPLICATION";
	m_ClientWidth = 800;
	m_ClientHeight = 600;
	m_EnableFullscreen = false; //not used yet anyway
	m_Paused = false; //application starts unpaused
	m_FPS = 0;
	m_WindowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX; //Standard non-resizeable window
	g_pApp = this; //Set our global pointer

	m_pDirect3D = 0;
	m_pDevice3D = 0;
	m_DevType = D3DDEVTYPE_HAL;
}

DXApp::~DXApp(void)
{
	//Release objects from memory
	SAFE_RELEASE(m_pDevice3D);
	SAFE_RELEASE(m_pDirect3D);
	SAFE_DELETE(g_pApp);
}

int DXApp::Run()
{
	//Main message loop (MAIN APPLICATION LOOP)
	//For every application that remains persistent on a system there needs to be a loop of some kind
	//As I explained further down, windows applications use a message based system. While your application
	//is still receiving message that don't equal WM_QUIT it will be running on the system.
	//Here is how your create such a loop:

	//To create a high performance timer you must use two methods
	//QueryPerformanceFrequency() and QueryPerformanceCounter()
	
	//QueryPerformanceFrequency():
	//This method stores the (COUNTS PER SECOND) that your system timer is running at
	//Using this we can cache a conversion to seconds per count
	__int64 countsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	float secPerCount = 1.0f / countsPerSec; //Conversion to seconds per count

	//QueryPerformanceCounter():
	//This method stores and image of the current count of the system timer
	//Using this we can find the count before and after we update to determine
	//the CHANGE IN ELAPSED TIME (Otherwise known as deltaTime)
	__int64 prevTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTime);

	//Create MSG struct
	MSG msg = {0}; //sets all members to null. (empty set)
	while(WM_QUIT != msg.message) //While our message doesn't equal WM_QUIT
	{
		//Peek at a message on the stack,
		//store it in msg,
		//and remove it from the stack
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg); //Translate message
			DispatchMessage(&msg); //Dispatch message
		}
		//Here is where things such as rendering, updating, etc. would go
		else
		{
			if(!m_Paused) //If application is not paused
			{
				if(!IsDeviceLost())
				{
					//Cache current time in counts
					__int64 curTime = 0;
					QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
					float dt = (curTime - prevTime) * secPerCount; //Calculate delta time
					//Calculate FPS
					CalculateFPS(dt);
					//Update
					Update(dt); //pass in 0 for now until later tutorial
					//Render
					Render();

					prevTime = curTime;
				}
			}
			else
			{
				Sleep(100); //Free up cpu for 1/10 of a second
			}
		}
	}

	//Now when the application finally finishes, we need to return
	//the error code given from our application
	return static_cast<int>(msg.wParam); //The error code is stored in the wParam member of our msg struct
}

bool DXApp::Init()
{
	//Initialize main window
	if(!InitMainWindow())
		return false;
	if(!InitDirect3D())
		return false;

	//If all succeeds return true
	return true;
}

bool DXApp::InitMainWindow()
{
	//First step:
	//Create a window class structure to define our window
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX)); //ZERO it out
	wcex.cbClsExtra = 0; //no extra bytes
	wcex.cbWndExtra = 0; //no extra bytes
	wcex.cbSize = sizeof(WNDCLASSEX); //set size in bytes
	wcex.style = CS_HREDRAW | CS_VREDRAW; //Basically states that window should be redraw both HORIZ. and VERT.
	wcex.hInstance = m_hAppInstance; //Set handle to application instance;
	wcex.lpfnWndProc = MainWndProc; //Set message procedure to our globally defined one
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); //Set window icon (standard application icon)
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); //Set window arrow (standard windows arrow)
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //Set clear background
	wcex.lpszClassName = "WIN32WINDOWCLASS"; //Name it w.e you like. 
	wcex.lpszMenuName = NULL; //We are not using a menu at this time.
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); //Set small window icon (standard application icon)

	//Now we must register the window class
	//Here is an example of some simple error checking
	//This can be quite useful for larger projects to debug errors
	if(!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Failed to register window class", NULL, NULL);
		return false;
	}

	//Second step:
	//Cache the correct window dimensions
	RECT r = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&r, m_WindowStyle, false); //Use our window style
	int width = r.right - r.left;  //correct width based on requested client size
	int height = r.bottom - r.top;  //correct height based on requested client size
	int x = GetSystemMetrics(SM_CXSCREEN)/2 - width/2; //Centers window on desktop
	int y = GetSystemMetrics(SM_CYSCREEN)/2 - height/2; //Centers window on desktop

	//Third step:
	//Create our window
	//lpClassName: MUST BE SAME AS ABOVE FROM WINDOW CLASS
	//lpWindowTitle: SHOULD BE m_AppTitle.c_str()
	m_hAppWindow = CreateWindow("WIN32WINDOWCLASS", m_AppTitle.c_str(), m_WindowStyle, x, y,
		width, height, NULL, NULL, m_hAppInstance, NULL);
	//Check window creation
	if(!m_hAppWindow)
	{
		MessageBox(NULL, "Failed to create window", NULL, NULL);
		return false;
	}

	//Fourth step:
	//Show window
	//SW_SHOW: Stand window display code, take the place of nCmdShow from entry point.
	ShowWindow(m_hAppWindow, SW_SHOW); 

	//If all succeeded return true
	return true;
}

bool DXApp::InitDirect3D()
{
	//There are a few steps to successfully intializing a Direct3D device object
	//The first is to actually create the Direct3D interface object
	
	//Step 1:
	m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_pDirect3D)
	{
		MessageBox(NULL, "Failed to create Direct3D interface object", NULL, NULL);
		return false;
	}

	//Step 2:
	//We must check the display mode. The display mode defines
	//the pixel format our application is rendering to and only certain
	//screens have certain pixel formats supported. This is why we must
	//check if the formats we want are ok.
	
	//Cache the adapter display mode into our member variable
	m_pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_Mode);
	//Check both our WINDOWED and FULLSCREEN formats
	HR(m_pDirect3D->CheckDeviceType(D3DADAPTER_DEFAULT, m_DevType, m_Mode.Format, m_Mode.Format, true)); //WINDOWED
	//NOTE: D3DFMT_X8R8G8B8 is a widely supported display format. This is what we will be using.
	HR(m_pDirect3D->CheckDeviceType(D3DADAPTER_DEFAULT, m_DevType, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false)); //FULLSCREEN
	
	//Step 3:
	//We must check to see if our graphics device supports hardware accelerated
	//vertex processing. Another term for this is HARDWARE TRANSFORM AND LIGHTING.
	//I'm going to assume virtually everyone following these tutorials has a device capable
	//of HWTRANSLIGHT
	int vp = 0;
	D3DCAPS9 devCaps;
	//Cache device caps
	HR(m_pDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, m_DevType, &devCaps));
	if(devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		//Our device supports transformations in hardware
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//Step 4:
	//We must initialize our present parameters structure
	//This will tell our device how it should render to the back buffer, how many
	//buffers it has, etc. It is basically a swap chain description.
	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	m_d3dpp.BackBufferWidth = m_ClientWidth;
	m_d3dpp.BackBufferHeight = m_ClientHeight;
	m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; //32 bit format
	m_d3dpp.Windowed = true; //start windowed
	m_d3dpp.BackBufferCount = 1; //Double buffered. 
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE; //No multisampling (way too intensive)
	m_d3dpp.MultiSampleQuality = 0;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow = m_hAppWindow;
	m_d3dpp.Flags = 0;
	m_d3dpp.EnableAutoDepthStencil =  true;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//Step 5:
	//Create our device
	HR(m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT,
		m_DevType, m_hAppWindow, vp, &m_d3dpp, &m_pDevice3D));

	D3DVIEWPORT9 viewport;
	ZeroMemory(&viewport, sizeof(D3DVIEWPORT9));
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = m_ClientWidth;
	viewport.Height = m_ClientHeight;
	viewport.MinZ = 0;
	viewport.MaxZ = 1;

	m_pDevice3D->SetViewport(&viewport);

	//If this all succeeds return true
	return true;
}

bool DXApp::IsDeviceLost()
{
	//Cache the state of the device
	HRESULT hr = m_pDevice3D->TestCooperativeLevel();
	if(hr == D3DERR_DEVICELOST) //If it is lost
	{
		//Sleep cpu for 1/10 of a second
		Sleep(100);
		return true;
	}
	else if(hr == D3DERR_DRIVERINTERNALERROR) //Fatal error occured
	{
		//Display message box
		MessageBox(NULL, "FATAL INTERNAL ERROR DETECTED.\n APPLICATION QUITTING", NULL, NULL);
		PostQuitMessage(0); //Quit application
		return true;
	}
	else if(hr == D3DERR_DEVICENOTRESET) //Device available for reset
	{
		//Destroy graphics
		OnLostDevice();

		//Reset the device
		HR(m_pDevice3D->Reset(&m_d3dpp));

		//Reset graphics
		OnResetDevice();

		//Device no longer lost
		return false;
	}
	else
		return false;
}

//Calculates frame per second
void DXApp::CalculateFPS(float dt)
{
	//Create static counters
	static int frameCnt;
	static float elapsedTime;

	//Increment counters
	frameCnt++;
	elapsedTime += dt;

	if(elapsedTime >= 1.0f)
	{
		m_FPS = (float)frameCnt;

		std::stringstream ss;
		ss << m_AppTitle.c_str() << "  FPS: " << m_FPS;
		SetWindowText(m_hAppWindow, ss.str().c_str());

		//Reset counters
		frameCnt = 0;
		elapsedTime = 0;
	}
}

//Enables fullscreen mode
//This entails resetting the window style, window position,
//and redefining the present parameters back buffer.
void DXApp::EnableFullscreen(bool enable)
{
	if(enable)
	{
		//Cache desktop width and height
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		m_d3dpp.BackBufferWidth = width;
		m_d3dpp.BackBufferHeight = height;
		m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_d3dpp.Windowed = false;
		
		//Set window style to fullscreen friendly
		SetWindowLongPtr(m_hAppWindow, GWL_STYLE, WS_POPUP);

		//Need to set new position for window
		SetWindowPos(m_hAppWindow, HWND_TOP, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	else
	{
		//Set window back to windowed mode
		RECT r = { 0, 0, m_ClientWidth, m_ClientHeight };
		AdjustWindowRect(&r, m_WindowStyle, false);
		int w = r.right - r.left;
		int h = r.bottom - r.top;
		m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		m_d3dpp.BackBufferWidth = m_ClientWidth;
		m_d3dpp.BackBufferHeight = m_ClientHeight;
		m_d3dpp.Windowed = true;

		//Change window style back to windowed friendly
		SetWindowLongPtr(m_hAppWindow, GWL_STYLE, m_WindowStyle);

		//Set window position
		SetWindowPos(m_hAppWindow, HWND_TOP,
			GetSystemMetrics(SM_CXSCREEN)/2 - w/2,
			GetSystemMetrics(SM_CYSCREEN)/2 - h/2,
			w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	//Reset our device to reflect the changes
	OnLostDevice();
	HR(m_pDevice3D->Reset(&m_d3dpp));
	OnResetDevice();
}

//Message procedure function.
//Windows OS runs of a message based system where all application
//that run on the OS send and receive messages that tell it what to do. To receive
//these messages our application needs what is called a message procedure function.
//Using this function we can "catch" these messages and tell out application what to do.
LRESULT DXApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Switch statement on the message passed to us
	switch(msg)
	{
		//CASE: WM_DESTROY, our application is told to destroy itself
	case WM_DESTROY:
		PostQuitMessage(0); //Tell the application to quit
		return 0;

		//CASE: WM_ACTIVATE, our application is the active window
	case WM_ACTIVATE:
		//CHECK IF LOW ORDER BIT = INACTIVE
		if(LOWORD(wParam) == WA_INACTIVE)
			//Application should pause itself
			m_Paused = true;
		else //Otherwise, the application is not paused
			m_Paused = false;
		return 0;

		//CASE: WM_KEYDOWN, user pressed a key on keyboard
	case WM_KEYDOWN:
		//Switch on the WPARAM, (which stores the keycode)
		switch(wParam)
		{
			//CASE: VK_ESCAPE, user pressed the escape key
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;

		case VK_F1:
			m_EnableFullscreen = !m_EnableFullscreen;
			EnableFullscreen(m_EnableFullscreen);
			return 0;
		}
		return 0;
	}

	//Always return the default window procedure if we don't catch anything
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
