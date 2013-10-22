#include "minigame_picturepuzzle.h"

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

MiniGamePicturePuzzle* PuzzleGame;

// the class definition for the core "framework" of our app
ref class Game sealed : public IFrameworkView
{
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
		#if !defined(__cplusplus_winrt)
			PuzzleGame->hWnd = hWnd;
		#endif
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
		PuzzleGame->Click(Args->CurrentPoint->Position.X, Args->CurrentPoint->Position.Y);
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