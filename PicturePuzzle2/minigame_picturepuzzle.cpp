#include "minigame_picturepuzzle.h"

Rect coordsScreen, coordsTexture;

MiniGamePicturePuzzle::MiniGamePicturePuzzle()
{
	isFirstClick = false;
	flagGameFinished = false;
	previousCellNumber = 0;
	currentCellNumber = 0;

	ZeroMemory(&textureShaderViews, sizeof(textureShaderViews));
}

MiniGamePicturePuzzle::~MiniGamePicturePuzzle()
{
	comSwapChain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
    // close and release all existing COM objects
	comInputLayout->Release();
    comVertexShader->Release();
    comPixelShader->Release();
    comVertexBuffer->Release();
	comIndexBuffer->Release();
    comSwapChain->Release();
	comBackBuffer->Release();
    comDevice->Release();
    comDeviceContext->Release();
	fontVertexBuffer->Release();
	fontIndexBuffer->Release();
	for(int i = 0; i < 2; i++)
		textureShaderViews[i]->Release();
}

void MiniGamePicturePuzzle::Initialize()
{// called before start
	coordsScreen.left = -1.0f;
	coordsScreen.bottom = -1.0f;
	coordsScreen.right = 1.0f;
	coordsScreen.top = 1.0f;
		
	coordsTexture.left = 0.0f;
	coordsTexture.bottom = 0.0f;
	coordsTexture.right = 1.0f;
	coordsTexture.top = 1.0f;

	// create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC tempSwapChain;

    // clear out the struct for use
    ZeroMemory(&tempSwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    tempSwapChain.BufferCount = 1;                                    // one back buffer
    tempSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	tempSwapChain.BufferDesc.Width = SCREEN_WIDTH;                    // set the back buffer width
    tempSwapChain.BufferDesc.Height = SCREEN_HEIGHT;                  // set the back buffer height
    tempSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    tempSwapChain.OutputWindow = hWnd;                                // the window to be used
    tempSwapChain.SampleDesc.Count = 4;                               // how many multisamples
    tempSwapChain.Windowed = TRUE;                                    // windowed/full-screen mode
	//scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
                                  D3D11_SDK_VERSION, &tempSwapChain, &comSwapChain, &comDevice, NULL, &comDeviceContext);

	// get the address of the back buffer
    ID3D11Texture2D *tempBackBuffer;
    comSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&tempBackBuffer);

    // use the back buffer address to create the render target
    comDevice->CreateRenderTargetView(tempBackBuffer, NULL, &comBackBuffer);
    tempBackBuffer->Release();

    // set the render target as the back buffer
    comDeviceContext->OMSetRenderTargets(1, &comBackBuffer, NULL);

	  // Set the viewport
    D3D11_VIEWPORT tempViewport;
    ZeroMemory(&tempViewport, sizeof(D3D11_VIEWPORT));

    tempViewport.TopLeftX = 0;
    tempViewport.TopLeftY = 0;
    tempViewport.Width = SCREEN_WIDTH;
    tempViewport.Height = SCREEN_HEIGHT;

	//activates viewport structs. The first parameter is the number of viewports being used, 
	//and the second parameter is the address of a list of pointers to the viewport structs.
    comDeviceContext->RSSetViewports(1, &tempViewport);

	InitPipeline();				//load & init shaders
    InitBuffers();				//creating render shape
	//загружаем текстуры
	D3DX11CreateShaderResourceViewFromFile(comDevice, L"beyond.bmp", NULL, NULL, &textureShaderViews[0], NULL);
	D3DX11CreateShaderResourceViewFromFile(comDevice, L"task_complete.png", NULL, NULL, &textureShaderViews[1], NULL);
}

// this is the function that creates the shape to render
void MiniGamePicturePuzzle::InitBuffers()
{
	comVertexBuffer = NULL;
	comIndexBuffer = NULL;
	ZeroMemory(&textureShaderViews, sizeof(textureShaderViews));

	D3DXVECTOR3 ScreenCoordsArray[vertexCount/4][4];
	D3DXVECTOR2 TextCoordsArray[vertexCount/4][4];
	unsigned long indices[indexCount];
	for (int i = 0; i < cRows; i++)
	{
		for (int j = 0; j < cColumns; j++)
		{
			ScreenCoordsArray[(i*cColumns+j)][0] = D3DXVECTOR3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*j    ), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*(i+1)), 0.0f);
			ScreenCoordsArray[(i*cColumns+j)][1] = D3DXVECTOR3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*j    ), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*i    ), 0.0f);
			ScreenCoordsArray[(i*cColumns+j)][2] = D3DXVECTOR3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*(j+1)), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*i    ), 0.0f);
			ScreenCoordsArray[(i*cColumns+j)][3] = D3DXVECTOR3(coordsScreen.left + (((coordsScreen.right-coordsScreen.left)/cColumns)*(j+1)), coordsScreen.top - (((coordsScreen.top-coordsScreen.bottom)/cRows)*(i+1)), 0.0f);
			TextCoordsArray[(i*cColumns+j)][0] = D3DXVECTOR2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+0)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+1)+coordsTexture.top);
			TextCoordsArray[(i*cColumns+j)][1] = D3DXVECTOR2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+0)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+0)+coordsTexture.top);
			TextCoordsArray[(i*cColumns+j)][2] = D3DXVECTOR2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+1)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+0)+coordsTexture.top);
			TextCoordsArray[(i*cColumns+j)][3] = D3DXVECTOR2(((coordsTexture.right-coordsTexture.left)/cColumns)*(j+1)+coordsTexture.left, ((coordsTexture.top-coordsTexture.bottom)/cRows)*(i+1)+coordsTexture.top);
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
	srand (time(NULL));
	D3DXVECTOR2 t1, t2;
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

    // create the vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc)); ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// GPU (read only) and the CPU (write only). To update a dynamic resource, use a Map method.
    vertexBufferDesc.ByteWidth = sizeof(VERTEX_TEXTURE) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	vertexData.pSysMem = RectangleVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	comDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &comVertexBuffer);       // create the buffer filled with data

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	comDevice->CreateBuffer(&indexBufferDesc, &indexData, &comIndexBuffer);

	fontVertexBuffer = NULL;
	fontIndexBuffer = NULL;
	textureShaderViews[1] = NULL;

	VERTEX_TEXTURE LabelVertices[4] = { {D3DXVECTOR3(-0.9f,-0.25f,0.0f),D3DXVECTOR2(0.0f,1.0f)},
										{D3DXVECTOR3(-0.9f,0.25f,0.0f),D3DXVECTOR2(0.0f,0.0f)},
										{D3DXVECTOR3(0.9f,0.25f,0.0f),D3DXVECTOR2(1.0f,0.0f)},
										{D3DXVECTOR3(0.9f,-0.25f,0.0f),D3DXVECTOR2(1.0f,1.0f)}};
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
}


// this function loads and prepares the shaders
void MiniGamePicturePuzzle::InitPipeline()
{
    // load and compile the two shaders
    ID3D10Blob *VS, *PS;
	HRESULT h1 = D3DX11CompileFromFile(L"shaders_2d.hlsl", 0, 0, "TextureVertexShader", "vs_5_0", 0, 0, 0, &VS, 0, 0);
    HRESULT h2 = D3DX11CompileFromFile(L"shaders_2d.hlsl", 0, 0, "TexturePixelShader", "ps_5_0", 0, 0, 0, &PS, 0, 0);

    // encapsulate both shaders into shader objects
    comDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &comVertexShader);
    comDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &comPixelShader);

    // set the shader objects
    comDeviceContext->VSSetShader(comVertexShader, 0, 0);
    comDeviceContext->PSSetShader(comPixelShader, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC myInputLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    comDevice->CreateInputLayout(myInputLayout, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &comInputLayout);

	VS->Release(); PS->Release();

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
    HRESULT result = comDevice->CreateSamplerState(&samplerDesc, &comSamplerState);

	ID3D10Blob* FONT_VS_Buffer,*FONT_PS_Buffer;
	h1 = D3DX11CompileFromFile(L"shaders_font.hlsl", 0, 0, "FONT_VS", "vs_5_0", 0, 0, 0, &FONT_VS_Buffer, 0, 0);
    h2 = D3DX11CompileFromFile(L"shaders_font.hlsl", 0, 0, "FONT_PS", "ps_5_0", 0, 0, 0, &FONT_PS_Buffer, 0, 0);
	comDevice->CreateVertexShader(FONT_VS_Buffer->GetBufferPointer(),FONT_VS_Buffer->GetBufferSize(),NULL,&fontVertexShader);
	comDevice->CreatePixelShader(FONT_PS_Buffer->GetBufferPointer(),FONT_PS_Buffer->GetBufferSize(),NULL,&fontPixelShader);
	comDeviceContext->VSSetShader(fontVertexShader, 0, 0);
    comDeviceContext->PSSetShader(fontPixelShader, 0, 0);
	comDevice->CreateInputLayout(myInputLayout, 2, FONT_VS_Buffer->GetBufferPointer(), FONT_VS_Buffer->GetBufferSize(), &comInputLayout);

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	comDevice->CreateSamplerState(&samplerDesc,&fontAtlasSampler);
}

void MiniGamePicturePuzzle::Click(float x1, float y1)
{
	if (!flagGameFinished)
			{
				isFirstClick = !isFirstClick;
			
				RECT rect;
				float width = SCREEN_WIDTH;
				float height = SCREEN_HEIGHT;
				if(GetClientRect(hWnd, &rect))
				{
				  width = (rect.right - rect.left)*((coordsScreen.right - coordsScreen.left)/2.0f);
				  height = (rect.bottom - rect.top)*((coordsScreen.top - coordsScreen.bottom)/2.0f);
				}
				float oneColumnSize = width/cColumns;
				float oneRowSize = height/cRows;
				float firstColumnCoordX = ((float)(rect.right - rect.left))*(1.0f+coordsScreen.left)/2;
				float firstRowCoordY = ((float)(rect.bottom - rect.top))*(1.0f-coordsScreen.top)/2;
				//если кликнули не в прямоугольник - выходим
				if (!((firstColumnCoordX<=x1)&&(firstColumnCoordX+width>=x1)&&(firstRowCoordY<=y1)&&(firstRowCoordY+height>=y1)))
					return;
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
					comDeviceContext->Map(comVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubRes);  

					D3DXVECTOR2 buf[4];
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
					comDeviceContext->Unmap(comVertexBuffer, NULL);
				}
				previousCellNumber = currentCellNumber;
			}
}

bool MiniGamePicturePuzzle::IsComplete() const
{
	bool res = false;
	for (int i=0; i< vertexCount; i++)
	{
		if (StandardTextCoords[i] != RectangleVertices[i].texture)
			return res;
	}
	res = true;
	return res;
}

void MiniGamePicturePuzzle::Render() const
{
	// Set shader texture resource in the pixel shader.
	comDeviceContext->PSSetShaderResources(0, 1, &textureShaderViews[0]); //APPLYING TEXTURE
	// Set the vertex input layout.
	comDeviceContext->IASetInputLayout(comInputLayout);

	// clear the back buffer to a deep blue
    comDeviceContext->ClearRenderTargetView(comBackBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

	// select which vertex buffer to display
    UINT stride = sizeof(VERTEX_TEXTURE);
    UINT offset = 0;
	
    comDeviceContext->IASetVertexBuffers(0, 1, &comVertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	comDeviceContext->IASetIndexBuffer(comIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	comDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	comDeviceContext->VSSetShader(comVertexShader, NULL, 0);
	comDeviceContext->PSSetShader(comPixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	comDeviceContext->PSSetSamplers(0, 1, &comSamplerState);

    // draw the vertex buffer to the back buffer
	comDeviceContext->DrawIndexed(indexCount, 0, 0);
    //comDeviceContext->Draw(indexCount, 0); // draw 3 vertices, starting from vertex 0

	if (IsComplete())
	{
		flagGameFinished = true;
		comDeviceContext->VSSetShader(fontVertexShader,0,0);
		comDeviceContext->PSSetShader(fontPixelShader,0,0);
		comDeviceContext->PSSetShaderResources(0, 1, &textureShaderViews[1]);
		comDeviceContext->IASetVertexBuffers(0, 1, &fontVertexBuffer, &stride, &offset);
		comDeviceContext->IASetIndexBuffer(fontIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		comDeviceContext->PSSetSamplers(0,1,&fontAtlasSampler);
		comDeviceContext->DrawIndexed(6, 0, 0);
	}
	// switch the back buffer and the front buffer
    comSwapChain->Present(0, 0); // Present as fast as possible.

	//comDeviceContext->VSGetShader
}

void Render(const Rect& screenCoords, int textureId, const Rect& textureCoord)
{
	
}


