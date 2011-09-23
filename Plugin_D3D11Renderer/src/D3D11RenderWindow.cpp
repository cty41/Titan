#include "D3D11RenderWindow.h"

namespace Titan
{
	D3D11RenderWindow::D3D11RenderWindow(HINSTANCE hInstance)
		:mhWnd(0), mhInstance(hInstance), mSwapChain(nullptr)
	{
	}
	//-------------------------------------------------------------------------------//
	D3D11RenderWindow::~D3D11RenderWindow()
	{
		destroy();
	}
	//-------------------------------------------------------------------------------//
	void D3D11RenderWindow::destroy()
	{

	}
	//-------------------------------------------------------------------------------//
	void D3D11RenderWindow::create(const String& title, uint width, uint height, bool isFullScreen, const AnyMap* particularParams )
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
	void D3D11RenderWindow::update()
	{
		RenderWindow::update();
	}
	//-------------------------------------------------------------------------------//
	void D3D11RenderWindow::swapBuffer()
	{
		HRESULT hr;
		if( FAILED( hr = mSwapChain->Present(0, 0)))
		{
			String msg = DXGetErrorDescription(hr);
			TITAN_EXCEPT_API( "Error in D3D9 Present function : " + msg);
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D11RenderWindow::setD3D11Descs(DXGI_SWAP_CHAIN_DESC* descs)
	{
		ZeroMemory(descs, sizeof(DXGI_SWAP_CHAIN_DESC));

		descs->BufferCount = 1;
		descs->BufferDesc.Width = mWidth;
		descs->BufferDesc.Height = mHeight;
		descs->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		descs->BufferDesc.RefreshRate.Numerator = 60;
		descs->BufferDesc.RefreshRate.Denominator = 1;
		descs->OutputWindow = mhWnd;
		descs->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		descs->SampleDesc.Count = 1;
		descs->SampleDesc.Quality = 0;
		descs->Windowed = mIsWindowed;
	}
	//-------------------------------------------------------------------------------//
	void D3D11RenderWindow::getCustomParams(const String& name, void* pDst)
	{
		if(name == "WINDOW")
		{
			HWND *pHwnd = (HWND*)pDst;
			*pHwnd = mhWnd;
		}
	}
	//-------------------------------------------------------------------------------//
	void D3D11RenderWindow::setSwapChain(IDXGISwapChain* sc)
	{
		mSwapChain = sc;
	}
	//-------------------------------------------------------------------------------//
	LRESULT D3D11RenderWindow::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
