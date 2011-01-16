#ifndef __TITAN_D3D9RENDERWINDOW_HH
#define __TITAN_D3D9RENDERWINDOW_HH

#include "D3D9Prerequisites.h"
#include "RenderWindow.h"

namespace Titan
{
	class _D3D9DllExport D3D9RenderWindow : public RenderWindow
	{
	public:
		D3D9RenderWindow(HINSTANCE hInstance);

		~D3D9RenderWindow();

		virtual void		create(const String& title, uint width, uint height, bool isFullScreen, const AnyMap* particularParams /* = 0 */);

		virtual void		destroy();

		virtual void		update();

		virtual void		swapBuffer();

		void				buildD3D9PrensentParameters(D3DPRESENT_PARAMETERS* params);

		void				getCustomParams(const String& name, void* pDst);

		void				setD3D9Device(LPDIRECT3DDEVICE9 pD3d9Device);

		LPDIRECT3DDEVICE9	getD3D9Device() const { return mpD3dDevice; }
	
		static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HWND				mhWnd;
		HINSTANCE			mhInstance;
		LPDIRECT3DDEVICE9	mpD3dDevice;
	};
}

#endif