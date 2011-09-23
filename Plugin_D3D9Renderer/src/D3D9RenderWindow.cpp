#include "D3D9RenderWindow.h"

namespace Titan
{
	D3D9RenderWindow::D3D9RenderWindow(HINSTANCE hInstance)
		:mhWnd(0), mhInstance(hInstance), mpD3dDevice(0)
	{
	}
	//-------------------------------------------------------------------------------//
	D3D9RenderWindow::~D3D9RenderWindow()
	{
		destroy();
	}
	//-------------------------------------------------------------------------------//
	void D3D9RenderWindow::destroy()
	{

	}
	//-------------------------------------------------------------------------------//
	void D3D9RenderWindow::create(const String& title, uint width, uint height, bool isFullScreen, const AnyMap* particularParams )
	{
		mWidth = width;
		mHeight = height;
		mWindowCaption = title;
		mIsWindowed = !isFullScreen;

		// Register the windows class
		WNDCLASS wndClass = 
		{ 
			CS_CLASSDC, MsgProc, 0, 0, (mhInstance),
			NULL, NULL, NULL, NULL, "Titan" 
		};
		RegisterClass(&wndClass);

		DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
			WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;

		// Create the render window
		mhWnd = CreateWindow( ("Titan"), mWindowCaption.c_str(), windowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			mWidth, mHeight , 0,
			0,
			0, 0 );
	}
	//-------------------------------------------------------------------------------//
	void D3D9RenderWindow::update()
	{
		RenderWindow::update();
	}
	//-------------------------------------------------------------------------------//
	void D3D9RenderWindow::swapBuffer()
	{
		HRESULT hr;
		if( FAILED( hr = mpD3dDevice->Present(0, 0, 0, 0)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( "Error in D3D9 Present function : " + msg);
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9RenderWindow::buildD3D9PrensentParameters(D3DPRESENT_PARAMETERS* params)
	{
		params->BackBufferWidth				= mWidth;
		params->BackBufferHeight				= mHeight;
		params->BackBufferFormat				= D3DFMT_A8R8G8B8;
		params->BackBufferCount				= 1;
		params->MultiSampleType				= D3DMULTISAMPLE_NONE;
		params->MultiSampleQuality			= 0;
		params->SwapEffect					= D3DSWAPEFFECT_DISCARD; 
		params->hDeviceWindow				= mhWnd;
		params->Windowed						= mIsWindowed;
		params->EnableAutoDepthStencil		= true; 
		params->AutoDepthStencilFormat		= D3DFMT_D24S8;
		//params->Flags						= 0;
		//params->FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
		params->PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	//-------------------------------------------------------------------------------//
	void D3D9RenderWindow::getCustomParams(const String& name, void* pDst)
	{
		if(name == "WINDOW")
		{
			HWND *pHwnd = (HWND*)pDst;
			*pHwnd = mhWnd;
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D9RenderWindow::setD3D9Device(LPDIRECT3DDEVICE9 pD3d9Device)
	{
		mpD3dDevice = pD3d9Device;
	}
	//-------------------------------------------------------------------------------//
	LRESULT D3D9RenderWindow::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
		{	// Store pointer to Win32Window in user data area
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));
			return 0;
		}

		switch (uMsg)
		{
		case WM_CLOSE:
		case WM_QUIT:
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
