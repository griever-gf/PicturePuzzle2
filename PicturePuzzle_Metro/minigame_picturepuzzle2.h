#include "pch.h"

#include "minigameinterface.h"
#pragma once

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;

extern ComPtr<ID3D11DeviceContext1> comDeviceContext;    // the device context interface

class MiniGamePicturePuzzle : public MiniGame
{
public:
    MiniGamePicturePuzzle();
    ~MiniGamePicturePuzzle();
	void Initialize();	
	void Click(float x, float y);
	bool IsComplete() const;
	void Render() const;

	HWND hWnd;
private:
	void InitPipeline(void);
	void InitBuffers(void);
	//ComPtr<ID3D11Device1> comDevice;              // the device interface
	ComPtr<ID3D11Device1> comDevice;
	ComPtr<IDXGISwapChain1> comSwapChain;
	//IDXGISwapChain				*comSwapChain;             // the pointer to the swap chain interface
	//ID3D11Device				*comDevice; 
};