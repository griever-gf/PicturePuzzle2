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

        RenderFrame();
    }

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

	InitPipeline();
    InitGraphics();
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
    comSwapChain->Release();
	comBackBuffer->Release();
    comDevice->Release();
    comDeviceContext->Release();
}


// this is the function used to render a single frame
void RenderFrame(void)
{
    // clear the back buffer to a deep blue
    comDeviceContext->ClearRenderTargetView(comBackBuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

	// select which vertex buffer to display
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    comDeviceContext->IASetVertexBuffers(0, 1, &comVertexBuffer, &stride, &offset);

    // select which primtive type we are using
	comDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw the vertex buffer to the back buffer
    comDeviceContext->Draw(3, 0); // draw 3 vertices, starting from vertex 0

    // switch the back buffer and the front buffer
    comSwapChain->Present(0, 0);
}

// this is the function that creates the shape to render
void InitGraphics()
{
    // create a triangle using the VERTEX struct
    VERTEX OurVertices[] =
    {
        {0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
        {0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
        {-0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
    };


    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    comDevice->CreateBuffer(&bd, NULL, &comVertexBuffer);       // create the buffer


    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    comDeviceContext->Map(comVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
    comDeviceContext->Unmap(comVertexBuffer, NULL);                                      // unmap the buffer
}


// this function loads and prepares the shaders
void InitPipeline()
{
    // load and compile the two shaders
    ID3D10Blob *VS, *PS;
    HRESULT hhh = D3DX11CompileFromFile(L"shaders.hlsl", 0, 0, "VShader", "vs_5_0", 0, 0, 0, &VS, 0, 0);
    HRESULT h2 = D3DX11CompileFromFile(L"shaders.hlsl", 0, 0, "PShader", "ps_5_0", 0, 0, 0, &PS, 0, 0);

    // encapsulate both shaders into shader objects
    comDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &comVertexShader);
    comDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &comPixelShader);

    // set the shader objects
    comDeviceContext->VSSetShader(comVertexShader, 0, 0);
    comDeviceContext->PSSetShader(comPixelShader, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    comDevice->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &comInputLayout);
    comDeviceContext->IASetInputLayout(comInputLayout);
}