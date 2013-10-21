#include "pch.h"
#include "minigame_picturepuzzle2.h"

ComPtr<ID3D11DeviceContext1>		comDeviceContext;
ComPtr<ID3D11RenderTargetView>		comBackBuffer;
ComPtr<ID3D11Buffer>				comVertexBuffer;
ComPtr<ID3D11Buffer>				comIndexBuffer;
ComPtr<ID3D11VertexShader>			comVertexShader;
ComPtr<ID3D11PixelShader>			comPixelShader;
ComPtr<ID3D11InputLayout>			comInputLayout; 
ComPtr<ID3D11SamplerState>			comSamplerState;
ComPtr<ID3D11ShaderResourceView>	comShaderViews[texturesNum];

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

	// Create the device and device context objects
	//D3D_FEATURE_LEVEL levelFeature = D3D_FEATURE_LEVEL_9_1;
    HRESULT h =  D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &dev11, nullptr/*&levelFeature*/, &devcon11);
	//D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &comDevice, nullptr/*&levelFeature*/, &comDeviceContext);
    
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
    ComPtr<ID3D11Texture2D> tmpBackBuffer;
    comSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &tmpBackBuffer);

    // create a render target pointing to the back buffer
	HRESULT hh = comDevice->CreateRenderTargetView(tmpBackBuffer.Get(), nullptr, comBackBuffer.GetAddressOf());

    // set the render target as the back buffer
    comDeviceContext->OMSetRenderTargets(1, comBackBuffer.GetAddressOf(), NULL);
    //comDeviceContext->OMSetRenderTargets(1, comBackBuffer.GetAddressOf(), nullptr);

	// set the viewport
    D3D11_VIEWPORT viewport = {0};

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = Window->Bounds.Width;
    viewport.Height = Window->Bounds.Height;

    comDeviceContext->RSSetViewports(1, &viewport);

	//can't use D3DX11CreateShaderResourceViewFromFile for WinStore
	HRESULT	result = CreateWICTextureFromFile(comDevice.Get(), comDeviceContext.Get(), L".\\beyond.jpg", NULL, &comShaderViews[0], 2048);

    InitPipeline();
	InitBuffers();
}

// this is the function that creates the shape to render
void MiniGamePicturePuzzle::InitBuffers()
{
	VERTEX_TEXTURE OurVertices[] =
    {
        { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f,1.0f) },
        { XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f,0.0f) },
        { XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f,0.0f) },
		 { XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f,1.0f) }
    };

	D3D11_BUFFER_DESC vertexBufferDesc = {0};
    vertexBufferDesc.ByteWidth = sizeof(VERTEX_TEXTURE) * ARRAYSIZE(OurVertices);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {OurVertices, 0, 0};

    comDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &comVertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	unsigned long indices[6] = {0, 1, 3, 3, 1, 2}; //rectangle = two triangles
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 6;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	comDevice->CreateBuffer(&indexBufferDesc, &indexData, &comIndexBuffer);
}


// this function loads and prepares the shaders
void MiniGamePicturePuzzle::InitPipeline()
{
	// Create a Basic Reader-Writer class to load data from disk.  This class is examined
    // in the Resource Loading sample.
    BasicReaderWriter^ reader = ref new BasicReaderWriter();

    // load the shader files
	//ID3D10Blob *VS, *PS;

	// You can use this API to develop your Windows Store apps, but you can't use it in apps that you submit to the Windows Store.
	//HRESULT h1 = D3DCompileFromFile(L".\\res\\shaders_2d.hlsl", NULL, NULL, NULL, "vs_5_0", 0, 0, &VS, nullptr);
    //HRESULT h2 = D3DCompileFromFile(L".\\res\\shaders_2d.hlsl", NULL, NULL, NULL, "ps_5_0", 0, 0, &PS, nullptr);
	// load the shader files
    //Array<byte>^ VSFile = LoadShaderFile("VertexShader.cso");
    //Array<byte>^ PSFile = LoadShaderFile("PixelShader.cso");
	//D3DReadFileToBlob(L"VertexShadercso", &VS);
	//D3DReadFileToBlob(L"PixelShader.cso", &PS);
	auto vertexShaderBytecode = reader->ReadData("VertexShader.cso");
	HRESULT H1 = comDevice->CreateVertexShader(vertexShaderBytecode->Data, vertexShaderBytecode->Length, nullptr, &comVertexShader);
	auto pixelShaderBytecode = reader->ReadData("PixelShader.cso");
	H1= comDevice->CreatePixelShader(pixelShaderBytecode->Data, pixelShaderBytecode->Length, nullptr, &comPixelShader);

	/*std::ifstream vs_stream;
	size_t vs_size;
	char* vs_data;

	vs_stream.open("VertexShader.cso", std::ifstream::in | std::ifstream::binary);
	if(vs_stream.good())
	{
		vs_stream.seekg(0, std::ios::end);
		vs_size = size_t(vs_stream.tellg());
		vs_data = new char[vs_size];
		vs_stream.seekg(0, std::ios::beg);
		vs_stream.read(&vs_data[0], vs_size);
		vs_stream.close();

		HRESULT H1 = comDevice->CreateVertexShader(&vs_data, vs_size, 0, &comVertexShader);
		HRESULT H2 = H1+1;
	}*/

	//vertexShaderBytecode = reader->ReadData("PixelShader.cso");
	//comDevice->CreateVertexShader(vertexShaderBytecode->Data, vertexShaderBytecode->Length, nullptr, &comPixelShader);

	// create the shader objects
    //comDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &comVertexShader);
    //comDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &comPixelShader);
	// create the shader objects
    //comDevice->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &comVertexShader);
    //comDevice->CreatePixelShader(PSFile->Data, PSFile->Length, nullptr, &comPixelShader);

	// set the shader objects as the active shaders
    comDeviceContext->VSSetShader(comVertexShader.Get(), nullptr, 0);
    comDeviceContext->PSSetShader(comPixelShader.Get(), nullptr, 0);

	// create the input layout object
    D3D11_INPUT_ELEMENT_DESC myInputLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    H1 = comDevice->CreateInputLayout(myInputLayout, ARRAYSIZE(myInputLayout), vertexShaderBytecode->Data, vertexShaderBytecode->Length, &comInputLayout);
	comDeviceContext->IASetInputLayout(comInputLayout.Get());

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    //samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
    HRESULT result = comDevice->CreateSamplerState(&samplerDesc, &comSamplerState);
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
	comDeviceContext->OMSetRenderTargets(1, comBackBuffer.GetAddressOf(), NULL);

	// Set shader texture resource in the pixel shader.
	//comDeviceContext->PSSetShaderResources(0, 1, &textureShaderViews[0]); //APPLYING TEXTURE

	 // Очищаем буфер глубин до едицины (максимальная глубина)
    //g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	 // clear the back buffer to a deep blue
    float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    comDeviceContext->ClearRenderTargetView(comBackBuffer.Get(), color);
	
		// set the vertex buffer
    UINT stride = sizeof(VERTEX_TEXTURE);
	//UINT stride = sizeof(VERTEX);
    UINT offset = 0;
	comDeviceContext->IASetVertexBuffers(0, 1, comVertexBuffer.GetAddressOf(), &stride, &offset);

	comDeviceContext->VSSetShader(comVertexShader.Get(), NULL, 0 );
	comDeviceContext->PSSetShader(comPixelShader.Get(), NULL, 0);
	comDeviceContext->PSSetShaderResources( 0, 1, comShaderViews[0].GetAddressOf());
	comDeviceContext->PSSetSamplers( 0, 1, comSamplerState.GetAddressOf() );

	comDeviceContext->IASetIndexBuffer(comIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// set the primitive topology
    comDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*comDeviceContext->VSSetShader(comVertexShader.Get(), NULL, 0);
	comDeviceContext->PSSetShader(comPixelShader.Get(), NULL, 0);

	// Set the sampler state in the pixel shader.
	comDeviceContext->PSSetSamplers(0, 1, &comSamplerState);*/
	
	 // draw 3 vertices, starting from vertex 0
    //comDeviceContext->Draw(3, 0);
	comDeviceContext->DrawIndexed(6, 0, 0);
	
	// switch the back buffer and the front buffer
    comSwapChain->Present(1, 0);
}

void Render(const Rect& screenCoords, int textureId, const Rect& textureCoords)
{
	
}


