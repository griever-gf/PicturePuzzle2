#if defined(__cplusplus_winrt)
	#include <wrl/client.h>
	#include <d3d11_1.h>
	#include <DirectXMath.h>
	#include <memory>
	#include <ppltasks.h> //async tasks, for file open picker
	#include "BasicReaderWriter.h" //compiled shaders (*.cso) loading
	#include "WICTextureLoader.h" //image to texture converting (because D3DX11CreateShaderResourceViewFromFile is deprecated for Windows Store:( )
	#pragma once
	using namespace Microsoft::WRL;
	using namespace Windows::UI::Core;
	using namespace DirectX;
	using namespace Windows::Storage;
	using namespace Windows::Storage::Streams;
	using namespace Windows::Storage::Pickers;
	using namespace concurrency; //Include the namespace for the parallel processing library (PPL)
	typedef XMFLOAT3 VECTOR_F3;
	typedef XMFLOAT2 VECTOR_F2;
	typedef XMFLOAT4 VECTOR_F4;
#else
	#include <windows.h>
	#include <windowsx.h>
	#include <Shobjidl.h>
	#include <d3d11.h>
	#include <d3dx11.h>
	#include <d3dx10.h>
	// include the Direct3D Library file
	#pragma comment (lib, "d3d11.lib")
	#pragma comment (lib, "d3dx11.lib")
	#pragma comment (lib, "d3dx10.lib")
	// define the screen resolution for desktop window
	typedef D3DXVECTOR3 VECTOR_F3;
	typedef D3DXVECTOR2 VECTOR_F2;
	typedef D3DXCOLOR	VECTOR_F4;
#endif

#include "minigameinterface.h"

#include <sstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>       /* fmod */

struct VERTEX_TEXTURE
{
	VECTOR_F3 position;
	VECTOR_F2 texture;
};

struct VERTEX_COLOR
{
	VECTOR_F3 position;
	VECTOR_F4 color;
};

//эти переменные глобальные только потому, что требуется вызов глобальной функции Render из класса MiniGamePicturePuzzle,
//а ей нужно передавать параметры, а они ограничены шаблоном из текста тестового задания...
static const int				vertexCount = MiniGame::cRows*MiniGame::cColumns*4; //may be  (cRows+1)*(cColumns+1);
static const int				indexCount = MiniGame::cRows*MiniGame::cColumns*6;
extern VERTEX_TEXTURE			RectangleVertices[vertexCount];
extern VECTOR_F2				StandardTextCoords[vertexCount];
extern Rect						coordsScreen, coordsTexture;
static const int texturesNum	= 5;

#if defined(__cplusplus_winrt)
	extern ComPtr<ID3D11DeviceContext1>		comDeviceContext;    // the device context interface
	extern ComPtr<ID3D11RenderTargetView>	comBackBuffer;
	extern ComPtr<ID3D11Buffer>				comVertexBuffer;
	extern ComPtr<ID3D11Buffer>				comIndexBuffer;
	extern ComPtr<ID3D11VertexShader>		comVertexShader;
	extern ComPtr<ID3D11PixelShader>		comPixelShader;
	extern ComPtr<ID3D11InputLayout>		comInputLayout;    // the input layout interface
	extern ComPtr<ID3D11SamplerState>		comSamplerState;
	extern ComPtr<ID3D11ShaderResourceView>	comShaderViews[texturesNum];
#else
	extern ID3D11DeviceContext				*comDeviceContext;           // the pointer to our Direct3D device context
	extern ID3D11RenderTargetView			*comBackBuffer;			// the pointer to our back buffer
	extern ID3D11Buffer						*comVertexBuffer;
	extern ID3D11Buffer						*comIndexBuffer;
	extern ID3D11VertexShader				*comVertexShader;    // the vertex shader
	extern ID3D11PixelShader				*comPixelShader;     // the pixel shader
	extern ID3D11InputLayout				*comInputLayout;
	extern ID3D11SamplerState				*comSamplerState;
	extern ID3D11ShaderResourceView			*comShaderViews[texturesNum];
#endif

class MiniGamePicturePuzzle : public MiniGame
{
public:
    MiniGamePicturePuzzle();
    ~MiniGamePicturePuzzle();
	void Initialize();	
	void Click(float x, float y);
	bool IsComplete() const;
	void Render() const;

	#if !defined(__cplusplus_winrt)
		HWND hWnd;
	#endif
private:
	void InitPipeline(void);
	void InitBuffers(void);
	bool isPointInsideCircle(Rect coordsCircle, float x, float y, float screenWidth, float screenHeight);
	void ReleaseComObjects(void);

	Rect				coordsLabel, coordsIcon1, coordsIcon2;
	VERTEX_COLOR		ColorLinesVertices[vertexCount];
	static const int	indexcolorCount = MiniGame::cRows*MiniGame::cColumns*5;
	
	mutable int txtID;
	bool isFirstClick, isHardMode, isRestart;
	mutable bool flagGameFinished;
	int previousCellNumber, currentCellNumber;

	LPCWSTR mainTextureFileName;
	WCHAR initialDirectory[MAX_PATH];;

	#if defined(__cplusplus_winrt)
		ComPtr<ID3D11Device1>		comDevice;
		ComPtr<IDXGISwapChain1>		comSwapChain;
		ComPtr<ID3D11Buffer>		fontVertexBuffer; 
		ComPtr<ID3D11Buffer>		fontIndexBuffer;
		ComPtr<ID3D11Buffer>		iconLoadPicVertexBuffer;
		ComPtr<ID3D11Buffer>		iconModeSwitchVertexBuffer;
		ComPtr<ID3D11VertexShader>	colorVertexShader;
		ComPtr<ID3D11PixelShader>	colorPixelShader;
		ComPtr<ID3D11Buffer>		colorVertexBuffer; 
		ComPtr<ID3D11Buffer>		colorIndexBuffer; 
		ComPtr<ID3D11InputLayout>	colorInputLayout;    // the input layout interface
	#else
		ID3D11Device				*comDevice;                     // the pointer to our Direct3D device interface
		IDXGISwapChain				*comSwapChain;             // the pointer to the swap chain interface
		ID3D11Buffer				*fontVertexBuffer; 
		ID3D11Buffer				*fontIndexBuffer; 
		ID3D11Buffer				*iconLoadPicVertexBuffer;
		ID3D11Buffer				*iconModeSwitchVertexBuffer;
		ID3D11VertexShader			*colorVertexShader;
		ID3D11PixelShader			*colorPixelShader;
		ID3D11Buffer				*colorVertexBuffer; 
		ID3D11Buffer				*colorIndexBuffer; 
		ID3D11InputLayout			*colorInputLayout;    // the input layout interface
	#endif
};