#include <windows.h>
#include <windowsx.h>

#include <sstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

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

	static const int vertexCount = cRows*cColumns*4; //may be  (cRows+1)*(cColumns+1);
	static const int indexCount = cRows*cColumns*6;

	struct VERTEX_TEXTURE
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	VERTEX_TEXTURE RectangleVertices[vertexCount];
	D3DXVECTOR2 StandardTextCoords[vertexCount];

	bool isFirstClick;
	mutable bool flagGameFinished;
	int previousCellNumber, currentCellNumber;

	IDXGISwapChain				*comSwapChain;             // the pointer to the swap chain interface
	ID3D11Device				*comDevice;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext			*comDeviceContext;           // the pointer to our Direct3D device context
	ID3D11RenderTargetView		*comBackBuffer;			// the pointer to our back buffer

	ID3D11VertexShader			*comVertexShader;    // the vertex shader
	ID3D11PixelShader			*comPixelShader;     // the pixel shader
	ID3D11Buffer				*comVertexBuffer; 
	ID3D11Buffer				*comIndexBuffer; 
	ID3D11InputLayout			*comInputLayout;
	ID3D11ShaderResourceView	*comTextureShaderView; // Объект текстуры
	ID3D11SamplerState			*comSamplerState;

	ID3D11ShaderResourceView	*fontTextureShaderView;
	ID3D11VertexShader			*fontVertexShader;    // the vertex shader
	ID3D11PixelShader			*fontPixelShader;     // the pixel shader
	ID3D11Buffer				*fontVertexBuffer; 
	ID3D11Buffer				*fontIndexBuffer; 
	ID3D11SamplerState			*fontAtlasSampler;
};