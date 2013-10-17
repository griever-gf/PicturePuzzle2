#include "pch.h"
#include "minigame_picturepuzzle2.h"

ComPtr<ID3D11DeviceContext1>	comDeviceContext;
ComPtr<ID3D11Buffer>			comVertexBuffer;
ComPtr<ID3D11VertexShader>		comVertexShader;
ComPtr<ID3D11PixelShader>		comPixelShader;

MiniGamePicturePuzzle::MiniGamePicturePuzzle()
{

}

MiniGamePicturePuzzle::~MiniGamePicturePuzzle()
{

}

void MiniGamePicturePuzzle::Initialize()
{// called before start
	// Define temporary pointers to a device and a device context
    ComPtr<ID3D11Device> dev11;
    ComPtr<ID3D11DeviceContext> devcon11;

	//D3D_FEATURE_LEVEL levelFeature = D3D_FEATURE_LEVEL_9_1;
	// Create the device and device context objects
    HRESULT h =  D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &dev11, nullptr/*&levelFeature*/, &devcon11);
    
    // Convert the pointers from the DirectX 11 versions to the DirectX 11.1 versions
	dev11.As(&comDevice);
	devcon11.As(&comDeviceContext);

	//For every ID3D11Device1 there is a corresponding IDXGIDevice1 that lets us access DXGI components
    ComPtr<IDXGIDevice1> dxgiDevice;
    comDevice.As(&dxgiDevice);
    
    //This could simply be described as the virtutal representation of the video card
	//(assuming the video card is separate, and not built into the motherboard).
    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter);
    
    //Calling GetParent() gets us access to the factory of our adapter and of the device.
	//It has two parameters: the type of interface we are obtaining, and a pointer to store the address in.
    ComPtr<IDXGIFactory2> dxgiFactory;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

	// set up the swap chain description
    DXGI_SWAP_CHAIN_DESC1 tempSwapChain = {0};
    tempSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // how the swap chain should be used
    tempSwapChain.BufferCount = 2;                                  // a front buffer and a back buffer
    tempSwapChain.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // the most common swap chain format
	//tempSwapChain.BufferDesc.Format= DXGI_FORMAT_B8G8R8A8_UNORM;
    tempSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;    // the recommended flip mode
    tempSwapChain.SampleDesc.Count = 1;


	CoreWindow^ Window = CoreWindow::GetForCurrentThread();    // get the window pointer

    dxgiFactory->CreateSwapChainForCoreWindow(comDevice.Get(), reinterpret_cast<IUnknown*>(Window), &tempSwapChain, nullptr, &comSwapChain);

	//An ID3D11Texture2D is an object that stores a flat image. Like any COM object, we first define the pointer, and later a function creates the object for us.
	// get a pointer directly to the back buffer
    ComPtr<ID3D11Texture2D> backbuffer;
    comSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);

    // create a render target pointing to the back buffer
    comDevice->CreateRenderTargetView(backbuffer.Get(), nullptr, &comRendertarget);
}

// this is the function that creates the shape to render
void MiniGamePicturePuzzle::InitBuffers()
{
	VERTEX_TEXTURE OurVertices[] =
    {
        { XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT2(0.0f,1.0f) },
        { XMFLOAT3(0.45f, -0.5f, 0.0f), XMFLOAT2(0.0f,0.0f) },
        { XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT2(1.0f,0.0f) },
    };

	D3D11_BUFFER_DESC vertexBufferDesc = {0};
    vertexBufferDesc.ByteWidth = sizeof(VERTEX_TEXTURE) * ARRAYSIZE(OurVertices);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {OurVertices, 0, 0};

    comDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &comVertexBuffer);
}


// this function loads and prepares the shaders
void MiniGamePicturePuzzle::InitPipeline()
{
    // load the shader files
    //Array<byte>^ VSFile = LoadShaderFile("VertexShader.cso");
    //Array<byte>^ PSFile = LoadShaderFile("PixelShader.cso");
	ID3D10Blob *VS, *PS;

	// You can use this API to develop your Windows Store apps, but you can't use it in apps that you submit to the Windows Store.
	HRESULT h1 = D3DCompileFromFile(L".\\res\\shaders_2d.hlsl", NULL, NULL, NULL, "vs_5_0", 0, 0, &VS, nullptr);
    HRESULT h2 = D3DCompileFromFile(L".\\res\\shaders_2d.hlsl", NULL, NULL, NULL, "ps_5_0", 0, 0, &PS, nullptr);

	// create the shader objects
    comDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &comVertexShader);
    comDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &comPixelShader);

	// set the shader objects as the active shaders
    comDeviceContext->VSSetShader(comVertexShader.Get(), nullptr, 0);
    comDeviceContext->PSSetShader(comPixelShader.Get(), nullptr, 0);
}

void MiniGamePicturePuzzle::Click(float x1, float y1)
{
	
}

bool MiniGamePicturePuzzle::IsComplete() const
{
	return true;
}

void MiniGamePicturePuzzle::Render() const
{
	// set our new render target object as the active render target
    comDeviceContext->OMSetRenderTargets(1, comRendertarget.GetAddressOf(), nullptr);

	 // clear the back buffer to a deep blue
    float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    comDeviceContext->ClearRenderTargetView(comRendertarget.Get(), color);

	// switch the back buffer and the front buffer
    comSwapChain->Present(1, 0);
}

void Render(const Rect& screenCoords, int textureId, const Rect& textureCoords)
{
	
}


