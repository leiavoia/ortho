#ifndef PS_GAME_STATES_H
#define PS_GAME_STATES_H


#include <string>

#include <SDL/SDL.h>

#include <libLevGC/src/core/Sequencer.h>
#include <libLevGC/src/core/Texture.h>
#include <libLevGC/src/core/Sound.h>
// #include <libLevGC/src/view/Material.h>
// #include <libLevGC/src/view/Light.h>
#include <libLevGC/src/core/GameState.h>

#include <libLevGC/src/UI/widgets/BasicWidget.h>
#include <libLevGC/src/UI/widgets/Button.h>
#include <libLevGC/src/UI/widgets/Label.h>
#include <libLevGC/src/UI/widgets/TextBox.h>
#include <libLevGC/src/UI/widgets/DropBox.h>
#include <libLevGC/src/UI/widgets/Slider.h>
#include "../objects/ShapeBox.h"
#include "../UI/ScoreWidget.h"
#include "../UI/MultiplierWidget.h"
#include "../UI/CollectablesWidget.h"
#include "../UI/StatusEffectWidget.h"
#include "../UI/TextBlock.h"



// predeclare
class Cursor;


//////////////////////////////
    using namespace LGC;
//////////////////////////////




// -------------------------------------------------------------------------------

class GameState_Title: public GameState, public has_slots<> {
public:

	GameState_Title();
	~GameState_Title();

	void Activate();
// 	void PauseGame();
	void RegisterObject( LGC::PollingObject* obj, int layer = 0 );
	void Cycle( float incoming_time );
	void AcceptInputEvent( SDL_Event& e );
	void InternalChangeGameState( const std::string& s );

	// events
	void HandlePressPlay( Button* obj );
	void HandlePressOptions( Button* obj );
	void HandlePressScores( Button* obj );
	void HandlePressTutorial( Button* obj );
	void HandlePressAbout( Button* obj );
	void HandlePressQuit( Button* obj );

private:

	LGC::Sequencer* sequencer;
	LGC::Texture bg;

	LGC::Font button_font;

	LGC::BasicWidget* window;
	LGC::Button* play_button;
	LGC::Button* options_button;
	LGC::Button* about_button;
	LGC::Button* tutorial_button;
	LGC::Button* scores_button;
	LGC::Button* quit_button;

	std::vector<Button*> selectron;
	unsigned int selectron_index;

	ColorQuad selected_color;
	ColorQuad unselected_color;
	};


// gamestate_title factory:
class GameState_Title_Factory : public GameStateFactory {
	public:
		~GameState_Title_Factory() {}
		GameState_Title_Factory() : GameStateFactory("TITLE_SCREEN") {}
	protected:
		inline GameState* CreateNew() { return new GameState_Title(); }
		static const GameState_Title_Factory auto_register_me;
	};






///////////////////// SPLASH /////////////////////////
class GameState_Splash: public GameState, public has_slots<> {
public:

	GameState_Splash();
	~GameState_Splash();

	void Activate();
	void RegisterObject( LGC::PollingObject* obj, int layer = 0 );
	void Cycle( float incoming_time );
	void AcceptInputEvent( SDL_Event& e );
// 	void InternalChangeGameState( const std::string& s );

private:
	LGC::Sequencer* sequencer;
	float timer;
	};


// gamestate_title factory:
class GameState_Splash_Factory : public GameStateFactory {
	public:
		~GameState_Splash_Factory() {}
		GameState_Splash_Factory() : GameStateFactory("SPLASH") {}
	protected:
		inline GameState* CreateNew() { return new GameState_Splash(); }
		static const GameState_Splash_Factory auto_register_me;
	};





///////////////////// ABOUT /////////////////////////

class GameState_About: public GameState, public has_slots<> {
public:

	GameState_About();
	~GameState_About();

	void Activate();
	void RegisterObject( LGC::PollingObject* obj, int layer = 0 );
	void Cycle( float incoming_time );
	void AcceptInputEvent( SDL_Event& e );

	// events
	void HandlePressBack( Button* obj );

private:

	LGC::Sequencer* sequencer;
	LGC::Texture bg;

	LGC::Font font;
	LGC::Font title_font;

	LGC::BasicWidget* window;
	LGC::Button* back_button;
	};


// gamestate_title factory:
class GameState_About_Factory : public GameStateFactory {
	public:
		~GameState_About_Factory() {}
		GameState_About_Factory() : GameStateFactory("ABOUT") {}
	protected:
		inline GameState* CreateNew() { return new GameState_About(); }
		static const GameState_About_Factory auto_register_me;
	};




///////////////////// SCOREBOARD /////////////////////////

class GameState_Scores: public GameState, public has_slots<> {
public:

	GameState_Scores();
	~GameState_Scores();

	void Activate();
	void RegisterObject( LGC::PollingObject* obj, int layer = 0 );
	void Cycle( float incoming_time );
	void AcceptInputEvent( SDL_Event& e );

	// events
	void HandlePressBack( Button* obj );

private:

	LGC::Sequencer* sequencer;
	LGC::Texture bg;

	LGC::Font font;
	LGC::Font title_font;

	LGC::BasicWidget* window;
	LGC::BasicWidget* score_pane;
	LGC::Button* back_button;
	};


// gamestate_title factory:
class GameState_Scores_Factory : public GameStateFactory {
	public:
		~GameState_Scores_Factory() {}
		GameState_Scores_Factory() : GameStateFactory("SCOREBOARD") {}
	protected:
		inline GameState* CreateNew() { return new GameState_Scores(); }
		static const GameState_Scores_Factory auto_register_me;
	};




///////////////////// HOW TO PLAY /////////////////////////

class GameState_Howto: public GameState, public has_slots<> {
public:

	GameState_Howto();
	~GameState_Howto();

	void Activate();
	void RegisterObject( LGC::PollingObject* obj, int layer = 0 );
	void Cycle( float incoming_time );
	void AcceptInputEvent( SDL_Event& e );

	// events
	void HandlePressBack( Button* obj );

private:

	LGC::Sequencer* sequencer;
	LGC::Texture bg;

	LGC::Font font;
	LGC::Font title_font;

	LGC::BasicWidget* window;
	LGC::Button* back_button;
	};


// gamestate_title factory:
class GameState_Howto_Factory : public GameStateFactory {
	public:
		~GameState_Howto_Factory() {}
		GameState_Howto_Factory() : GameStateFactory("HOWTO") {}
	protected:
		inline GameState* CreateNew() { return new GameState_Howto(); }
		static const GameState_Howto_Factory auto_register_me;
	};





///////////////////// CONFIG /////////////////////////

class GameState_Config: public GameState, public has_slots<> {
public:

	GameState_Config();
	~GameState_Config();

	void Activate();
	void RegisterObject( LGC::PollingObject* obj, int layer = 0 );
	void Cycle( float incoming_time );
	void AcceptInputEvent( SDL_Event& e );

	// events
	void HandlePressBack( Button* obj );
	void HandleCallsign( TextBox* obj );
	void HandleCallsignEnter( TextBox* obj );
	void HandleDifficulty( Button* obj );
	void HandleMode( Button* obj );
	void HandleSoundVol( float value, Slider* obj );
	void HandleMusicVol( float value, Slider* obj );

private:

	LGC::Sequencer* sequencer;
	LGC::Texture bg;

	LGC::Font font;
	LGC::Font title_font;

	LGC::BasicWidget* window;
	LGC::Button* back_button;

	LGC::TextBox* callsign;

// 	LGC::DropBox* difficulty;
// 	LGC::DropBox* mode;
// 	LGC::HSlider* music_vol;

	LGC::HSlider* sound_vol;

	LGC::Button* mode0;
	LGC::Button* mode1;

	LGC::Button* diff0;
	LGC::Button* diff1;
	LGC::Button* diff2;
	LGC::Button* diff3;

	TextBlock* explain_pane;

	LGC::Label* diff_label;
	LGC::Label* mode_label;
	LGC::Label* callsign_label;
	LGC::Label* vol_label;

	LGC::Sound sfx;

	unsigned int row_index;

	ColorQuad selected_color;
	ColorQuad unselected_color;

	unsigned int diff_index;
	unsigned int mode_index;

	void CompileText();
	};


// gamestate_title factory:
class GameState_Config_Factory : public GameStateFactory {
	public:
		~GameState_Config_Factory() {}
		GameState_Config_Factory() : GameStateFactory("CONFIG") {}
	protected:
		inline GameState* CreateNew() { return new GameState_Config(); }
		static const GameState_Config_Factory auto_register_me;
	};





///////////////////// PLAY /////////////////////////

class GameState_Play: public GameState, public has_slots<> {
public:

	GameState_Play();
	~GameState_Play();

	void Activate();
	void PauseGame( bool x );
	void RegisterObject( LGC::PollingObject* obj, int layer = 0 );
	void Cycle( float incoming_time );
	void AcceptInputEvent( SDL_Event& e );
	void InternalChangeGameState( const std::string& s );
  	static void ReserveResources();
	static void DumpResources();
	void SendMessage( const std::string& s );

private:
	Cursor* cursor;
	LGC::Sequencer* sequencer;
	static LGC::Font label_font;
	static LGC::Font gameover_font;
	LGC::Label* timer_label;
	ScoreWidget* score_label;
	ScoreWidget* kills_label;
	MultiplierWidget* mult_label;
	ScoreWidget* color_chain_label;
	CollectablesWidget* collectables;
	StatusEffectWidget* status_effects;
	static std::map< std::string, Texture > textures;
	static std::map< std::string, Sound > sfx;

	float havoc_timeout;
	std::vector< ShapeBox* > shapeboxes;

	bool do_shapes;

	float game_over_time;

	float reverse_controls_timeout;
	float slowdown_timeout;

	ShapeType last_chain_color;
	int color_chains;

	float recycle_time;
	float countie;
	int last_file;
	int last_file2;
	int skip_last_sides; // AKA "play nice"
	int shapes_to_add;
	float havoc_mult;

	std::vector< bool > gameover_trips;

	float last_time;

	LGC::Label* gameover_label;

	float GetAwesomeness();
	float final_awesomeness;

	float speed_mod;
	};





// gamestate_title factory:
class GameState_Play_Factory : public GameStateFactory {
	public:
		~GameState_Play_Factory() {}
		GameState_Play_Factory() : GameStateFactory("Play") {}
	protected:
		inline GameState* CreateNew() { return new GameState_Play(); }
		static const GameState_Play_Factory auto_register_me;

	};









#endif




