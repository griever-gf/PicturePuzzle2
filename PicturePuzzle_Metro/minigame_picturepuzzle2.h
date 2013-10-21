#include "pch.h"

#include "minigameinterface.h"
#include "WICTextureLoader.h" //image to texture converting (because D3DX11CreateShaderResourceViewFromFile is deprecated for Windows Store:( )
#include "BasicReaderWriter.h" //compiled shaders (*.cso) loading
//#include <fstream>

#include <sstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>       /* fmod */

#pragma once

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;

// define the screen resolution for desktop window
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600


struct VERTEX_TEXTURE
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
};

//эти переменные глобальные только потому, что требуется вызов глобальной функции Render из класса MiniGamePicturePuzzle,
//а ей нужно передавать параметры, а они ограничены шаблоном из текста тестового задания...
static const int				vertexCount = MiniGame::cRows*MiniGame::cColumns*4; //may be  (cRows+1)*(cColumns+1);
static const int				indexCount = MiniGame::cRows*MiniGame::cColumns*6;
extern VERTEX_TEXTURE			RectangleVertices[vertexCount];
extern XMFLOAT2					StandardTextCoords[vertexCount];
extern Rect						coordsScreen, coordsTexture;
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

	Rect	coordsLabel, coordsIcon;

	mutable int txtID;
	bool isFirstClick;
	mutable bool flagGameFinished;
	int previousCellNumber, currentCellNumber;

	ComPtr<ID3D11Device1> comDevice;
	ComPtr<IDXGISwapChain1> comSwapChain;
};