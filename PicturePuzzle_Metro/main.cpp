// Include the precompiled headers
#include "pch.h"
#include "minigame_picturepuzzle2.h"

#include <sstream>
//#include <math.h>

// Use some common namespaces to simplify the code
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

// the class definition for the core "framework" of our app
ref class Game sealed : public IFrameworkView
{
	MiniGamePicturePuzzle* PuzzleGame;
	bool WindowClosed;
public:
    // some functions called by Windows
    virtual void Initialize(CoreApplicationView^ AppView)
    {
		WindowClosed = false;
        // set the OnActivated function to handle to Activated "event"
        AppView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &Game::OnActivated);
    }
    virtual void SetWindow(CoreWindow^ Window)
	{
	    Window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &Game::PointerPressed);
	}
    virtual void Load(String^ EntryPoint) {}
    virtual void Run()
	{
		PuzzleGame = new MiniGamePicturePuzzle;
		// Initialize and run the system object.
		//PuzzleGame->hWnd = hWnd;
		PuzzleGame->Initialize();

		// Obtain a pointer to the window
        CoreWindow^ Window = CoreWindow::GetForCurrentThread();

		while(!WindowClosed)
        {
        // Run ProcessEvents() to dispatch events
			Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			PuzzleGame->Render();
		}
	}
    virtual void Uninitialize() {}
        
    // an "event" that is called when the application window is ready to be activated
	//The first parameter, CoreApplicationView^, is the same parameter found in the Initialize() function.
	//The second parameter, IActivatedEventArgs^, is a parameter that will give us useful information about the circumstances under which the function was called (remember, we aren't calling it).
    void OnActivated(CoreApplicationView^ CoreAppView, IActivatedEventArgs^ Args)
    {
        CoreWindow^ Window = CoreWindow::GetForCurrentThread();
        Window->Activate();
    }

	void PointerPressed(CoreWindow^ Window, PointerEventArgs^ Args)
    {
		float x1 = Args->CurrentPoint->Position.X;
		float y1 = Args->CurrentPoint->Position.Y;

		std::wstringstream WStrStream;
		WStrStream << L"x:" << x1 << L",y:" << y1;
		Platform::String^ plStr = ref new Platform::String(WStrStream.str().c_str());
		//MessageBox(hWnd,WStrStream.str().c_str(),L"Element coords",MB_OK);
        MessageDialog Dialog(plStr, "Notice!");
        Dialog.ShowAsync();
    }
};


// the class definition that creates an instance of our core framework class
ref class AppSource sealed : IFrameworkViewSource // a function that creates an instance of App
{
public:
    virtual IFrameworkView^ CreateView()
    {
        return ref new Game();    // create an App class and return it
    }
};

[MTAThread]    // define main() as a multi-threaded-apartment function - forces app to use MT

// the starting point of all programs
int main(Array<String^>^ args)
{
    CoreApplication::Run(ref new AppSource());    // create and run a new AppSource class
    return 0;
}