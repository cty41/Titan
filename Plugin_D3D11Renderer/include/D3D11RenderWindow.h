#ifndef __D3D11_RENDERWINDOW_HH
#define __D3D11_RENDERWINDOW_HH

#include "D3D11Prerequisites.h"
#include "TiRenderWindow.h"

namespace Titan
{
	class _D3D11DllExport D3D11RenderWindow : public RenderWindow
	{
	public:
		D3D11RenderWindow(HINSTANCE Ins);

		~D3D11RenderWindow();

		virtual void		create(const String& title, uint width, uint height, bool isFullScreen, const AnyMap* particularParams /* = 0 */);

		virtual void		destroy();

		virtual void		update();

		virtual void		swapBuffer();

		void				setD3D11Descs(DXGI_SWAP_CHAIN_DESC* descs);

		void				getCustomParams(const String& name, void* pDst);

		void				setSwapChain(IDXGISwapChain* swapChain);

		IDXGISwapChain*		getSwapChain() const { return mSwapChain; }
	
		static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HWND				mhWnd;
		HINSTANCE			mhInstance;
		IDXGISwapChain*		mSwapChain;
	};
}

#endif