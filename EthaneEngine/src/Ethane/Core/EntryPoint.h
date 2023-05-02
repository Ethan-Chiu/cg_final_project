#pragma once
#include "Ethane/Core/Application.h"

// #ifdef ETH_PLATFORM_WINDOWS

extern Ethane::Application* Ethane::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) {

    Ethane::Log::Init();
    
    ETH_PROFILE_BEGIN_SESSION("Startup", "EthaneProfile-Startup.json");
    auto app = Ethane::CreateApplication({ argc, argv });
    ETH_PROFILE_END_SESSION();

    // ETH_PROFILE_BEGIN_SESSION("Runtime", "EthaneProfile-Runtime.json");
    app->Run();
    // ETH_PROFILE_END_SESSION();

    ETH_PROFILE_BEGIN_SESSION("Shutdown", "EthaneProfile-Shutdown.json");
    delete app;
    ETH_PROFILE_END_SESSION();
}


// #endif 
