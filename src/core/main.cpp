
//
//
// MAKING THIS COMPILE:
//
// export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
//
// g++ -Wall -pedantic `sdl-config --cflags` `freetype-config --cflags` main.cpp -o ps -L. -lLevGC -lSDL -lSDL_image -lSDL_mixer -lfreetype -lGL /usr/local/lib/libftgl.a
//
//

#include <libLevGC/src/core/GameApplication.h>
#include <libLevGC/src/core/CoreInterface.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/core/cast.h>
#include <libLevGC/src/utilities/LogManager.h>
#include <iostream>
#include <libLevGC/src/etc/Settings.h>
#include "Scores.h"
#include <time.h>


// global
std::string application_path = "./";


int main( int argc, char* argv[] ) {

	// print debug output?
#ifdef OUTPUT_DEBUG
	LGC::LogMgr::DisableDebugOutput( false );
#else
	LGC::LogMgr::DisableDebugOutput( true );
#endif

	// get application path
	application_path = argv[0];
	int last_slash = application_path.find_last_of("/");
	application_path.erase( last_slash + 1 );

	// load settings and scores file
	LGC::Settings->LoadSettings( application_path + "resource/options.txt" );
	Scores->LoadScores( application_path + "resource/scores.txt" );

	//get args:
	bool fullscreen = LGC::Settings->GetSetting<bool>("FullScreen");
	bool use_sound = LGC::Settings->GetSetting<bool>("UseSound");
	int colorDepth = LGC::Settings->GetSetting<int>("ColorDepth");
	for (int x = 1; x < argc; x++) {
		if ( strcmp(argv[x],"--fullscreen") == 0 ) 		{ fullscreen = true; continue; }
		if ( strcmp(argv[x],"--no_sound") == 0 ) 		{ use_sound = false; continue; }
		if ( strcmp(argv[x], "--color_depth") == 0) 		{ colorDepth = atoi(argv[x+1]); continue; }
		if ( strcmp(argv[x], "--help") == 0 || strcmp(argv[x], "-h") == 0 ) {
			std::cout
				<< "Gameplay instructions are contained within the game itself." << std::endl
				<< "Command line flags:" << std::endl
				<< "--fullscreen (toggles full screen option)" << std::endl
				<< "--color_depth [16|32] (manually sets the screen color bit depth)" << std::endl
				<< "--no_sound (disables all audio)" << std::endl;
			return 0;
			}
		if ( strcmp(argv[x], "--version") == 0 || strcmp(argv[x], "-v") == 0 ) {
			std::cout << "Ortho version 1.0.0" << std::endl;
			return 0;
			}
		}

	srand((unsigned int)time(NULL)); // important!

	int xres = LGC::Settings->GetSetting<int>("XResolution");
	int yres = LGC::Settings->GetSetting<int>("YResolution");
	LGC::App->Init( xres, yres, colorDepth, fullscreen, use_sound, application_path );
	LGC::App->SetWindowTitle("Ortho");
	LGC::App->SetIcon( application_path + "resource/graphics/icon.bmp" );

	if ( LGC::Settings->GetSetting<bool>("DrawFPS") ) { LGC::App->SetDrawFPS( true ); }
	if ( LGC::Settings->GetSetting<bool>("DrawFPSGraph") ) { LGC::App->SetDrawFPSGraph( true ); }
	if ( LGC::Settings->GetSetting<float>("SoundFXVolume") ) {
		LGC::Core->SetMasterFXVolume( LGC::Settings->GetSetting<float>("SoundFXVolume") );
		}
	if ( LGC::Settings->GetSetting<float>("MusicVolume") ) {
		LGC::Core->SetMasterMusicVolume( LGC::Settings->GetSetting<float>("MusicVolume") );
		}

	LGC::RM->LoadResourceXMLFile( application_path + "resource/resources.xml" );

	if ( LGC::Settings->GetSetting<bool>("ShowSplash") ) {
		LGC::Core->ChangeGameState( "SPLASH" );
		}
	else {
		LGC::Core->ChangeGameState( "TITLE_SCREEN" );
		}

	LGC::App->RunGameLoop();

	LGC::App->Shutdown();

	LGC::Settings->DumpSettings( application_path + "resource/options.txt" );

	LGC::RM->DebugReportLoadedResources();

	// no need to dump scores - works automatically when adding a new score

	return( 0 );
	}




//


