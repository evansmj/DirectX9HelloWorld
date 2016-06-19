/* Title: DirectX 9.0c Tutorial 03
/* Date: June 09, 2013
/* Description: Wrapper class for a Direct3D Application
/* Youtube: http://www.youtube.com/user/Direct3DTutorials
/* Terms of Use: Free to be used in any project
/************************************************************************/

#pragma once //TAKES PLACE OF (#ifndef guards)

#include "d3dUtil.h"

//Abstract application class
class DXApp
{
public:
	//Constructor
	DXApp(HINSTANCE hInstance);
	//Destructor
	virtual ~DXApp(void);

	//Main application loop
	int Run();

	//Framework methods
	virtual bool Init();
	virtual void Update(float dt) = 0; //pure virtual, aka MUST be overridden by inheriting class
	virtual void Render() = 0; //pure virtual, aka MUST be overridden by inheriting class
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); //message procedure function

	virtual void OnLostDevice() = 0;  //Handle lost graphics
	virtual void OnResetDevice() = 0; //Handle reset graphics

protected:
	//Members

	HWND			m_hAppWindow;				//HANDLE to application window
	HINSTANCE	m_hAppInstance;			//HANDLE to application instance
	UINT			m_ClientWidth;			//Requested client width
	UINT			m_ClientHeight;			//Requested client height
	std::string	m_AppTitle;				//Application title (window title bar)
	DWORD		m_WindowStyle;			//Window style (e.g. WS_OVERLAPPEDWINDOW)
	bool			m_Paused;				//True if application pause, false otherwise
	bool			m_EnableFullscreen;		//True to enable fullscreen, false otherwise
	float		m_FPS;					//Frames per second of our application

	//DirectX members
	IDirect3D9*				m_pDirect3D;			//Direct3D interface
	IDirect3DDevice9*			m_pDevice3D;			//Direct3D device interface
	D3DPRESENT_PARAMETERS		m_d3dpp;				//Direct3D present parameters struct
	D3DDISPLAYMODE			m_Mode;				//Direct3D display mode struct
	D3DDEVTYPE				m_DevType;			//Device Type (SHOULD BE DEVTYPE_HAL)

	
protected:
	//Methods

	//Initializes main application window
	bool InitMainWindow();
	//Initialize direct3D
	bool InitDirect3D();
	//Handles lost device
	bool IsDeviceLost();
	//Calculates FPS
	void CalculateFPS(float dt);
	//Enables fullscreen
	void EnableFullscreen(bool enable);
};
