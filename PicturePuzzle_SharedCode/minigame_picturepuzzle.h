#if defined(__cplusplus_winrt)
	#include <wrl/client.h>
	#include <d3d11_1.h>
	#include <DirectXMath.h>
	#include "BasicReaderWriter.h" //compiled shaders (*.cso) loading
	#include "WICTextureLoader.h" //image to texture converting (because D3DX11CreateShaderResourceViewFromFile is deprecated for Windows Store:( )
	#pragma once
	using namespace Microsoft::WRL;
	using namespace Windows::UI::Core;
	using namespace DirectX;
	typedef XMFLOAT3 VECTOR_F3;
	typedef XMFLOAT2 VECTOR_F2;
#else
	#include <windows.h>
	#include <windowsx.h>
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
	extern ID3D11DeviceContext			*comDeviceContext;           // the pointer to our Direct3D device context
	extern ID3D11RenderTargetView		*comBackBuffer;			// the pointer to our back buffer
	extern ID3D11Buffer					*comVertexBuffer;
	extern ID3D11Buffer					*comIndexBuffer;
	extern ID3D11ShaderResourceView		*comShaderViews[texturesNum];
	extern ID3D11VertexShader			*comVertexShader;    // the vertex shader
	extern ID3D11PixelShader			*comPixelShader;     // the pixel shader
	extern ID3D11InputLayout			*comInputLayout;
	extern ID3D11SamplerState			*comSamplerState;
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

	#if defined(__cplusplus_winrt)
		//Rect	screenFull;
	#else
		HWND hWnd;
	#endif
private:
	void InitPipeline(void);
	void InitBuffers(void);
	bool isPointInsideCircle(Rect coordsCircle, float x, float y, float screenWidth, float screenHeight);

	Rect	coordsLabel, coordsIcon1, coordsIcon2;
	
	mutable int txtID;
	bool isFirstClick, isHardMode;
	mutable bool flagGameFinished;
	int previousCellNumber, currentCellNumber;

	#if defined(__cplusplus_winrt)
		ComPtr<ID3D11Device1>		comDevice;
		ComPtr<IDXGISwapChain1>		comSwapChain;
		ComPtr<ID3D11Buffer>		fontVertexBuffer; 
		ComPtr<ID3D11Buffer>		fontIndexBuffer; 
		ComPtr<ID3D11SamplerState>	fontAtlasSampler;
		ComPtr<ID3D11Buffer>		iconLoadPicVertexBuffer;
		ComPtr<ID3D11Buffer>		iconModeSwitchVertexBuffer;
	#else
		ID3D11Device				*comDevice;                     // the pointer to our Direct3D device interface
		IDXGISwapChain				*comSwapChain;             // the pointer to the swap chain interface
		ID3D11Buffer				*fontVertexBuffer; 
		ID3D11Buffer				*fontIndexBuffer; 
		ID3D11SamplerState			*fontAtlasSampler;
		ID3D11Buffer				*iconLoadPicVertexBuffer;
		ID3D11Buffer				*iconModeSwitchVertexBuffer;
	#endif
};