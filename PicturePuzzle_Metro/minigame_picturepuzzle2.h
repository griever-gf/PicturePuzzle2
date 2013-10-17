#include "pch.h"

#include "minigameinterface.h"
#include "BasicReaderWriter.h"
#include <fstream>
//#include <iostream>
//#include <sstream>

#include <D3Dcompiler.h>
//#pragma comment(lib, "D3Dcompiler.lib")

#pragma once

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;

struct VERTEX_TEXTURE
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
};

struct VERTEX
{
	XMFLOAT3 position;
};

extern ComPtr<ID3D11DeviceContext1>		comDeviceContext;    // the device context interface
extern ComPtr<ID3D11Buffer>				comVertexBuffer;
extern ComPtr<ID3D11VertexShader>		comVertexShader;
extern ComPtr<ID3D11PixelShader>		comPixelShader;
extern ComPtr<ID3D11InputLayout>		comInputLayout;    // the input layout interface

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
	ComPtr<ID3D11RenderTargetView> comRendertarget;    
	//IDXGISwapChain				*comSwapChain;             // the pointer to the swap chain interface
	//ID3D11Device				*comDevice; 
};