#include <windows.h>
#include <windowsx.h>
#include "WICTextureLoader.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// global declarations
IDXGISwapChain			*comSwapChain;             // the pointer to the swap chain interface
ID3D11Device			*comDevice;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext		*comDeviceContext;           // the pointer to our Direct3D device context
ID3D11RenderTargetView	*comBackBuffer;			// the pointer to our back buffer

ID3D11VertexShader		*comVertexShader;    // the vertex shader
ID3D11PixelShader		*comPixelShader;     // the pixel shader
ID3D11Buffer			*comVertexBuffer; 
ID3D11Buffer			*comIndexBuffer; 
ID3D11InputLayout		*comInputLayout;

ID3D11Resource				*comTexture = NULL;
ID3D11ShaderResourceView	*comTextureShaderView = NULL; // Объект текстуры
ID3D11SamplerState			*comSamplerState;

// function prototypes
void InitD3D(HWND hWnd);     // sets up and initializes Direct3D
void RenderFrame(void);     // renders a single frame
void CleanD3D(void);         // closes Direct3D and releases memory
void InitPipeline(void);
void InitBuffers(void);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/*
struct VERTEX_COLOR
{
      //FLOAT X, Y, Z;      // position
	D3DXVECTOR3 position;
    D3DXCOLOR Color;    // color
	//D3DXVECTOR2 texture;
};*/

struct VERTEX_TEXTURE
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
};

const int vertexCount = 3, indexCount = 3;