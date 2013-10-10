// include the basic windows header file
#include <windows.h>
#include <windowsx.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
//#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx11.h"
//#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\D3DX10.h"


// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
//#pragma comment (lib, "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86\d3dx11.lib")
//#pragma comment (lib, "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86\d3dx10.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer

ID3D11VertexShader *pVS;    // the vertex shader
ID3D11PixelShader *pPS;     // the pixel shader
ID3D11Buffer *pVBuffer; 
ID3D11InputLayout *pLayout;

// function prototypes
void InitD3D(HWND hWnd);     // sets up and initializes Direct3D
void RenderFrame(void);     // renders a single frame
void CleanD3D(void);         // closes Direct3D and releases memory
void InitGraphics(void);
void InitPipeline(void);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct VERTEX
{
      FLOAT X, Y, Z;      // position
      D3DXCOLOR Color;    // color
};