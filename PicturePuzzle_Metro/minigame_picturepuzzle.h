#include "pch.h"
#pragma once

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;

class MiniGamePicturePuzzle 
{
public:
    void Initialize();
    void Update();
    void Render();
};