#include <windows.h>
#include <windowsx.h>

#include <sstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>       /* fmod */

#include "minigameinterface.h"

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


struct VERTEX_TEXTURE
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
};

//эти переменные глобальные только потому, что требуется вызов глобальной функции Render из класса MiniGamePicturePuzzle,
//а ей нужно передавать параметры, а они ограничены шаблоном из текста тестового задания...
static const int				vertexCount = MiniGame::cRows*MiniGame::cColumns*4; //may be  (cRows+1)*(cColumns+1);
static const int				indexCount = MiniGame::cRows*MiniGame::cColumns*6;
extern VERTEX_TEXTURE			RectangleVertices[vertexCount];
extern D3DXVECTOR2				StandardTextCoords[vertexCount];
extern ID3D11DeviceContext		*comDeviceContext;           // the pointer to our Direct3D device context
extern ID3D11Buffer				*comVertexBuffer;
extern Rect						coordsScreen, coordsTexture;
static const int texturesNum = 3;
extern ID3D11ShaderResourceView	*textureShaderViews[texturesNum];
extern int						currentTextureNum;
extern	ID3D11RenderTargetView		*comBackBuffer;			// the pointer to our back buffer
extern	ID3D11VertexShader			*comVertexShader;    // the vertex shader
extern	ID3D11PixelShader			*comPixelShader;     // the pixel shader
extern	ID3D11Buffer				*comIndexBuffer;
extern	ID3D11InputLayout			*comInputLayout;
extern	ID3D11SamplerState			*comSamplerState;

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


	mutable int txtID;

	bool isFirstClick;
	mutable bool flagGameFinished;
	int previousCellNumber, currentCellNumber;

	IDXGISwapChain				*comSwapChain;             // the pointer to the swap chain interface
	ID3D11Device				*comDevice;                     // the pointer to our Direct3D device interface

	ID3D11VertexShader			*fontVertexShader;    // the vertex shader
	ID3D11PixelShader			*fontPixelShader;     // the pixel shader
	ID3D11Buffer				*fontVertexBuffer; 
	ID3D11Buffer				*fontIndexBuffer; 
	ID3D11SamplerState			*fontAtlasSampler;
};