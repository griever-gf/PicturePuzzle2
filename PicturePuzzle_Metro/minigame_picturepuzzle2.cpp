#include "pch.h"
#include "minigame_picturepuzzle2.h"

ComPtr<ID3D11DeviceContext1> comDeviceContext;

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
	D3D_FEATURE_LEVEL levelFeature ;//= D3D_FEATURE_LEVEL_9_1;
	// Create the device and device context objects
    HRESULT h =  D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &dev11, nullptr, &devcon11);
    
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

    dxgiFactory->CreateSwapChainForCoreWindow(
		comDevice.Get(),                                  // address of the device
        reinterpret_cast<IUnknown*>(Window),        // address of the window
        &tempSwapChain,                                       // address of the swap chain description
        nullptr,                                    // advanced
        &comSwapChain);  
}

// this is the function that creates the shape to render
void MiniGamePicturePuzzle::InitBuffers()
{
	
}


// this function loads and prepares the shaders
void MiniGamePicturePuzzle::InitPipeline()
{
   
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
	// switch the back buffer and the front buffer
    comSwapChain->Present(1, 0);
}

void Render(const Rect& screenCoords, int textureId, const Rect& textureCoords)
{
	
}


