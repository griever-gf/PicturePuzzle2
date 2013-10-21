#include "pch.h"

#include "minigameinterface.h"
#include "WICTextureLoader.h" //image to texture converting (because D3DX11CreateShaderResourceViewFromFile is deprecated for Windows Store:( )
#include "BasicReaderWriter.h" //compiled shaders (*.cso) loading
#include <fstream>

#pragma once

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;


struct VERTEX_TEXTURE
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
};

extern ComPtr<ID3D11DeviceContext1>		comDeviceContext;    // the device context interface
extern ComPtr<ID3D11RenderTargetView>	comBackBuffer;
extern ComPtr<ID3D11Buffer>				comVertexBuffer;
extern ComPtr<ID3D11Buffer>				comIndexBuffer;
extern ComPtr<ID3D11VertexShader>		comVertexShader;
extern ComPtr<ID3D11PixelShader>		comPixelShader;
extern ComPtr<ID3D11InputLayout>		comInputLayout;    // the input layout interface
extern ComPtr<ID3D11SamplerState>		comSamplerState;
static const int texturesNum			= 4;
extern ComPtr<ID3D11ShaderResourceView>	comShaderViews[texturesNum];


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
	ComPtr<ID3D11Device1> comDevice;
	ComPtr<IDXGISwapChain1> comSwapChain;
};