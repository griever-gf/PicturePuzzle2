#include "main.h"

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // the handle for the window, filled by a function
    HWND hWnd;
    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"PicturePuzzle";

    // register the window class
    RegisterClassEx(&wc);

	//RECT wr = {0, 0, 800, 600};
    //AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(NULL,
                          L"PicturePuzzle",    // name of the window class
                          L"Picture Puzzle (test task)",   // title of the window
                          WS_OVERLAPPEDWINDOW,    // window style
                          300,    // x-position of the window
                          300,    // y-position of the window
                          SCREEN_WIDTH, SCREEN_HEIGHT,    // set window to new resolution
                          NULL,    // we have no parent window, NULL
                          NULL,    // we aren't using menus, NULL
                          hInstance,    // application handle
                          NULL);    // used with multiple windows, NULL

    // display the window on the screen
    ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
    InitD3D(hWnd);

    // this struct holds Windows event messages
    MSG msg = {0};

    // enter the main loop:
	while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT)
                break;
        }

        RenderFrame(hWnd);
    }

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::wstringstream WStrStream;
    // sort through and find what code to run for the message given
    switch(message)
    {
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                // close the application entirely
                PostQuitMessage(0);
                return 0;
            } break;
			
		case WM_LBUTTONDOWN: //on mouse click
			if (!flagGameFinished)
			{
				isFirstClick = !isFirstClick;
			
				RECT rect;
				int width = SCREEN_WIDTH;
				int height = SCREEN_HEIGHT;
				if(GetClientRect(hWnd, &rect))
				{
				  width = rect.right - rect.left;
				  height = rect.bottom - rect.top;
				}
				int x = GET_X_LPARAM(lParam) / (width/numColumns);
				int y = GET_Y_LPARAM(lParam) / (height/numRows);

				currentCellNumber = x + y*numColumns;

				//WStrStream << "x:" << x << ",y:" << y << " ,cellnum:" << currentCellNumber;
				//MessageBox(hWnd,WStrStream.str().c_str(),L"Element coords",MB_OK);
				//isFirstClick = true;
				if (!isFirstClick) //if second click - swap texture coordinates for rectangle regions
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

			break;
			//D3D11_MAP_READ_WRITE
			//D3D11_MAP_WRITE_DISCARD

			//VERTEX_TEXTURE EmptyVertices[vertexCount];
			//ZeroMemory( &EmptyVertices, sizeof(EmptyVertices) );
			//VERTEX_TEXTURE* mappedData = reinterpret_cast<VERTEX_TEXTURE*>(mappedSubRes.pData);

			/*D3D11_BOX srcBox;
			//srcBox.left = sizeof(VERTEX_TEXTURE)*4+3;
			//srcBox.right = sizeof(VERTEX_TEXTURE)*4+5;
			srcBox.left = 0;
			srcBox.right = 20;
			srcBox.top = 0;
			srcBox.bottom = 1;
			srcBox.front = 0;
			srcBox.back = 1;*/

			//comDeviceContext->CopySubresourceRegion(mappedSubRes, 0, NULL, comVertexBuffer, 0, 0);
			
			//UINT stride = sizeof(VERTEX_TEXTURE);
			//UINT offset = 0;
			//comDeviceContext->IASetVertexBuffers(0, 1, &comVertexBuffer, &stride, &offset);
			//this->_context->PSSetConstantBuffers(0, 1, &pShaderBuffer);
			//ID3D11Buffer			*tmpVertexBuffer; 
			//comDeviceContext->CopyResource(tmpVertexBuffer, comVertexBuffer);

			/*ID3D11Buffer* tmpBuffer;
			//comDeviceContext->CopySubresourceRegion(resource, 0, 0, 0, 0, comVertexBuffer, 0, &srcBox);

			D3D11_BUFFER_DESC vertexBufferDesc;
			D3D11_SUBRESOURCE_DATA vertexData;
			VERTEX_TEXTURE EmptyVertices[vertexCount];

			vertexBufferDesc.Usage = D3D11_USAGE_STAGING;                
			//vertexBufferDesc.ByteWidth = sizeof(VERTEX_TEXTURE) * vertexCount;             // size is the VERTEX struct * 3
			vertexBufferDesc.BindFlags = 0;       // use as a vertex buffer
			//vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
			//vertexBufferDesc.CPUAccessFlags  = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			vertexBufferDesc.CPUAccessFlags  = D3D11_CPU_ACCESS_READ;
			//D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

			vertexData.pSysMem = EmptyVertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			
			comDevice->CreateBuffer(&vertexBufferDesc, NULL, &tmpBuffer);
			comDeviceContext->CopyResource(tmpBuffer, comVertexBuffer);

			HRESULT h = comDeviceContext->Map(tmpBuffer, NULL, D3D11_MAP_READ_WRITE, NULL, &mappedSubRes);*/

			/*tmpVertexData.pSysMem = RectangleVertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			VERTEX_TEXTURE *dataPtr = (VERTEX_TEXTURE*)vertexData.pSysMem;
			VERTEX_TEXTURE tmp = dataPtr[0];
			dataPtr[10] = tmp;

			memcpy(mappedSubRes.pData, &vertexData[0], sizeof(vertexData));*/

			/*D3D11_BOX sourceRegion;
			sourceRegion.left = 0;
			sourceRegion.right = sizeof(VERTEX_TEXTURE);
			sourceRegion.top = 0;
			sourceRegion.bottom = 1;
			sourceRegion.front = 0;
			sourceRegion.back = 1;
			//ID3D11Resource dummyRes;
			ID3D11Buffer *tmp_buffer;

			comDeviceContext->CopySubresourceRegion(tmp_buffer, 0+sizeof(VERTEX_TEXTURE), 0, 0, 0, comVertexBuffer, 0, &sourceRegion);*/
			 //HRESULT hr = s_d3dContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
 
			//VERTEX_TEXTURE *dataPtr;
			//D3D11_SUBRESOURCE_DATA

			/*float *vertices = (float *) mappedSubRes.pData;
				data = (float *)comVertexBuffer.pData[i];
			int len = s_vertexBuffers.pStrides[i];
			  if (i != VERTEX_BUFFER::COLOR)
			  {
				  len *= total;
			  }
			  memcpy(vertices, data, len);*/

			/*
			VERTEX_TEXTURE *dataPtr;
			dataPtr = (VERTEX_TEXTURE *)ms.pData[2];
			//dataPtr[0]->

			dataPtr = (VERTEX_TEXTURE *)(ms.pData+sizeof(VERTEX_TEXTURE));
			dataPtr->
			dataPtr->pBuffers[i] = s_vertexBuffers.pBuffers[i].Get();
			dataPtr->pSizes[i] = s_vertexBuffers.pSizes[i];
			dataPtr->pStrides[i] = s_vertexBuffers.pStrides[i];
			if (i != VERTEX_BUFFER::COLOR) 
			{
			   dataPtr->pStrides[i] *= total;
			}
			memcpy(dataPtr->pData[i], s_vertexBuffers.pData[i], dataPtr->pStrides[i]);
			*/

    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}

// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
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
	//сначала инициализируем буфферы, а уже потом загружаем текстуру
	HRESULT result = D3DX11CreateShaderResourceViewFromFile(comDevice, L"beyond.bmp", NULL, NULL, &comTextureShaderView, NULL);
}

// this is the function that cleans up Direct3D and COM
void CleanD3D()
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
}

// this is the function that creates the shape to render
void InitBuffers()
{
	comVertexBuffer = NULL;
	comIndexBuffer = NULL; 
	comTexture = NULL;
	comTextureShaderView = NULL;

	D3DXVECTOR3 ScreenCoordsArray[vertexCount/4][4];
	D3DXVECTOR2 TextCoordsArray[vertexCount/4][4];
	unsigned long indices[indexCount];
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numColumns; j++)
		{
			ScreenCoordsArray[(i*numColumns+j)][0] = D3DXVECTOR3(-1.0f + ((2.0f/numColumns)*j    ), 1.0f - ((2.0f/numRows)*(i+1)), 0.0f);
			ScreenCoordsArray[(i*numColumns+j)][1] = D3DXVECTOR3(-1.0f + ((2.0f/numColumns)*j    ), 1.0f - ((2.0f/numRows)*i    ), 0.0f);
			ScreenCoordsArray[(i*numColumns+j)][2] = D3DXVECTOR3(-1.0f + ((2.0f/numColumns)*(j+1)), 1.0f - ((2.0f/numRows)*i    ), 0.0f);
			ScreenCoordsArray[(i*numColumns+j)][3]= D3DXVECTOR3(-1.0f + ((2.0f/numColumns)*(j+1)), 1.0f - ((2.0f/numRows)*(i+1)), 0.0f);
			TextCoordsArray[(i*numColumns+j)][0] = D3DXVECTOR2((1.0f/numColumns)*(j+0), (1.0f/numRows)*(i+1));
			TextCoordsArray[(i*numColumns+j)][1] = D3DXVECTOR2((1.0f/numColumns)*(j+0), (1.0f/numRows)*i    );
			TextCoordsArray[(i*numColumns+j)][2] = D3DXVECTOR2((1.0f/numColumns)*(j+1), (1.0f/numRows)*i    );
			TextCoordsArray[(i*numColumns+j)][3] = D3DXVECTOR2((1.0f/numColumns)*(j+1), (1.0f/numRows)*(i+1));
			indices[(i*numColumns+j)*6  ] = (i*numColumns+j)*4;
			indices[(i*numColumns+j)*6+1] = (i*numColumns+j)*4+1;
			indices[(i*numColumns+j)*6+2] = (i*numColumns+j)*4+3;
			indices[(i*numColumns+j)*6+3] = (i*numColumns+j)*4+3;
			indices[(i*numColumns+j)*6+4] = (i*numColumns+j)*4+1;
			indices[(i*numColumns+j)*6+5] = (i*numColumns+j)*4+2;
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
}


// this function loads and prepares the shaders
void InitPipeline()
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
    //comDeviceContext->IASetInputLayout(comInputLayout); //moved to render function

	VS->Release(); PS->Release();

	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
    //samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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
}

// this is the function used to render a single frame
void RenderFrame(HWND hWnd)
{
	// Set shader texture resource in the pixel shader.
	comDeviceContext->PSSetShaderResources(0, 1, &comTextureShaderView); //APPLYING TEXTURE
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

    // switch the back buffer and the front buffer
    comSwapChain->Present(0, 0); // Present as fast as possible.

	if (IsGameComplete())
	{
		flagGameFinished = true;
		MessageBox(hWnd,L"Game Complete!",L"Element coords",MB_OK);
	}
	
}

bool IsGameComplete()
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