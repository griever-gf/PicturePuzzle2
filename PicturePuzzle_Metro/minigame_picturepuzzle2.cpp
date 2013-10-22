#include "pch.h"
#include "minigame_picturepuzzle2.h"

Rect						coordsScreen,	coordsTexture;
Rect						coordsScreenNew, coordsTextureNew;
VERTEX_TEXTURE				RectangleVertices[vertexCount];
XMFLOAT2					StandardTextCoords[vertexCount];
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
	isFirstClick = false;
	flagGameFinished = false;
	previousCellNumber = 0;
	currentCellNumber = 0;
	txtID = 0;

	coordsScreen.left = -1.0f; coordsScreen.bottom = -1.0f; coordsScreen.right = 1.0f; coordsScreen.top = 1.0f;
	coordsTexture.left = 0.0f; coordsTexture.bottom = 0.0f; coordsTexture.right = 1.0f; coordsTexture.top = 1.0f;

	coordsScreenNew.left = -1.0f; coordsScreenNew.right = 1.0f; coordsScreenNew.top = 1.0f; coordsScreenNew.bottom = -1.0f;
	coordsTextureNew.left = 0.0f; coordsTextureNew.right = 1.0f; coordsTextureNew.top = 1.0f; coordsTextureNew.bottom = 0.0f;

	coordsLabel.left = -0.9f; coordsLabel.right = 0.9f; coordsLabel.top = 0.25f; coordsLabel.bottom = -0.25f;

	coordsIcon.left = -0.95f; coordsIcon.right = -0.8f; coordsIcon.top = 0.95f; coordsIcon.bottom = 0.75f;

	// Define temporary pointers to a device and a device context
    ComPtr<ID3D11Device> dev11;
    ComPtr<ID3D11DeviceContext> devcon11;

	// Create the device and device context objects
	//D3D_FEATURE_LEVEL levelFeature = D3D_FEATURE_LEVEL_9_1;
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

	//определяем разрешение экрана
	IDXGIOutput *pOutput;
	dxgiAdapter->EnumOutputs(0, &pOutput);
	DXGI_OUTPUT_DESC desc;
	pOutput->GetDesc(&desc);
	screenFull.left = desc.DesktopCoordinates.left;
	screenFull.right = desc.DesktopCoordinates.right;
	screenFull.top = desc.DesktopCoordinates.top;
	screenFull.bottom = desc.DesktopCoordinates.bottom;

	// set up the swap chain description
    DXGI_SWAP_CHAIN_DESC1 tempSwapChain = {0};
    tempSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // how the swap chain should be used
    tempSwapChain.BufferCount = 2;                                  // a front buffer and a back buffer
    tempSwapChain.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // the most common swap chain format
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

	// set the viewport
    D3D11_VIEWPORT viewport = {0};

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = Window->Bounds.Width;
    viewport.Height = Window->Bounds.Height;

    comDeviceContext->RSSetViewports(1, &viewport);

	//can't use D3DX11CreateShaderResourceViewFromFile for WinStore
	HRESULT	r1 = CreateWICTextureFromFile(comDevice.Get(), comDeviceContext.Get(), L".\\res\\beyond.jpg", NULL, &comShaderViews[0], 2048);
	r1 = CreateWICTextureFromFile(comDevice.Get(), comDeviceContext.Get(), L".\\res\\fargus_souls.jpeg", NULL, &comShaderViews[1], 2048);
	r1 = CreateWICTextureFromFile(comDevice.Get(), comDeviceContext.Get(), L".\\res\\task_complete.png", NULL, &comShaderViews[2], 2048);
	r1 = CreateWICTextureFromFile(comDevice.Get(), comDeviceContext.Get(), L".\\res\\icon.png", NULL, &comShaderViews[3], 2048);

    InitPipeline();
	InitBuffers();
}

// this is the function that creates the shape to render
void MiniGamePicturePuzzle::InitBuffers()
{
	
	XMFLOAT3 ScreenCoordsArray[vertexCount/4][4];
	XMFLOAT2 TextCoordsArray[vertexCount/4][4];
	unsigned long indices[indexCount];
	for (int i = 0; i < cRows; i++)
	{
		for (int j = 0; j < cColumns; j++)
		{
			ScreenCoordsArray[(i*cColumns+j)][0] = XMFLOAT3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*j    ), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*(i+1)), 0.0f);
			ScreenCoordsArray[(i*cColumns+j)][1] = XMFLOAT3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*j    ), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*i    ), 0.0f);
			ScreenCoordsArray[(i*cColumns+j)][2] = XMFLOAT3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*(j+1)), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*i    ), 0.0f);
			ScreenCoordsArray[(i*cColumns+j)][3] = XMFLOAT3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*(j+1)), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*(i+1)), 0.0f);
			TextCoordsArray[(i*cColumns+j)][0] = XMFLOAT2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+0)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+1)+coordsTexture.top);
			TextCoordsArray[(i*cColumns+j)][1] = XMFLOAT2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+0)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+0)+coordsTexture.top);
			TextCoordsArray[(i*cColumns+j)][2] = XMFLOAT2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+1)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+0)+coordsTexture.top);
			TextCoordsArray[(i*cColumns+j)][3] = XMFLOAT2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+1)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+1)+coordsTexture.top);
			indices[(i*cColumns+j)*6  ] = (i*cColumns+j)*4;
			indices[(i*cColumns+j)*6+1] = (i*cColumns+j)*4+1;
			indices[(i*cColumns+j)*6+2] = (i*cColumns+j)*4+3;
			indices[(i*cColumns+j)*6+3] = (i*cColumns+j)*4+3;
			indices[(i*cColumns+j)*6+4] = (i*cColumns+j)*4+1;
			indices[(i*cColumns+j)*6+5] = (i*cColumns+j)*4+2;
		}
	}
	//save original texture coordinates
	for (int i=0; i< vertexCount; i++)
		StandardTextCoords[i] = TextCoordsArray[i/4][i%4];
	// shuffle texture coordinates (by group of 4)
	srand ((unsigned int)time(NULL));
	XMFLOAT2 t1, t2;
    for(int i = 0; i < 100; i++){ // 100 is just a big number
        for(int k=0;k<(vertexCount/4);k++){
            int r2=rand()%(vertexCount/4); //# of rows
            for(int m=0;m<4;m++){
                t1=TextCoordsArray[k][m];
                t2=TextCoordsArray[r2][m];
                TextCoordsArray[r2][m]=t1;
                TextCoordsArray[k][m]=t2;
            }
        }
    }
	
	for (int i = 0; i < vertexCount/4; i++)
		for (int j = 0; j < 4; j++)
	{
		RectangleVertices[i*4+j].position = ScreenCoordsArray[i][j];
		RectangleVertices[i*4+j].texture = TextCoordsArray[i][j];
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {0};
    vertexBufferDesc.ByteWidth = sizeof(VERTEX_TEXTURE) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA vertexData = {RectangleVertices, 0, 0};

    comDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &comVertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	comDevice->CreateBuffer(&indexBufferDesc, &indexData, &comIndexBuffer);

	VERTEX_TEXTURE LabelVertices[4] = { {XMFLOAT3(coordsLabel.left,coordsLabel.bottom,0.0f),XMFLOAT2(0.0f,1.0f)},
										{XMFLOAT3(coordsLabel.left,coordsLabel.top,0.0f),XMFLOAT2(0.0f,0.0f)},
										{XMFLOAT3(coordsLabel.right,coordsLabel.top,0.0f),XMFLOAT2(1.0f,0.0f)},
										{XMFLOAT3(coordsLabel.right,coordsLabel.bottom,0.0f),XMFLOAT2(1.0f,1.0f)}};
	vertexBufferDesc.ByteWidth = sizeof(VERTEX_TEXTURE) * 4;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexData.pSysMem = LabelVertices;
	comDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &fontVertexBuffer); 

	unsigned long label_indices[6] = {0, 1, 3, 3, 1, 2}; //rectangle = two triangles
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 6;
	indexData.pSysMem = label_indices;
	comDevice->CreateBuffer(&indexBufferDesc, &indexData, &fontIndexBuffer);

	VERTEX_TEXTURE IconVertices[4] = { {XMFLOAT3(coordsIcon.left,coordsIcon.bottom,0.0f),XMFLOAT2(0.0f,1.0f)},
										{XMFLOAT3(coordsIcon.left,coordsIcon.top,0.0f),XMFLOAT2(0.0f,0.0f)},
										{XMFLOAT3(coordsIcon.right,coordsIcon.top,0.0f),XMFLOAT2(1.0f,0.0f)},
										{XMFLOAT3(coordsIcon.right,coordsIcon.bottom,0.0f),XMFLOAT2(1.0f,1.0f)}};
	vertexData.pSysMem = IconVertices;
	comDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &iconVertexBuffer);
}


// this function loads and prepares the shaders
void MiniGamePicturePuzzle::InitPipeline()
{
	// Create a Basic Reader-Writer class to load data from disk.  This class is examined
    // in the Resource Loading sample.
    BasicReaderWriter^ reader = ref new BasicReaderWriter();
	auto vertexShaderBytecode = reader->ReadData("VertexShader.cso");
	HRESULT H1 = comDevice->CreateVertexShader(vertexShaderBytecode->Data, vertexShaderBytecode->Length, nullptr, &comVertexShader);
	auto pixelShaderBytecode = reader->ReadData("PixelShader.cso");
	H1= comDevice->CreatePixelShader(pixelShaderBytecode->Data, pixelShaderBytecode->Length, nullptr, &comPixelShader);

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

	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	//comDevice->CreateSamplerState(&samplerDesc,&fontAtlasSampler);
}

void MiniGamePicturePuzzle::Click(float x1, float y1)
{
	isFirstClick = !isFirstClick;
			
	Rect rect = screenFull;
	float width = SCREEN_WIDTH;
	float height = SCREEN_HEIGHT;
	//if(GetClientRect(hWnd, &rect)) for DESKTOP
	{
		width = (rect.right - rect.left)*((coordsScreen.right - coordsScreen.left)/2.0f);
		height = (rect.bottom - rect.top)*((coordsScreen.top - coordsScreen.bottom)/2.0f);
	}

	float oneColumnSize = width/cColumns;
	float oneRowSize = height/cRows;
	float firstColumnCoordX = ((float)(rect.right - rect.left))*(1.0f+coordsScreen.left)/2;
	float firstRowCoordY = ((float)(rect.bottom - rect.top))*(1.0f-coordsScreen.top)/2;

	float centerx = width*((coordsIcon.right+coordsIcon.left+2)/4);
	float centery = height*(1 - (coordsIcon.top+coordsIcon.bottom+2)/4);
	float radiusx = width*((coordsIcon.right - coordsIcon.left)/2);
	float radiusy = height*((coordsIcon.top - coordsIcon.bottom)/2);
	//if point inside the circle icon...
	if (((x1 - centerx)*(x1 - centerx) + (y1 - centery)*(y1 - centery)) < radiusx*radiusy)
	{
		std::wstringstream WStrStream;
		WStrStream << "inside a circle!";
		//MessageBox(hWnd,WStrStream.str().c_str(),L"Element coords",MB_OK);
		Platform::String^ plStr = ref new Platform::String(WStrStream.str().c_str());
		Windows::UI::Popups::MessageDialog Dialog(plStr, "Notice!");
        Dialog.ShowAsync();
		return;
	}
	else	//если кликнули не в прямоугольник - выходим
		if (!((firstColumnCoordX<=x1)&&(firstColumnCoordX+width>=x1)&&(firstRowCoordY<=y1)&&(firstRowCoordY+height>=y1)))
			return;
		else
			if (!flagGameFinished)
			{
				int x = (int)((x1 - firstColumnCoordX) / oneColumnSize );
				int y = (int)((y1 - firstRowCoordY ) / oneRowSize );

				currentCellNumber = x + y*cColumns;

				//std::wstringstream WStrStream;
				//WStrStream << "x:" << x << ",y:" << y << " ,cellnum:" << currentCellNumber;
				//MessageBox(hWnd,WStrStream.str().c_str(),L"Element coords",MB_OK);
				if ((!isFirstClick)&&(currentCellNumber!=previousCellNumber)) //if second click - swap texture coordinates for rectangle regions
				{
					D3D11_MAPPED_SUBRESOURCE mappedSubRes;
					ZeroMemory( &mappedSubRes, sizeof(D3D11_MAPPED_SUBRESOURCE) );
					comDeviceContext->Map(comVertexBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubRes);  

					//swap texture_coords[4] gropes
					XMFLOAT2 buf[4];
					buf[0]= RectangleVertices[previousCellNumber*4+0].texture;
					buf[1]= RectangleVertices[previousCellNumber*4+1].texture;
					buf[2]= RectangleVertices[previousCellNumber*4+2].texture;
					buf[3]= RectangleVertices[previousCellNumber*4+3].texture;
					RectangleVertices[previousCellNumber*4+0].texture = RectangleVertices[currentCellNumber*4+0].texture;
					RectangleVertices[previousCellNumber*4+1].texture = RectangleVertices[currentCellNumber*4+1].texture;
					RectangleVertices[previousCellNumber*4+2].texture = RectangleVertices[currentCellNumber*4+2].texture;
					RectangleVertices[previousCellNumber*4+3].texture = RectangleVertices[currentCellNumber*4+3].texture;
					RectangleVertices[currentCellNumber*4+0].texture = buf[0];
					RectangleVertices[currentCellNumber*4+1].texture = buf[1];
					RectangleVertices[currentCellNumber*4+2].texture = buf[2];
					RectangleVertices[currentCellNumber*4+3].texture = buf[3];
				
					//понятное дело, что лучше обновить часть вертексного буфера, чем буфер целиком, но у меня не получилось :(
					//пытался копать CopySubresourceRegion, но...
					memcpy(mappedSubRes.pData, RectangleVertices, sizeof(RectangleVertices));  
					comDeviceContext->Unmap(comVertexBuffer.Get(), NULL);
				}
				previousCellNumber = currentCellNumber;
			}
}

bool MiniGamePicturePuzzle::IsComplete() const
{
	bool res = false;
	for (int i=0; i< vertexCount; i++)
	{
		if ((StandardTextCoords[i].x != RectangleVertices[i].texture.x)||(StandardTextCoords[i].y != RectangleVertices[i].texture.y))
			return res;
	}
	res = true;
	return res;
}

void MiniGamePicturePuzzle::Render() const
{
	//uncomment to see some FUNNY effect :)
	/*
	coordsScreenNew.left = fmod(coordsScreenNew.left+0.0001+1, 1) - 1;
	coordsScreenNew.right = fmod(coordsScreenNew.right+0.0001+1, 1);
	coordsScreenNew.top = fmod(coordsScreenNew.top+0.0001+1, 1);
	coordsScreenNew.bottom = fmod(coordsScreenNew.bottom+0.0001+1, 1) - 1;
	coordsTextureNew.left = fmod(coordsTextureNew.left+0.0001, 0.5);
	coordsTextureNew.right = fmod(coordsTextureNew.right+0.0001, 0.5) + 0.5;
	coordsTextureNew.bottom = fmod(coordsTextureNew.bottom+0.0001, 0.5);
	coordsTextureNew.top = fmod(coordsTextureNew.top+0.0001, 0.5) + 0.5;
	txtID = (txtID+1) % 600;
	*/
	//comment lower string to see some FUNNY effect :)
	txtID = 0;
	::Render(coordsScreenNew, txtID / 300, coordsTextureNew);

	//open file icon render
	comDeviceContext->PSSetShaderResources(0, 1, comShaderViews[3].GetAddressOf());
	UINT stride = sizeof(VERTEX_TEXTURE);
	UINT offset = 0;
	comDeviceContext->IASetVertexBuffers(0, 1, iconVertexBuffer.GetAddressOf(), &stride, &offset);
	comDeviceContext->IASetIndexBuffer(fontIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); //because the same for font & icon
	comDeviceContext->DrawIndexed(6, 0, 0);

	if (IsComplete()) //render "complete" label
	{
		flagGameFinished = true;
		comDeviceContext->PSSetShaderResources(0, 1, comShaderViews[2].GetAddressOf());
		comDeviceContext->IASetVertexBuffers(0, 1, fontVertexBuffer.GetAddressOf(), &stride, &offset);
		comDeviceContext->DrawIndexed(6, 0, 0);
	}
	
	// switch the back buffer and the front buffer
    comSwapChain->Present(1, 0);
}

void Render(const Rect& screenCoords, int textureId, const Rect& textureCoords)
{
	// наверное, тут надо применять матрицы преобразований, но я так устал... :)
	float levelCompressionCoordX = (screenCoords.right - screenCoords.left)/(coordsScreen.right - coordsScreen.left);
	float levelCompressionCoordY = (screenCoords.top - screenCoords.bottom)/(coordsScreen.top - coordsScreen.bottom);
	float levelShiftCoordX = screenCoords.left - coordsScreen.left;
	float levelShiftCoordY = screenCoords.bottom - coordsScreen.bottom;
	float levelCompressionTextureX = (textureCoords.right - textureCoords.left)/(coordsTexture.right - coordsTexture.left);
	float levelCompressionTextureY = (textureCoords.bottom - textureCoords.top)/(coordsTexture.bottom - coordsTexture.top);
	float levelShiftTextureX = textureCoords.left - coordsTexture.left;
	float levelShiftTextureY = textureCoords.top - coordsTexture.top;

	for (int k = 0; k < vertexCount; k++)
	{
		RectangleVertices[k].position.x = (RectangleVertices[k].position.x+1)*levelCompressionCoordX - 1 + levelShiftCoordX;
		RectangleVertices[k].position.y = (RectangleVertices[k].position.y+1)*levelCompressionCoordY - 1 + levelShiftCoordY;
		RectangleVertices[k].texture.x = RectangleVertices[k].texture.x*levelCompressionTextureX + levelShiftTextureX;
		RectangleVertices[k].texture.y = RectangleVertices[k].texture.y*levelCompressionTextureY + levelShiftTextureY;
		StandardTextCoords[k].x = StandardTextCoords[k].x * levelCompressionTextureX + levelShiftTextureX;
		StandardTextCoords[k].y = StandardTextCoords[k].y * levelCompressionTextureY + levelShiftTextureY;
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubRes;
	ZeroMemory( &mappedSubRes, sizeof(D3D11_MAPPED_SUBRESOURCE) );
	comDeviceContext->Map(comVertexBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubRes);  
	memcpy(mappedSubRes.pData, RectangleVertices, sizeof(RectangleVertices));  
	comDeviceContext->Unmap(comVertexBuffer.Get(), NULL);

	//save current rectangle
	coordsScreen = screenCoords;
	coordsTexture = textureCoords;

	comDeviceContext->OMSetRenderTargets(1, comBackBuffer.GetAddressOf(), NULL);

	// clear the back buffer to a deep blue
	float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
	comDeviceContext->ClearRenderTargetView(comBackBuffer.Get(), color);

	// select which vertex buffer to display
    UINT stride = sizeof(VERTEX_TEXTURE);
    UINT offset = 0;
	comDeviceContext->IASetVertexBuffers(0, 1, comVertexBuffer.GetAddressOf(), &stride, &offset);

	comDeviceContext->VSSetShader(comVertexShader.Get(), NULL, 0 );
	comDeviceContext->PSSetShader(comPixelShader.Get(), NULL, 0);
	comDeviceContext->PSSetShaderResources( 0, 1, comShaderViews[textureId].GetAddressOf());
	comDeviceContext->PSSetSamplers( 0, 1, comSamplerState.GetAddressOf() );

	comDeviceContext->IASetIndexBuffer(comIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    comDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw the vertex buffer to the back buffer
	comDeviceContext->DrawIndexed(indexCount, 0, 0);
}


