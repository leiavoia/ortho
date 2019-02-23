#include "GameStates.h"

#include <libLevGC/src/core/CoreInterface.h>
#include <libLevGC/src/core/InputHandler.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/utilities/LogManager.h>
#include <libLevGC/src/UI/widgets/FaderMessageBox.h>

#include <libLevGC/src/fx/ParticleSource.h>
#include <libLevGC/src/fx/backgrounds/BackgroundImage.h>
#include <libLevGC/src/fx/transitions/Fader.h>
#include <libLevGC/src/fx/light/LightFlare.h>
#include <libLevGC/src/objects/PollingObject.h>
#include <libLevGC/src/core/GameApplication.h>
#include <libLevGC/src/core/DrawingTools.h>
#include <libLevGC/src/core/cast.h>
#include <libLevGC/src/core/RenderingLayer.h>
#include <libLevGC/src/core/extra_math.h>
#include <libLevGC/src/etc/ColorCurve.h>
#include <libLevGC/src/etc/Settings.h>
#include <libLevGC/src/etc/Commaize.h>
#include <libLevGC/src/core/cast.h>
#include "Scores.h"
#include <cctype>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <GL/gl.h>
#include <math.h>
#include <libLevGC/src/fx/light/LightFlare.h>
#include "../UI/WidgetMover.h"
#include "../objects/Cursor.h"



//////////////////////////////
    using namespace LGC;
//////////////////////////////


GameState_Title::GameState_Title() :
	sequencer( new LGC::Sequencer() ),
	selectron_index(0),
	selected_color( ColorQuad(0.35f,0.9f,0.5f,1.0f) ),
	unselected_color( ColorQuad(1,1,1,1) )
	{
	Renderer->Ortho(true);
	button_font = RM->GetFont("TitleButtonFont", "GameState_Title");
	}


void GameState_Title::Activate() {

	SDL_ShowCursor( SDL_ENABLE );

	sequencer->Register( new BackgroundImage("TitleBG", NULL, LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), BG_SCROLLMODE_STRETCH), 0 );

 	CrossFadeTransition* trans = new CrossFadeTransition(sequencer);
 	trans->SetFadeTime(0.4f);
	sequencer->Register( trans, 4 );

	window = new LGC::BasicWidget( 0, 0, App->ScreenWidth(), App->ScreenHeight(), NULL );
	window->SetBorderWidth(0);
	window->SetBGColor( ColorQuad(0,0,0,0) );
	window->PushModal(); // REQUIRED TO RECEIVE INPUT
	sequencer->Register( window, 1 );

	play_button = 		new LGC::Button( 250, 250, 300, 30, window, "PLAY" );
	options_button = 	new LGC::Button( 250, 290, 300, 30, window, "SETUP" );
	scores_button = 	new LGC::Button( 250, 330, 300, 30, window, "SCOREBOARD" );
	tutorial_button = 	new LGC::Button( 250, 370, 300, 30, window, "HOW TO PLAY" );
	about_button = 	new LGC::Button( 250, 410, 300, 30, window, "ABOUT ORTHO" );
	quit_button = 		new LGC::Button( 250, 450, 300, 30, window, "QUIT" );

	selectron.push_back(play_button);
	selectron.push_back(options_button);
	selectron.push_back(scores_button);
	selectron.push_back(tutorial_button);
	selectron.push_back(about_button);
	selectron.push_back(quit_button);

	play_button->SetTextColor( selected_color );

	// BG color
	play_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	options_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	scores_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	tutorial_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	about_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	quit_button->SetBGColor( ColorQuad(0,0,0,0.5f) );

	// border color
	play_button->SetBorderColor( ColorQuad(0,0,0,1) );
	options_button->SetBorderColor( ColorQuad(0,0,0,1) );
	scores_button->SetBorderColor( ColorQuad(0,0,0,1) );
	tutorial_button->SetBorderColor( ColorQuad(0,0,0,1) );
	about_button->SetBorderColor( ColorQuad(0,0,0,1) );
	quit_button->SetBorderColor( ColorQuad(0,0,0,1) );

	// font
	play_button->SetFont( button_font );
	options_button->SetFont( button_font );
	scores_button->SetFont( button_font );
	tutorial_button->SetFont( button_font );
	about_button->SetFont( button_font );
	quit_button->SetFont( button_font );

	// events
	play_button->signal_button_down.connect( this, &GameState_Title::HandlePressPlay );
	options_button->signal_button_down.connect( this, &GameState_Title::HandlePressOptions );
	scores_button->signal_button_down.connect( this, &GameState_Title::HandlePressScores );
	tutorial_button->signal_button_down.connect( this, &GameState_Title::HandlePressTutorial );
	about_button->signal_button_down.connect( this, &GameState_Title::HandlePressAbout );
	quit_button->signal_button_down.connect( this, &GameState_Title::HandlePressQuit );

	}


GameState_Title::~GameState_Title() {
	sequencer->Flush(3);
	delete sequencer;
	RM->DumpFont( button_font, "GameState_Title");
	}

void GameState_Title::RegisterObject( LGC::PollingObject* obj, int layer ) {
	sequencer->Register(obj,layer);
	}

void GameState_Title::Cycle( float incoming_time ) {
	// NOTE: Not using the more advanced input system:
	//input_handler.ProcessAllInput();
	sequencer->PollAll( incoming_time );
	sequencer->DrawAll();
	}

void GameState_Title::AcceptInputEvent( SDL_Event& e ) {
	// NOTE: Not using the more advanced input system:
	// input_handler.AcceptInputEvent( e );
 	if (e.type == SDL_KEYDOWN) {
 		if ( e.key.keysym.sym == SDLK_ESCAPE ) {
			Core->Quit();
 			return;
 			}
		else if ( e.key.keysym.sym == SDLK_RETURN ) {
			selectron[selectron_index]->signal_button_down(selectron[selectron_index]);
 			return;
 			}
 		else if ( e.key.keysym.sym == SDLK_DOWN ) {
			// find the selected widget
			selectron[selectron_index]->SetTextColor( unselected_color );
			if ( ++selectron_index >= selectron.size() ) { selectron_index = 0; }
 			selectron[selectron_index]->SetTextColor( selected_color );
 			return;
 			}
		else if ( e.key.keysym.sym == SDLK_UP ) {
			// find the selected widget
			selectron[selectron_index]->SetTextColor( unselected_color );
			selectron_index = ( selectron_index == 0 ) ? selectron.size() - 1 : selectron_index - 1;
 			selectron[selectron_index]->SetTextColor( selected_color );
 			return;
 			}
		}
	BasicWidget::ProcessEvent( e );
	}


void GameState_Title::InternalChangeGameState( const std::string& s ) {
// 	LGC::Core->ChangeGameState( "Play" );
	}

void GameState_Title::HandlePressPlay( Button* obj ) {
	LGC::Core->ChangeGameState( "Play" );
	}
void GameState_Title::HandlePressOptions( Button* obj ) {
	LGC::Core->ChangeGameState( "CONFIG" );
	}
void GameState_Title::HandlePressScores( Button* obj ) {
	LGC::Core->ChangeGameState( "SCOREBOARD" );
	}
void GameState_Title::HandlePressTutorial( Button* obj ) {
	LGC::Core->ChangeGameState( "HOWTO" );
	}
void GameState_Title::HandlePressAbout( Button* obj ) {
	LGC::Core->ChangeGameState( "ABOUT" );
	}
void GameState_Title::HandlePressQuit( Button* obj ) {
	Core->Quit();
	}





const GameState_Title_Factory GameState_Title_Factory::auto_register_me = GameState_Title_Factory();
















GameState_Splash::GameState_Splash() : sequencer( new LGC::Sequencer() ), timer(0) {
	Renderer->Ortho(true);
	LGC::RM->ReserveResourcesFor("LightFlare");
	}

void GameState_Splash::Activate() {
	SDL_ShowCursor( SDL_DISABLE );
	sequencer->Register( new BackgroundImage("SplashLogo", NULL, LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), BG_SCROLLMODE_CENTER), 0 );
	sequencer->Register( new LightFlare( LGC::App->ScreenWidth() * 0.5f, LGC::App->ScreenHeight() * 0.5f, LightFlare::FLASH_ULTRABLINDER ), 2 );
// 	sequencer->Register( new ParticleSourceFirework( LGC::App->ScreenWidth() * 0.5f, LGC::App->ScreenHeight() * 0.5f, 1500 ), 3 );

	sequencer->Register( new ParticleSourceWeightlessFirework( LGC::App->ScreenWidth() * 0.5f, LGC::App->ScreenHeight() * 0.5f, 500 ), 3 );



// 	sequencer->Register( new LightFlare( LGC::App->ScreenWidth() * 0.5f, LGC::App->ScreenHeight() * 0.5f, LightFlare::FLASH_BLINDER ), 2 );
	FadeInFromBlackTransition* trans = new FadeInFromBlackTransition(sequencer);
 	trans->SetFadeTime(0.2f);
	sequencer->Register( trans, 4 );
	}

GameState_Splash::~GameState_Splash() {
	sequencer->Flush(3);
	delete sequencer;
	LGC::RM->DumpResourcesFor("LightFlare");
	}

void GameState_Splash::RegisterObject( LGC::PollingObject* obj, int layer ) {
	sequencer->Register(obj,layer);
	}

void GameState_Splash::Cycle( float incoming_time ) {
	timer += incoming_time;
	if ( timer > 4.0f ) {
		LGC::Core->ChangeGameState( "TITLE_SCREEN" );
		}
// 	Renderer->ClearScreen();
	sequencer->PollAll( incoming_time );
	sequencer->DrawAll();
	}

void GameState_Splash::AcceptInputEvent( SDL_Event& e ) {
 	if ( e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN ) {
 		LGC::Core->ChangeGameState( "TITLE_SCREEN" );
		}
	}

// void GameState_Splash::InternalChangeGameState( const std::string& s ) {
// // 	LGC::Core->ChangeGameState( "Play" );
// 	}

const GameState_Splash_Factory GameState_Splash_Factory::auto_register_me = GameState_Splash_Factory();




















//-----------------------------------------------------------------------------------------------

// STATIC MEMBERS
LGC::Font GameState_Play::label_font = LGC::Font();
LGC::Font GameState_Play::gameover_font = LGC::Font();
std::map< std::string, LGC::Texture > GameState_Play::textures;
std::map< std::string, LGC::Sound > GameState_Play::sfx;

GameState_Play::GameState_Play() :
	sequencer( new LGC::Sequencer(6) ), // note: play gamestate has 6 layers instead of default 5
	havoc_timeout(0),
	do_shapes(true),
	game_over_time(0),
	reverse_controls_timeout(0),
	slowdown_timeout(0),
	last_chain_color(SHAPE_NULL),
	color_chains(0),
	recycle_time(0),
	countie(0),
	last_file(0),
	last_file2(0),
	skip_last_sides(0),
	shapes_to_add(1),
	havoc_mult(1.0),
	last_time(0),
	final_awesomeness(0),
	speed_mod(1.0f)
	{
	RM->ReserveResourcesFor("GameState_Play");
	}


void GameState_Play::Activate() {

	SDL_ShowCursor( SDL_DISABLE );

//  	FadeInFromBlackTransition* trans = new FadeInFromBlackTransition(sequencer);
 	BreakBoxTransition* trans = new BreakBoxTransition(sequencer);
	trans->SetFadeTime(1.0f);
	sequencer->Register( trans, 4 );

	Renderer->Ortho(true);

	sequencer->Register(
		new BackgroundImage("WoodBG", NULL, LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), BG_SCROLLMODE_STRETCH),
		0
		);

	const float margin = 30.0f;

	// timer
	this->timer_label = new LGC::Label( 560.0f, 20.0f + margin, NULL, "00:00", label_font);
	this->timer_label->SetXPos( (760.0f - margin) - this->timer_label->Width() );
	sequencer->Register( this->timer_label, 4 );

	// score
	this->score_label = new ScoreWidget( 40.0f + margin, 20.0f + margin, NULL );
	sequencer->Register( this->score_label, 4 );

	// status effects
 	this->status_effects = new StatusEffectWidget( 40.0f + margin, this->score_label->YFar() + 10.f, NULL );
	sequencer->Register( this->status_effects, 4 );

	// multiplier
	this->mult_label = new MultiplierWidget( 40.0f + margin, 20.0f, NULL );
	this->mult_label->SetYPos( (580.0f - margin ) - this->mult_label->Height() );
	sequencer->Register( this->mult_label, 4 );

	// kills
	this->kills_label = new ScoreWidget( 580.0f, 450.0f, NULL);
	this->kills_label->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );
	this->kills_label->DrawFloaties(false);
	this->kills_label->SetXPos( (760.0f - margin) - this->kills_label->Width() );
	this->kills_label->Hide(); // we only want this at the end of the game;
	sequencer->Register( this->kills_label, 4 );

	// color chains
	this->color_chain_label = new ScoreWidget( 560.0f, 20.0f + margin, NULL );
	this->color_chain_label->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );
	this->color_chain_label->SetXPos( (760.0f - margin) - this->color_chain_label->Width() );
	this->color_chain_label->SetYPos( (580.0f - margin) - this->color_chain_label->Height() );
	this->color_chain_label->DrawFloaties(false);
	sequencer->Register( this->color_chain_label, 4 );

	// collectables
	this->collectables = new CollectablesWidget( 560.0f, this->timer_label->YFar() + 20.0f, NULL );
 	this->collectables->SetXPos( (760.0f - margin) - this->collectables->Width() );
	sequencer->Register( this->collectables, 4 );

	// cursor
	this->cursor = new Cursor();
	this->cursor->Preserve(); // don't delete on flush; delete manually
	sequencer->Register( this->cursor, 4 );

	// shape boxes
	shapeboxes.push_back( new ShapeBox( 340, 40, 4, 6, 0 ) );
	shapeboxes.push_back( new ShapeBox( 460, 240, 4, 8, 90 ) );
	shapeboxes.push_back( new ShapeBox( 340, 360, 4, 6, 180 ) );
	shapeboxes.push_back( new ShapeBox( 60, 240, 4, 8, 270 ) );
	for ( unsigned int i = 0; i < shapeboxes.size(); i++ ) {
		sequencer->Register( shapeboxes[i], 1 );
		}

	// 0 = training
	// 1 = normal
	// 2 = hard
	// 3 = punishing
	int difficulty = Settings->GetSetting<int>("Difficulty");
	skip_last_sides = (difficulty == 3) ? 0 : ((difficulty == 2) ? 1 : 2);
	shapes_to_add = difficulty == 3 ? 2 : 1;

	gameover_trips.push_back( false );
	gameover_trips.push_back( false );
	gameover_trips.push_back( false );
	gameover_trips.push_back( false );
	gameover_trips.push_back( false );
	}


GameState_Play::~GameState_Play() {
	RM->DumpResourcesFor("GameState_Play");
	sequencer->Flush(3);
	delete sequencer;
	delete cursor; // we manually removed this before here
	cursor = NULL;
	}


void GameState_Play::PauseGame( bool x ) {
	if ( !x ) {
		sequencer->UnPause();
		sequencer->FlushLayer( 5, 1 );
		Sound::PauseAllChannels( false );
		}
	else {
		sequencer->Pause();
		sequencer->ThawLayer( 5 ); // keep this one open for pause FX
		Sound::PauseAllChannels( true );

		float fade_time = 0.8f;

		// blackout widget
		LGC::BasicWidget* blackout = new LGC::BasicWidget( 0,0,LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), NULL );
		blackout->SetBGColor( ColorQuad(0,0,0,1) );
		blackout->SetBorderWidth(0);
		sequencer->Register( blackout, 5 );

		// paused label
		LGC::Label* paused_label = new LGC::Label( 0, 0, 800, 600, NULL, "PAUSED");
		sequencer->Register( paused_label, 5 );
		paused_label->SetFont( gameover_font );
		paused_label->DrawCentered(true);
		paused_label->FadeIn(fade_time);

		// instructions
		LGC::Label* instr_label = new LGC::Label( 0, 300, 800, 300, NULL, "Press any key to continue. Press DELETE to quit.");
		sequencer->Register( instr_label, 5 );
		instr_label->DrawCentered(true);
		instr_label->FadeIn(fade_time);
		}
	}

void GameState_Play::RegisterObject( LGC::PollingObject* obj, int layer ) {
	sequencer->Register(obj,layer);
	}

void GameState_Play::ReserveResources() {
	label_font = RM->GetFont("UILabelFont", "GameState_Play");
	gameover_font = RM->GetFont("GameoverFont", "GameState_Play");
	RM->ReserveResourcesFor("ShapeBox");
	RM->ReserveResourcesFor("Label");
	sfx["game_over"] = LGC::RM->GetSound( "game_over", "GameState_Play" );
	sfx["clear_screen"] = LGC::RM->GetSound( "clear_screen", "GameState_Play" );
	sfx["speedup"] = LGC::RM->GetSound( "speedup", "GameState_Play" );
	sfx["slowdown"] = LGC::RM->GetSound( "slowdown", "GameState_Play" );
	sfx["collect"] = LGC::RM->GetSound( "collect", "GameState_Play" );
	sfx["collect_bad"] = LGC::RM->GetSound( "collect_bad", "GameState_Play" );
	sfx["use_item"] = LGC::RM->GetSound( "use_item", "GameState_Play" );
	sfx["pop_shape"] = LGC::RM->GetSound( "pop_shape", "GameState_Play" );
	sfx["infiltrate"] = LGC::RM->GetSound( "infiltrate", "GameState_Play" );
	}

void GameState_Play::DumpResources() {
	RM->DumpFont( label_font, "GameState_Play");
	RM->DumpFont( gameover_font, "GameState_Play");
	RM->DumpResourcesFor("ShapeBox");
	RM->DumpResourcesFor("Label");
	for( std::map< std::string, Sound >::iterator iter = sfx.begin(); iter != sfx.end(); iter++ ) {
		LGC::RM->DumpSound( iter->second, "GameState_Play" );
		}
	}

void GameState_Play::Cycle( float incoming_time ) {

	glPointSize(2);
	glLineWidth(1);


	if ( !sequencer->Paused() ) {

		incoming_time *= speed_mod;
		last_time += incoming_time;

		if ( !game_over_time ) {
			int minute = int((last_time / 60.0f )) % 60;
			int second = int(last_time) % 60;
			std::stringstream ss;
			ss << std::setw( 2 ) << std::setfill( '0' ) << minute << ":";
			ss << std::setw( 2 ) << std::setfill( '0' ) << second;
			std::string str = ss.str();
			this->timer_label->SetText( str );
			}

		// game over marker
		if ( !do_shapes && !game_over_time ) {
			game_over_time = last_time;
			}
		else if ( game_over_time && last_time > game_over_time + 2.0f && !gameover_trips[0] ) {
			gameover_trips[0] = true;
			for ( unsigned int i = 0; i < shapeboxes.size(); i++ ) {
				shapeboxes[i]->Clear();
				}
			sfx["clear_screen"].Play();
			mult_label->FadeOut(0.8);
			color_chain_label->FadeOut(0.8);
			this->status_effects->FadeOut(0.8);
			this->collectables->FadeOut(0.8);
			score_label->FadeOut(0.8);
			timer_label->FadeOut(0.8);
			}
		else if ( game_over_time && last_time > game_over_time + 4.0f && !gameover_trips[1] ) {
			gameover_trips[1] = true;
			sequencer->Unlink( this->cursor );
			Core->RegisterObject( new LightFlare( cursor->Xpos(), cursor->Ypos(), LightFlare::FLASH_ULTRABLINDER ), 4 );
			Core->RegisterObject( new ParticleSourceFirework( cursor->Xpos(), cursor->Ypos(), 1000 ), 4 );
			sfx["game_over"].Play();
			score_label->SetTextAlign( LGC::BasicWidget::TEXT_LEFT );
			kills_label->SetTextAlign( LGC::BasicWidget::TEXT_LEFT );
			timer_label->SetTextAlign( LGC::BasicWidget::TEXT_LEFT );
			}
		else if ( game_over_time && last_time > game_over_time + 7.0f && !gameover_trips[2] ) {
			gameover_trips[2] = true;
			gameover_label = new LGC::Label( 0, 0, 800, 600, NULL, "GAME OVER");
			sequencer->Register( gameover_label, 4 );
			gameover_label->SetFont( gameover_font );
			gameover_label->DrawCentered(true);
			gameover_label->FadeIn( 0.8 );
			}
		else if ( game_over_time && last_time > game_over_time + 9.5f && !gameover_trips[3] ) {
			float slide_time = 1.2f;
			float fade_time = 1.0f;

			gameover_trips[3] = true;
			gameover_label->FadeOut(fade_time);
			score_label->FadeIn(fade_time);
			kills_label->FadeIn(fade_time);
			timer_label->FadeIn(fade_time);

			score_label->SetXPos( 900 ); score_label->SetYPos( 100 );
			kills_label->SetXPos( 1100 ); kills_label->SetYPos( 200 );
			timer_label->SetXPos( 1300 ); timer_label->SetYPos( 300 );

			// blackout widget
			LGC::BasicWidget* blackout = new LGC::BasicWidget( 0,0,LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), NULL );
			blackout->SetBGColor( ColorQuad(0,0,0,1) );
			sequencer->Register( blackout, 1 );
			blackout->Hide();
			blackout->FadeIn(fade_time);

			// create an awesomeness label
			LGC::Label* awesomeness = new LGC::Label( 1500, 400, NULL, LGC::cast::lexical_cast<std::string,int>(int(GetAwesomeness())) + " %" );
			awesomeness->SetFont(label_font);
			awesomeness->Hide();
			awesomeness->FadeIn(fade_time);
			awesomeness->SetTextAlign( LGC::BasicWidget::TEXT_LEFT );
			sequencer->Register( awesomeness, 4 );

			// slide widgets into place
			sequencer->Register( new WidgetMover( 450, 105, score_label, slide_time ), 4 );
			sequencer->Register( new WidgetMover( 450, 205, kills_label, slide_time ), 4 );
			sequencer->Register( new WidgetMover( 450, 305, timer_label, slide_time ), 4 );
			sequencer->Register( new WidgetMover( 450, 405, awesomeness, slide_time ), 4 );

			// create stat labels
			LGC::Label* label1 = new LGC::Label( -100, 100, 280, timer_label->Height(), NULL, "SCORE:" );
			LGC::Label* label2 = new LGC::Label( -300, 200, 280, timer_label->Height(), NULL, "HITS:" );
			LGC::Label* label3 = new LGC::Label( -500, 300, 280, timer_label->Height(), NULL, "ENDURANCE:" );
			LGC::Label* label4 = new LGC::Label( -700, 400, 280, timer_label->Height(), NULL, "AWESOMENESS:" );

			label1->SetFont(label_font);
			label1->FadeIn(fade_time);
			label1->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );

			label2->SetFont(label_font);
			label2->FadeIn(fade_time);
			label2->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );

			label3->SetFont(label_font);
			label3->FadeIn(fade_time);
			label3->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );

			label4->SetFont(label_font);
			label4->FadeIn(fade_time);
			label4->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );

			sequencer->Register( label1, 4 );
			sequencer->Register( label2, 4 );
			sequencer->Register( label3, 4 );
			sequencer->Register( label4, 4 );

			sequencer->Register( new WidgetMover( 130, 100, label1, slide_time ), 4 );
			sequencer->Register( new WidgetMover( 130, 200, label2, slide_time ), 4 );
			sequencer->Register( new WidgetMover( 130, 300, label3, slide_time ), 4 );
			sequencer->Register( new WidgetMover( 130, 400, label4, slide_time ), 4 );

			}
		else if ( game_over_time && last_time > game_over_time + 22.0f && !gameover_trips[4] ) {
			gameover_trips.clear();
			LGC::Core->ChangeGameState( "SCOREBOARD" );
			}


		// create havoc
		if ( havoc_timeout > 0 ) {
			havoc_timeout -= incoming_time;
			if ( havoc_timeout <= 0 ) {
				havoc_timeout = 0;
				havoc_mult = 1.0;
				}
			else {
				havoc_mult = 4.0f;
				}
			}

		if ( slowdown_timeout > 0 ) {
			slowdown_timeout -= incoming_time;
			if ( slowdown_timeout <= 0 ) {
				slowdown_timeout = 0;
				havoc_mult = 1.0;
				}
			else {
				havoc_mult = 0.25f;
				}
			}

		if ( reverse_controls_timeout > 0 ) {
			reverse_controls_timeout -= incoming_time;
			if ( reverse_controls_timeout <= 0 ) {
				reverse_controls_timeout = 0;
				}
			}

		countie += incoming_time * havoc_mult;

		if ( do_shapes && countie > recycle_time  ) {
			for ( int n = 0; n < shapes_to_add; n++ ) {
				int next_file = RandomNum(4);
				if ( skip_last_sides == 2 ) {
					while ( next_file == last_file || next_file == last_file2 ) { next_file = RandomNum(4); }
					}
				else if ( skip_last_sides == 1 ) {
					while ( next_file == last_file ) { next_file = RandomNum(4); }
					}
				last_file2 = last_file;
				last_file = next_file;
				shapeboxes[ next_file ]->AddRandomShape( Settings->GetSetting<float>("GerminationSpeed") /* recycle_time > 1.0f ? recycle_time : 1.0f */ );
				}
			countie -= recycle_time;
			float starting_speed = Settings->GetSetting<float>("StartingSpeed");
			if ( !starting_speed ) { starting_speed = 1.0; }
			recycle_time = starting_speed * (1.0 - (last_time / Settings->GetSetting<float>("GameLength")));
			}

		}

	glColor4f( 1,1,1,1 ); // reset. should use popattribute instead

	sequencer->PollAll( incoming_time );
	sequencer->DrawAll();

	}


float GameState_Play::GetAwesomeness() {
	if ( score_label->Score() == 0 ) { return 0; }
	if ( final_awesomeness ) { return final_awesomeness; }
	final_awesomeness = (
		(last_time / ( Settings->GetSetting< int >("GameLength") * 0.75 )) // 3/4 game length survival = awesome
		+
		( (score_label->Score() / kills_label->Score()) / 10.0f ) // 10:1 score/hits ratio = awesome
		) * 0.5f  * 100.0f;
	// modify the awesomeness by the play mode
	switch ( Settings->GetSetting< int >("Difficulty") ) {
		case 0: final_awesomeness *= Settings->GetSetting< float >("AwesomeBonusDiff0"); break;
		case 1: final_awesomeness *= Settings->GetSetting< float >("AwesomeBonusDiff1"); break;
		case 2: final_awesomeness *= Settings->GetSetting< float >("AwesomeBonusDiff2"); break;
		case 3: final_awesomeness *= Settings->GetSetting< float >("AwesomeBonusDiff3"); break;
		}
	if ( Settings->GetSetting< bool >("UseSpecials") )
		{ final_awesomeness *= Settings->GetSetting< float >("AwesomeBonusMode1"); }
	else { final_awesomeness *= Settings->GetSetting< float >("AwesomeBonusMode0"); }
	return final_awesomeness;
	}

void GameState_Play::SendMessage( const std::string& s ) {
	if ( s == "gameover" ) {
		if ( collectables->UseHeart() ) {
			for ( unsigned int i = 0; i < shapeboxes.size(); i++ ) {
				shapeboxes[i]->ClearFirstRank();
				}
			sfx["pop_shape"].Play();
			}
		else {
			sfx["infiltrate"].Play();
			do_shapes = false;
			mult_label->StopAdjustments();
			for ( unsigned int i = 0; i < shapeboxes.size(); i++ ) {
				shapeboxes[i]->Cancel();
				}
			// record highscore now
			Scores->AddScore(
				ScoresMgr::Score(
					Settings->GetSetting< std::string >("Callsign"),
					int( score_label->Score() ),
					int( kills_label->Score() ),
					int( last_time ),
					GetAwesomeness(),
					Settings->GetSetting<int>("Difficulty"),
					int( Settings->GetSetting<bool>("UseSpecials") )
					)
				);
			}
		}
	else if ( s == "acquire_1up" ) {
		collectables->AddHeart();
		sfx["collect"].Play();
		}
	else if ( s == "acquire_antidote" ) {
		collectables->AddBandaid();
		sfx["collect"].Play();
		}
	else if ( s == "acquire_bomb" ) {
		collectables->AddBomb();
		sfx["collect"].Play();
		}
	else if ( s == "double_score" ) {
		this->mult_label->AddToValue( 2.0f );
		sfx["collect"].Play();
		} // was MultiplyValue
	else if ( s == "triple_score" ) {
		this->mult_label->AddToValue( 3.0f );
		sfx["collect"].Play();
		} // was MultiplyValue
	else if ( s == "slowdown" ) {
		slowdown_timeout = +5.0f;
		havoc_timeout = 0;
		this->status_effects->ActivateEffect( "speedup", 0 );
		this->status_effects->ActivateEffect( "slowdown", 5.0f );
		sfx["slowdown"].Play();
		}
	else if ( s == "speedup" ) {
		if ( !collectables->UseBandaid() ) {
			havoc_timeout += 5.0f;
			slowdown_timeout = 0;
			this->status_effects->ActivateEffect( "speedup", 5.0f );
			this->status_effects->ActivateEffect( "slowdown", 0 );
			sfx["speedup"].Play();
			}
		else { sfx["use_item"].Play(); }
		}
	else if ( s == "reverse" ) {
		if ( !collectables->UseBandaid() ) {
			reverse_controls_timeout += 5.0f;
			this->status_effects->ActivateEffect( "reverse", 5.0f );
			sfx["collect_bad"].Play();
			}
		else { sfx["use_item"].Play(); }
		}
	else if ( s == "half_score" ) {
		if ( !collectables->UseBandaid() ) {
			this->mult_label->MultiplyValue( 0.5f );
			sfx["collect_bad"].Play();
			}
		else { sfx["use_item"].Play(); }
		}
	else if ( s == "clear_screen" ) {
		for ( unsigned int i = 0; i < shapeboxes.size(); i++ ) {
			shapeboxes[i]->Clear();
			}
		sfx["clear_screen"].Play();
		}
	else if ( s == "randomize" ) {
		if ( !collectables->UseBandaid() ) {
			for ( unsigned int i = 0; i < shapeboxes.size(); i++ ) {
				shapeboxes[i]->Randomize();
				}
			sfx["collect_bad"].Play();
			}
		else { sfx["use_item"].Play(); }
		}
	}

void GameState_Play::AcceptInputEvent( SDL_Event& e ) {
 	if (e.type == SDL_KEYUP) {
 		if ( !sequencer->Paused() ) {
			switch ( e.key.keysym.sym ) {
				case SDLK_UP:
				case SDLK_DOWN: { this->cursor->Do( Cursor::HALT_Y ); break; }
				case SDLK_RIGHT:
				case SDLK_LEFT: { this->cursor->Do( Cursor::HALT_X ); break; }
				default:;;
				}
			}
		}
	// note: we want any key to bail out of the game over sequence,
	// but not for the first few seconds (until the screen starts to blow up).
	// Otherwise players will be actively pressing buttons and the game
	// may seem to suddenly "quit" if they press a key right after gameover
	// but before the ending sequence starts to roll.
	else if (e.type == SDL_KEYDOWN && gameover_trips[0] ) {
		LGC::Core->ChangeGameState( "TITLE_SCREEN" );
		}
 	else if (e.type == SDL_KEYDOWN) {
 		if ( sequencer->Paused() ) {
			switch ( e.key.keysym.sym ) {
				case SDLK_DELETE: {
					LGC::Core->ChangeGameState( "TITLE_SCREEN" );
					break;
					}
				default: {
					this->PauseGame( false );
					}
				}
 			}
 		else {
 			// easter egg!
 			if (	(SDL_GetModState() & KMOD_SHIFT) != 0) {
				switch ( e.key.keysym.sym ) {
					case SDLK_1: speed_mod = 1.0f; break;
					case SDLK_2: speed_mod = ( 1.0f / 2.0f ); break;
					case SDLK_3: speed_mod = ( 1.0f / 3.0f ); break;
					case SDLK_4: speed_mod = ( 1.0f / 4.0f ); break;
					case SDLK_5: speed_mod = ( 1.0f / 5.0f ); break;
					case SDLK_6: speed_mod = ( 1.0f / 6.0f ); break;
					case SDLK_7: speed_mod = ( 1.0f / 7.0f ); break;
					case SDLK_8: speed_mod = ( 1.0f / 8.0f ); break;
					case SDLK_9: speed_mod = ( 1.0f / 9.0f ); break;
					case SDLK_0: speed_mod = ( 1.0f / 10.0f ); break;
					default: ;;
					}
 				}
 			else {
				switch ( e.key.keysym.sym ) {
					case SDLK_p:
					case SDLK_DELETE:
					case SDLK_TAB:
					case SDLK_BACKQUOTE:
					case SDLK_ESCAPE: {
						this->PauseGame( true );
						break;
						}
					case SDLK_RETURN: {
						if ( collectables->UseBomb() ) {
							for ( unsigned int i = 0; i < shapeboxes.size(); i++ ) {
								shapeboxes[i]->Clear();
								}
							sfx["clear_screen"].Play();
							}
						break;
						}
					case SDLK_UP: {
						if ( !cursor || game_over_time ) { break; }
						this->cursor->Do( reverse_controls_timeout ? Cursor::DOWN : Cursor::UP );
						break;
						}
					case SDLK_DOWN: {
						if ( !cursor || game_over_time ) { break; }
						this->cursor->Do( reverse_controls_timeout ? Cursor::UP : Cursor::DOWN );
						break;
						}
					case SDLK_RIGHT: {
						if ( !cursor || game_over_time ) { break; }
						this->cursor->Do( reverse_controls_timeout ? Cursor::LEFT : Cursor::RIGHT );
						break;
						}
					case SDLK_LEFT: {
						if ( !cursor || game_over_time ) { break; }
						this->cursor->Do( reverse_controls_timeout ? Cursor::RIGHT : Cursor::LEFT );
						break;
						}
					case SDLK_SPACE: {
						if ( !cursor || game_over_time ) { break; }
						int pts = 0;
						int kills = 0;
						float mult = 0;
						Vector v;
						switch ( this->cursor->Direction() ) {
							case   0: v = shapeboxes[0]->ShootAt( this->cursor->XFile(), this->cursor, pts, kills, mult, this->last_chain_color, this->color_chains ); break;
							case  90: v = shapeboxes[1]->ShootAt( this->cursor->YFile(), this->cursor, pts, kills, mult, this->last_chain_color, this->color_chains ); break;
							case 180: v = shapeboxes[2]->ShootAt( this->cursor->XFile(), this->cursor, pts, kills, mult, this->last_chain_color, this->color_chains ); break;
							case 270: v = shapeboxes[3]->ShootAt( this->cursor->YFile(), this->cursor, pts, kills, mult, this->last_chain_color, this->color_chains ); break;
							}
						this->cursor->ShootAt( v );

						// update pts total
						if ( pts ) {
							this->score_label->AddToScore(  float(pts) * this->mult_label->Value() );
							}

						// update kills total
						if ( kills ) {
							this->kills_label->AddToScore( kills );
							}

						// update multiplier
						if ( mult ) {
							this->mult_label->AddToValue( mult );
							}

						// update color chains
						if ( color_chains ) {
							color_chain_label->ResetScore();
							color_chain_label->AddToScore( this->color_chains );
							switch ( this->last_chain_color ) {
								case SHAPE_BLUE: this->color_chain_label->SetTextColor( LGC::ColorQuad(0.06f, 0.2f, 0.8f, 1.0) ); break;
								case SHAPE_GREEN: this->color_chain_label->SetTextColor( LGC::ColorQuad( 0.08f, 0.8f, 0.2f, 1.0 ) ); break;
								case SHAPE_YELLOW: this->color_chain_label->SetTextColor( LGC::ColorQuad( 0.8f, 0.8f, 0.1f, 1.0 ) ); break;
								case SHAPE_RED: this->color_chain_label->SetTextColor( LGC::ColorQuad( 0.8f, 0.2f, 0.08f, 1.0 ) ); break;
								case SHAPE_PURPLE: this->color_chain_label->SetTextColor( LGC::ColorQuad( 0.8f, 0.26f, 0.58f, 1.0 ) ); break;
								default: ;;
								}
							}

						break;
						}
					default: {
						// do nothing
						;;
						break;
						}
					}
				}
 			}
		}
	}


const GameState_Play_Factory GameState_Play_Factory::auto_register_me = GameState_Play_Factory();

REGISTER_CLASS_AS_RESOURCE_CLIENT( GameState_Play )












GameState_About::GameState_About() :
	sequencer( new LGC::Sequencer() )
	{
	Renderer->Ortho(true);
	font = RM->GetFont("ReadingFont", "GameState_About");
	title_font = RM->GetFont("PageHeadingFont", "GameState_About");
	}


void GameState_About::Activate() {

	SDL_ShowCursor( SDL_ENABLE );

	sequencer->Register( new BackgroundImage("AboutPageBG", NULL, LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), BG_SCROLLMODE_STRETCH), 0 );

 	CrossFadeTransition* trans = new CrossFadeTransition(sequencer);
 	trans->SetFadeTime(0.4f);
	sequencer->Register( trans, 4 );

	LGC::Button::ReserveResources();

	window = new LGC::BasicWidget( 0, 0, App->ScreenWidth(), App->ScreenHeight(), NULL );
	window->SetBorderWidth(0);
	window->SetBGColor( ColorQuad(0,0,0,0) );
	window->PushModal(); // REQUIRED TO RECEIVE INPUT
	sequencer->Register( window, 1 );

	back_button = new LGC::Button( 50, 20, 300, 30, window, "<< BACK" );
	back_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	back_button->SetBorderColor( ColorQuad(0,0,0,1) );
	back_button->signal_button_down.connect( this, &GameState_About::HandlePressBack );

	LGC::Label* title = new LGC::Label( 220, 15, 530, 30, window, "ABOUT ORTHO" );
	title->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );
	title->SetFont( title_font );

	TextBlock* textblock = new TextBlock( 50, 80, 700, 550, font, window );
	textblock->SetTextColor( ColorQuad(0,0,0,1) );
	textblock->SetShadowColor( ColorQuad(1,1,1,0) );
	textblock->SetBGColor( ColorQuad(0,0,0,0) );
	textblock->SetBorderWidth( 0 );
	textblock->AddLine("Ortho was written and programmed entirely by leiavoia in 2010. Ortho is based on ZOOP, originally created by Hookstone and published by Viacom New Media in 1995.");
	textblock->AddLine("Ortho changes or improves on ZOOP in many ways, so it is not a \"ZOOP clone\". Its goal was never to duplicate ZOOP, but rather to take the concept further. Ortho does away with ZOOP elements which were considered bad and adds new game mechanics and specials. Two new game mechanics created for Ortho are the Multiplier and Color Chaining. Ortho also implements fun new specials like screen-clearing bombs, hearts (1UPs), point multipliers, and even a variety of negative specials.");
	textblock->AddLine("Ortho has a definate end. The game goes progressively faster until you are utterly crushed by oncoming shapes. This makes for brisk, lively gameplay for people with short attention spans. The longest theoretical game is 20 minutes. Most people won't make it past 15 on normal difficulty.");
	textblock->AddLine("Ortho uses the LibLevGC game development library also created by leiavoia. It took 7 years to bake. It uses OpenGL, SDL, and FTGL. Hopefully you'll appreciate the game. Donations welcomed.");
	textblock->AddLine("Contact: ORTHO@LEIAVOIA.NET, http://www.leiavoia.net/ortho/");
	textblock->AddLine("Thanks for playing. You rock!");
	}


GameState_About::~GameState_About() {
	sequencer->Flush(3);
	delete sequencer;
	RM->DumpFont( font, "GameState_About");
	RM->DumpFont( title_font, "GameState_About");
	LGC::Button::DumpResources();
	}

void GameState_About::RegisterObject( LGC::PollingObject* obj, int layer ) {
	sequencer->Register(obj,layer);
	}

void GameState_About::Cycle( float incoming_time ) {
	sequencer->PollAll( incoming_time );
	sequencer->DrawAll();
	SetColor( 0,0,0,1 );
	//
	// TEMPORARY - draw text
	//
	font.UseStoredText( "about_page_text", 50, 80 );
	}

void GameState_About::AcceptInputEvent( SDL_Event& e ) {
	// NOTE: Not using the more advanced input system:
	// input_handler.AcceptInputEvent( e );
 	if (e.type == SDL_KEYDOWN) {
 		LGC::Core->ChangeGameState( "TITLE_SCREEN" );
		}
	BasicWidget::ProcessEvent( e );
	}


void GameState_About::HandlePressBack( Button* obj ) {
	LGC::Core->ChangeGameState( "TITLE_SCREEN" );
	}




const GameState_About_Factory GameState_About_Factory::auto_register_me = GameState_About_Factory();










GameState_Howto::GameState_Howto() :
	sequencer( new LGC::Sequencer() )
	{
	Renderer->Ortho(true);
	font = RM->GetFont("ReadingFont", "GameState_Howto");
	title_font = RM->GetFont("PageHeadingFont", "GameState_Howto");
	}


void GameState_Howto::Activate() {

	SDL_ShowCursor( SDL_ENABLE );

	sequencer->Register( new BackgroundImage("HowtoPageBG", NULL, LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), BG_SCROLLMODE_STRETCH), 0 );

 	CrossFadeTransition* trans = new CrossFadeTransition(sequencer);
	trans->SetFadeTime(0.4f);
	sequencer->Register( trans, 4 );

	LGC::Button::ReserveResources();

	window = new LGC::BasicWidget( 0, 0, App->ScreenWidth(), App->ScreenHeight(), NULL );
	window->SetBorderWidth(0);
	window->SetBGColor( ColorQuad(0,0,0,0) );
	window->PushModal(); // REQUIRED TO RECEIVE INPUT
	sequencer->Register( window, 1 );

	back_button = new LGC::Button( 50, 20, 300, 30, window, "<< BACK" );
	back_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	back_button->SetBorderColor( ColorQuad(0,0,0,1) );
	back_button->signal_button_down.connect( this, &GameState_Howto::HandlePressBack );

	LGC::Label* title = new LGC::Label( 220, 15, 530, 30, window, "HOW TO PLAY" );
	title->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );
	title->SetFont( title_font );

	TextBlock* textblock = new TextBlock( 50, 80, 700, 550, font, window );
	textblock->SetTextColor( ColorQuad(0,0,0,1) );
	textblock->SetShadowColor( ColorQuad(1,1,1,0) );
	textblock->SetBGColor( ColorQuad(0,0,0,0) );
	textblock->SetBorderWidth( 0 );
	textblock->AddLine("[SPACE] to shoot. [ARROW KEYS] to move. [P] or [ESC] to pause. [ENTER] to detonate a bomb (if you have one).");
	textblock->AddLine("Protect the center square by shooting incoming shapes! Shoot like-colored shapes to destroy them. Shoot differently-colored shapes to swap colors. If any shape infiltrates the center, it's game over!");
	textblock->AddLine("The more shapes you shoot in a single shot, the more points you get. 1 for the first, 2 for the second, and so on. Get 50pts for shooting a full column of 5, 100pts for 7.");
	textblock->AddLine("Failure is inevitable! The game speeds up over time and you will eventually be crushed. Try for a high score! Receive your Awesomeness Rating at the end. Awesome players have a high point-to-hit ratio and last a long time.");
	textblock->AddLine("Shoot special shapes to get special effects: HEART (1up), BOMB (clear screen), BANDAID (cancels negative specials), SLOWDOWN (quarter speed), +2 and +3 point multipliers, and CLEAR SCREEN. Don't shoot the bad specials: 1/2X point multiplier, DOOM (quadruple speed), RANDOMIZER, and REVERSI.");
	textblock->AddLine("Increase your scoring potential by boosting your multiplier: collect multiplier specials or COLOR CHAIN. Shoot more than 10 of the same color in a row and earn incremental multipliers.");
	}


GameState_Howto::~GameState_Howto() {
	sequencer->Flush(3);
	delete sequencer;
	RM->DumpFont( font, "GameState_Howto");
	RM->DumpFont( title_font, "GameState_Howto");
	LGC::Button::DumpResources();
	}

void GameState_Howto::RegisterObject( LGC::PollingObject* obj, int layer ) {
	sequencer->Register(obj,layer);
	}

void GameState_Howto::Cycle( float incoming_time ) {
	sequencer->PollAll( incoming_time );
	sequencer->DrawAll();
	}

void GameState_Howto::AcceptInputEvent( SDL_Event& e ) {
	// NOTE: Not using the more advanced input system:
	// input_handler.AcceptInputEvent( e );
 	if (e.type == SDL_KEYDOWN) {
 		LGC::Core->ChangeGameState( "TITLE_SCREEN" );
		}
	BasicWidget::ProcessEvent( e );
	}


void GameState_Howto::HandlePressBack( Button* obj ) {
	LGC::Core->ChangeGameState( "TITLE_SCREEN" );
	}




const GameState_Howto_Factory GameState_Howto_Factory::auto_register_me = GameState_Howto_Factory();











GameState_Scores::GameState_Scores() :
	sequencer( new LGC::Sequencer() )
	{
	Renderer->Ortho(true);
	font = RM->GetFont("ScoresPageFont", "GameState_Scores");
	title_font = RM->GetFont("PageHeadingFont", "GameState_Scores");
	}


void GameState_Scores::Activate() {

	SDL_ShowCursor( SDL_ENABLE );

	sequencer->Register( new BackgroundImage("AboutPageBG", NULL, LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), BG_SCROLLMODE_STRETCH), 0 );

 	CrossFadeTransition* trans = new CrossFadeTransition(sequencer);
 	trans->SetFadeTime(0.4f);
	sequencer->Register( trans, 4 );

	LGC::Button::ReserveResources();
	LGC::Label::ReserveResources();

	window = new LGC::BasicWidget( 0, 0, App->ScreenWidth(), App->ScreenHeight(), NULL );
	window->SetBorderWidth(0);
	window->SetBGColor( ColorQuad(0,0,0,0) );
	window->PushModal(); // REQUIRED TO RECEIVE INPUT
	sequencer->Register( window, 1 );

	back_button = new LGC::Button( 20, 20, 200, 30, window, "<< BACK" );
	back_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	back_button->SetBorderColor( ColorQuad(0,0,0,1) );
	back_button->signal_button_down.connect( this, &GameState_Scores::HandlePressBack );

	// create a pane to put all the scores in
	score_pane = new LGC::BasicWidget( 20, 70, App->ScreenWidth()-40, App->ScreenHeight()-90, window );
	score_pane->SetBorderWidth(4);
	score_pane->SetBorderColor( ColorQuad(1,1,1,1) );
	score_pane->SetBGColor( ColorQuad(0,0,0,0.5) );

	LGC::Label* title = new LGC::Label( 240, 15, 530, 30, window, "SCOREBOARD" );
	title->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );
// 	title->SetTextColor( ColorQuad(0,0,0,1) );
	title->SetFont( title_font );

	float x[7];
	x[0] = 20;
	x[1] = x[0] + 120;
	x[2] = x[1] + 100;
	x[3] = x[2] + 80;
	x[4] = x[3] + 90;
	x[5] = x[4] + 130;
	x[6] = x[5] + 120;

	score_pane->AddChild( new LGC::Label( x[0], 20, NULL, "PLAYER", font ) );
	score_pane->AddChild( new LGC::Label( x[1], 20, NULL, "SCORE", font ) );
	score_pane->AddChild( new LGC::Label( x[2], 20, NULL, "HITS", font ) );
	score_pane->AddChild( new LGC::Label( x[3], 20, NULL, "TIME", font ) );
	score_pane->AddChild( new LGC::Label( x[4], 20, NULL, "AWESOME", font ) );
	score_pane->AddChild( new LGC::Label( x[5], 20, NULL, "LEVEL", font ) );
	score_pane->AddChild( new LGC::Label( x[6], 20, NULL, "MODE", font ) );
	// score_pane->AddChild( new LGC::Label( 0, y, 100, 20, NULL, "DATE" ) );

	// figure out which scores are the highest in their category
	std::vector<ScoresMgr::Score> scores = Scores->GetScores();
	int hscore = 0;
	int hhits = 0;
	int htime = 0;
	int hawesome = 0;
	for ( unsigned int i = 0; i < scores.size(); i++ ) {
		if ( scores[i].score > hscore ) { hscore = scores[i].score; }
		if ( scores[i].hits > hhits ) { hhits = scores[i].hits; }
		if ( scores[i].seconds > htime ) { htime = scores[i].seconds; }
		if ( scores[i].awesomeness > hawesome ) { hawesome = scores[i].awesomeness; }
		}

	// load all the scores into the window
	float y = 54.0f;
	for ( unsigned int i = 0; i < scores.size(); i++ ) {

		// NAME
		score_pane->AddChild( new LGC::Label( x[0], y, NULL, scores[i].player, font ) );

		// SCORE
		LGC::Label* scorelabel = new LGC::Label( x[1], y, NULL, commaize( scores[i].score ), font );
		if ( scores[i].score == hscore ) { scorelabel->SetTextColor( ColorQuad(0.35f,0.9f,0.5f,1.0f) ); }
		score_pane->AddChild( scorelabel );

		// HITS
		LGC::Label* hitslabel = new LGC::Label( x[2], y, NULL, commaize( scores[i].hits ), font );
		if ( scores[i].hits == hhits ) { hitslabel->SetTextColor( ColorQuad(0.35f,0.9f,0.5f,1.0f) ); }
		score_pane->AddChild( hitslabel );

		// TIME
		int seconds = scores[i].seconds;
		int minute = (seconds / 60) % 60;
		int second = seconds % 60;
		std::stringstream ss;
		ss << std::setw( 2 ) << std::setfill( '0' ) << minute << ":";
		ss << std::setw( 2 ) << std::setfill( '0' ) << second;
		LGC::Label* timelabel = new LGC::Label( x[3], y, NULL, ss.str(), font );
		if ( scores[i].seconds == htime ) { timelabel->SetTextColor( ColorQuad(0.35f,0.9f,0.5f,1.0f) ); }
		score_pane->AddChild( timelabel );

		// AWESOMENESS
		LGC::Label* awelabel = new LGC::Label( x[4], y, NULL, LGC::cast::lexical_cast<std::string,float>( floor(scores[i].awesomeness) ) + "%", font );
		if ( scores[i].awesomeness == hawesome ) { awelabel->SetTextColor( ColorQuad(0.35f,0.9f,0.5f,1.0f) ); }
		score_pane->AddChild( awelabel );

		// DIFFICULTY
		LGC::Label* difflabel = new LGC::Label( x[5], y, NULL, "Normal", font );
// 		difflabel->SetTextColor( ColorQuad(0.3f,0.75f,0.99f,1.0f) );
		if ( scores[i].difficulty == 0 ) {
			difflabel->SetText("Training");
// 			difflabel->SetTextColor( ColorQuad(0.35f,0.9f,0.5f,1.0f) );
			}
		else if ( scores[i].difficulty == 2 ) {
			difflabel->SetText("Hard");
// 			difflabel->SetTextColor( ColorQuad(0.8f,0.8f,0.2f,1.0f) );
			}
		else if ( scores[i].difficulty == 3 ) {
			difflabel->SetText("Punishing");
// 			difflabel->SetTextColor( ColorQuad(0.9f,0.2f,0.2f,1.0f) );
			}
		score_pane->AddChild( difflabel );

		// MODE
		std::string mode = "Specials";
		if ( scores[i].mode == 0 ) { mode = "Pure"; }
		score_pane->AddChild( new LGC::Label( x[6], y, NULL, mode, font ) );
		y += 30.0f;
		}
	}


GameState_Scores::~GameState_Scores() {
	sequencer->Flush(3);
	delete sequencer;
	RM->DumpFont( font, "GameState_Scores");
	LGC::Button::DumpResources();
	LGC::Label::DumpResources();
	LGC::RM->DumpFont( font, "GameState_Scores" );
	LGC::RM->DumpFont( title_font, "GameState_Scores" );
	}

void GameState_Scores::RegisterObject( LGC::PollingObject* obj, int layer ) {
	sequencer->Register(obj,layer);
	}

void GameState_Scores::Cycle( float incoming_time ) {
	sequencer->PollAll( incoming_time );
	sequencer->DrawAll();
	SetColor( 0,0,0,1 );
	}

void GameState_Scores::AcceptInputEvent( SDL_Event& e ) {
 	if (e.type == SDL_KEYDOWN) {
		LGC::Core->ChangeGameState( "TITLE_SCREEN" );
		return;
		}
	BasicWidget::ProcessEvent( e );
	}


void GameState_Scores::HandlePressBack( Button* obj ) {
	LGC::Core->ChangeGameState( "TITLE_SCREEN" );
	}




const GameState_Scores_Factory GameState_Scores_Factory::auto_register_me = GameState_Scores_Factory();










GameState_Config::GameState_Config() :
	sequencer( new LGC::Sequencer() ),
	row_index(0),
	selected_color( ColorQuad(0.35f,0.9f,0.5f,1.0f) ),
	unselected_color( ColorQuad(1,1,1,1) ),
	diff_index(0),
	mode_index(0)
	{
	Renderer->Ortho(true);
	font = RM->GetFont("ReadingFont", "GameState_Config");
	title_font = RM->GetFont("PageHeadingFont", "GameState_Config");
	sfx = RM->GetSound( "laser_sound", "GameState_Config" );
	}


void GameState_Config::Activate() {

	static float spacing = 85.0f;
	static float subspacing = 45.0f;
	static ColorQuad hilite = ColorQuad(0.35f,0.9f,0.5f,1.0f);
	static ColorQuad white = ColorQuad(1,1,1,1);

	SDL_ShowCursor( SDL_ENABLE );

	sequencer->Register( new BackgroundImage("AboutPageBG", NULL, LGC::App->ScreenWidth(), LGC::App->ScreenHeight(), BG_SCROLLMODE_STRETCH), 0 );

 	CrossFadeTransition* trans = new CrossFadeTransition(sequencer);
 	trans->SetFadeTime(0.4f);
	sequencer->Register( trans, 4 );

	LGC::Button::ReserveResources();
	LGC::TextBox::ReserveResources();
	LGC::DropBox::ReserveResources();

	window = new LGC::BasicWidget( 0, 0, App->ScreenWidth(), App->ScreenHeight(), NULL );
	window->SetBorderWidth(0);
	window->SetBGColor( ColorQuad(0,0,0,0) );
	window->PushModal(); // REQUIRED TO RECEIVE INPUT
	sequencer->Register( window, 1 );

	float y = 20.0f;

	back_button = new LGC::Button( 40, y, 300, 30, window, "<< BACK" );
	back_button->SetBGColor( ColorQuad(0,0,0,0.5f) );
	back_button->SetBorderColor( ColorQuad(0,0,0,1) );
	back_button->SetTextColor( selected_color );
	back_button->signal_button_down.connect( this, &GameState_Config::HandlePressBack );

	LGC::Label* title = new LGC::Label( 240, y-5.0f, 530, 30, window, "SETUP" );
	title->SetTextAlign( LGC::BasicWidget::TEXT_RIGHT );
	title->SetFont( title_font );

	y = 100.0f;

	diff_label = new LGC::Label( 40, y, 300, 40, window, "Difficulty" );
	diff_label->SetFont( title_font );

	y += subspacing;

	diff0 = new LGC::Button( 40, y, 170, 30, window, "Training" );
	diff0->SetBGColor( ColorQuad(0,0,0,0.5f) );
	diff0->SetBorderColor( ColorQuad(0,0,0,1) );
	diff0->signal_button_down.connect( this, &GameState_Config::HandleDifficulty );

	diff1 = new LGC::Button( 223, y, 170, 30, window, "Normal" );
	diff1->SetBGColor( ColorQuad(0,0,0,0.5f) );
	diff1->SetBorderColor( ColorQuad(0,0,0,1) );
	diff1->signal_button_down.connect( this, &GameState_Config::HandleDifficulty );

	diff2 = new LGC::Button( 406, y, 170, 30, window, "Hard" );
	diff2->SetBGColor( ColorQuad(0,0,0,0.5f) );
	diff2->SetBorderColor( ColorQuad(0,0,0,1) );
	diff2->signal_button_down.connect( this, &GameState_Config::HandleDifficulty );

	diff3 = new LGC::Button( 589, y, 170, 30, window, "Punishing" );
	diff3->SetBGColor( ColorQuad(0,0,0,0.5f) );
	diff3->SetBorderColor( ColorQuad(0,0,0,1) );
	diff3->signal_button_down.connect( this, &GameState_Config::HandleDifficulty );

	switch ( Settings->GetSetting<int>("Difficulty") ) {
		case 0 : diff0->SetTextColor( hilite ); diff_index = 0; break;
		case 1 : diff1->SetTextColor( hilite ); diff_index = 1; break;
		case 2 : diff2->SetTextColor( hilite ); diff_index = 2; break;
		case 3 : diff3->SetTextColor( hilite ); diff_index = 3; break;
		default:  diff1->SetTextColor( hilite ); diff_index = 1; break;
		}

	y += spacing;

	explain_pane = new TextBlock( 453, y, 306, (spacing*2)+(subspacing*3)+30, font, window );
	explain_pane->SetBGColor( ColorQuad(0,0,0,0.2f) );
	explain_pane->SetBorderColor( ColorQuad(0,0,0,1) );
	explain_pane->SetTextColor( ColorQuad(1,1,1,1) );
	explain_pane->SetShadowColor( ColorQuad(0,0,0,1) );
	explain_pane->SetBorderWidth( 2 );
	explain_pane->SetBorderPadding( 10 );

	mode_label = new LGC::Label( 40, y, 300, 40, window, "Specials" );
	mode_label->SetFont( title_font );

	y += subspacing;

	mode0 = new LGC::Button( 40, y, 170, 30, window, "On" );
	mode0->SetBGColor( ColorQuad(0,0,0,0.5f) );
	mode0->SetBorderColor( ColorQuad(0,0,0,1) );
	mode0->signal_button_down.connect( this, &GameState_Config::HandleMode );

	mode1 = new LGC::Button( 223, y, 170, 30, window, "Off" );
	mode1->SetBGColor( ColorQuad(0,0,0,0.5f) );
	mode1->SetBorderColor( ColorQuad(0,0,0,1) );
	mode1->signal_button_down.connect( this, &GameState_Config::HandleMode );

	if ( Settings->GetSetting<bool>("UseSpecials") ) {
		mode0->SetTextColor( hilite );
		mode_index = 0;
		}
	else {
		mode1->SetTextColor( hilite );
		mode_index = 1;
		}

	y += spacing;

	callsign_label = new LGC::Label( 40, y, 300, 40, window, "Callsign" );
	callsign_label->SetFont( title_font );

	y += subspacing;

	callsign = new LGC::TextBox( 40, y, 354, 30, window, Settings->GetSetting< std::string >("Callsign") );
	callsign->SetBGColor( ColorQuad(0,0,0,0.2f) );
	callsign->SetBorderColor( ColorQuad(0,0,0,1) );
	callsign->signal_change.connect( this, &GameState_Config::HandleCallsign );
	callsign->signal_enter.connect( this, &GameState_Config::HandleCallsignEnter );

	y += spacing;

	vol_label = new LGC::Label( 40, y, 300, 40, window, "SFX Volume" );
	vol_label->SetFont( title_font );

	y += subspacing;

	sound_vol = new LGC::HSlider( 40, y, 354, 30, 0, 100, window );
	sound_vol->SetBGColor( ColorQuad(0,0,0,0.2f) );
	sound_vol->SetBorderColor( ColorQuad(0,0,0,1) );
	sound_vol->SetRange( 0, 1.0f );
	sound_vol->SetValue( Settings->GetSetting<float>("SoundFXVolume") );
	sound_vol->signal_slider_move.connect( this, &GameState_Config::HandleSoundVol );


// 	music_vol = new LGC::HSlider( 450, 350, 300, 40, 0, 100, window );
// 	music_vol->SetBGColor( ColorQuad(0,0,0,1.0f) );

	CompileText();
	}


void GameState_Config::CompileText() {

	explain_pane->ClearText();

	if ( Settings->GetSetting<bool>("UseSpecials") ) {
		explain_pane->AddLine("Special shapes enabled.");
		}
	else {
		explain_pane->AddLine("No special shapes.");
		}

	switch ( Settings->GetSetting<int>("Difficulty") ) {
		case 0 : {
			explain_pane->AddLine("[3] shape types.");
			explain_pane->AddLine("[1] new shape at a time.");
			explain_pane->AddLine("New shapes won't appear on the last [2] sides.");
			break;
			}
		case 1 : {
			explain_pane->AddLine("[4] shape types.");
			explain_pane->AddLine("[1] new shape at a time.");
			explain_pane->AddLine("New shapes won't appear on the last [2] sides.");
			break;
			}
		case 2 : {
			explain_pane->AddLine("[5] shape types.");
			explain_pane->AddLine("[1] new shape at a time.");
			explain_pane->AddLine("New shapes won't appear on the last side.");
			break;
			}
		case 3 : {
			explain_pane->AddLine("[5] shape types.");
			explain_pane->AddLine("[2] new shapes at a time.");
			explain_pane->AddLine("New shapes can appear on any side.");
			if ( Settings->GetSetting<bool>("UseSpecials") ) {
				explain_pane->AddLine("Unfair number of negative specials.");
				}
			explain_pane->AddLine("Mostly for insane people.");
// 			explain_pane->AddLine("You won't last 3 minutes.");
			break;
			}
		default: {}
		}

	// modify the awesomeness by the play mode
	float a = 1.0f;
	switch ( Settings->GetSetting< int >("Difficulty") ) {
		case 0: a *= Settings->GetSetting< float >("AwesomeBonusDiff0"); break;
		case 1: a *= Settings->GetSetting< float >("AwesomeBonusDiff1"); break;
		case 2: a *= Settings->GetSetting< float >("AwesomeBonusDiff2"); break;
		case 3: a *= Settings->GetSetting< float >("AwesomeBonusDiff3"); break;
		}
	if ( Settings->GetSetting< bool >("UseSpecials") )
		{ a *= Settings->GetSetting< float >("AwesomeBonusMode1"); }
	else { a *= Settings->GetSetting< float >("AwesomeBonusMode0"); }
	a = round((a - 1.0f) * 100);
	if ( a ) {
		std::stringstream ss;
		if (a > 0 ) { ss << "+"; }
		ss << a << "% Awesomeness Bonus";
		explain_pane->AddLine( ss.str() );
		}

	}




GameState_Config::~GameState_Config() {
	if ( callsign->Text().empty() ) { callsign->SetText("Your Name Here"); }
	sequencer->Flush(3);
	delete sequencer;
	RM->DumpFont( font, "GameState_Config");
	RM->DumpFont( title_font, "GameState_Config" );
	RM->DumpSound( sfx, "GameState_Config" );
	LGC::Button::DumpResources();
	LGC::TextBox::DumpResources();
	LGC::DropBox::DumpResources();
	}

void GameState_Config::RegisterObject( LGC::PollingObject* obj, int layer ) {
	sequencer->Register(obj,layer);
	}

void GameState_Config::Cycle( float incoming_time ) {
	sequencer->PollAll( incoming_time );
	sequencer->DrawAll();
	SetColor( 0,0,0,1 );
	//
	// TEMPORARY - draw text
	//
	font.UseStoredText( "diff_text", explain_pane->Xpos() + 10, explain_pane->Ypos() + 20 );
	}

void GameState_Config::AcceptInputEvent( SDL_Event& e ) {
	// NOTE: Not using the more advanced input system:
	// input_handler.AcceptInputEvent( e );
	static ColorQuad hilite = ColorQuad(0.35f,0.9f,0.5f,1.0f);
	static ColorQuad white = ColorQuad(1,1,1,1);

	if (e.type == SDL_KEYDOWN) {
 		if ( e.key.keysym.sym == SDLK_ESCAPE ) {
			LGC::Core->ChangeGameState( "TITLE_SCREEN" );
 			return;
 			}
		else if ( e.key.keysym.sym == SDLK_RETURN && !callsign->Focused() ) {
			if ( row_index == 0 ) back_button->signal_button_down(back_button);
 			return;
 			}
 		else if ( e.key.keysym.sym == SDLK_RIGHT && !callsign->Focused() ) {
			switch ( row_index ) {
				case 1: {
					if ( ++diff_index >= 4 ) { diff_index = 0; }
					diff0->SetTextColor( white );
					diff1->SetTextColor( white );
					diff2->SetTextColor( white );
					diff3->SetTextColor( white );
					if ( diff_index == 0 ) {
						Settings->SetSetting<int>( "Difficulty", 0 );
						diff0->SetTextColor( hilite );
						}
					else if ( diff_index == 1 ) {
						Settings->SetSetting<int>( "Difficulty", 1 );
						diff1->SetTextColor( hilite );
						}
					else if ( diff_index == 2 ) {
						Settings->SetSetting<int>( "Difficulty", 2 );
						diff2->SetTextColor( hilite );
						}
					else if ( diff_index == 3 ) {
						Settings->SetSetting<int>( "Difficulty", 3 );
						diff3->SetTextColor( hilite );
						}
					CompileText();
					break;
					}
				case 2: {
					mode_index = ( mode_index == 0 ) ? 1 : 0;
					if ( mode_index == 0 ) {
						Settings->SetSetting<bool>( "UseSpecials", true );
						mode0->SetTextColor( hilite );
						mode1->SetTextColor( white );
						}
					else {
						Settings->SetSetting<bool>( "UseSpecials", false );
						mode0->SetTextColor( white );
						mode1->SetTextColor( hilite );
						}
					CompileText();
					break;
					}
				case 3: {
					std::string command = "right";
					callsign->HandleCommand( command );
					break;
					}
				case 4: {
					// manually decrease by 5%
					float range = sound_vol->End() - sound_vol->Start();
					sound_vol->SetValue( sound_vol->Value() + (range*0.05) );
					Settings->SetSetting<float>( "SoundFXVolume", sound_vol->Value() );
					LGC::Core->SetMasterFXVolume( sound_vol->Value() );
					sfx.Play();
					break;
					}
				}
			}
 		else if ( e.key.keysym.sym == SDLK_LEFT && !callsign->Focused() ) {
			switch ( row_index ) {
				case 1: {
					diff_index = ( diff_index == 0 ) ? 3 : diff_index - 1;
					diff0->SetTextColor( white );
					diff1->SetTextColor( white );
					diff2->SetTextColor( white );
					diff3->SetTextColor( white );
					if ( diff_index == 0 ) {
						Settings->SetSetting<int>( "Difficulty", 0 );
						diff0->SetTextColor( hilite );
						}
					else if ( diff_index == 1 ) {
						Settings->SetSetting<int>( "Difficulty", 1 );
						diff1->SetTextColor( hilite );
						}
					else if ( diff_index == 2 ) {
						Settings->SetSetting<int>( "Difficulty", 2 );
						diff2->SetTextColor( hilite );
						}
					else if ( diff_index == 3 ) {
						Settings->SetSetting<int>( "Difficulty", 3 );
						diff3->SetTextColor( hilite );
						}
					CompileText();
					break;
					}
				case 2: {
					mode_index = ( mode_index == 0 ) ? 1 : 0;
					if ( mode_index == 0 ) {
						Settings->SetSetting<bool>( "UseSpecials", true );
						mode0->SetTextColor( hilite );
						mode1->SetTextColor( white );
						}
					else {
						Settings->SetSetting<bool>( "UseSpecials", false );
						mode0->SetTextColor( white );
						mode1->SetTextColor( hilite );
						}
					CompileText();
					break;
					}
				case 3: {
					std::string command = "left";
					callsign->HandleCommand( command );
					break;
					}
				case 4: {
					// manually decrease by 5%
					float range = sound_vol->End() - sound_vol->Start();
					sound_vol->SetValue( sound_vol->Value() - (range*0.05) );
					Settings->SetSetting<float>( "SoundFXVolume", sound_vol->Value() );
					LGC::Core->SetMasterFXVolume( sound_vol->Value() );
					sfx.Play();
					break;
					}
				}
			}
 		else if ( e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_UP) {
			// unselect current widget bank
			switch ( row_index ) {
				case 0: { back_button->SetTextColor( unselected_color ); break; }
				case 1: { diff_label->SetTextColor( unselected_color ); break; }
				case 2: { mode_label->SetTextColor( unselected_color ); break; }
				case 3: { callsign_label->SetTextColor( unselected_color ); callsign->UnFocus(); break; }
				case 4: { vol_label->SetTextColor( unselected_color ); break; }
				}

			// shuffle the index
			if ( e.key.keysym.sym == SDLK_DOWN )  {
				if ( ++row_index >= 5 ) { row_index = 0; }
				}
			else {
				row_index = ( row_index == 0 ) ? 4 : row_index - 1;
				}

			// select the next widget bank
			switch ( row_index ) {
				case 0: { back_button->SetTextColor( selected_color ); break; }
				case 1: { diff_label->SetTextColor( selected_color ); break; }
				case 2: { mode_label->SetTextColor( selected_color ); break; }
				case 3: { callsign_label->SetTextColor( selected_color ); callsign->Focus(); break; }
				case 4: { vol_label->SetTextColor( selected_color ); break; }
				}

			// if the row index is now longer on the callsign, unfocus
			if ( row_index != 3 ) { callsign->UnFocus(); }

 			return;
 			}
		}
	BasicWidget::ProcessEvent( e );
	}


void GameState_Config::HandlePressBack( Button* obj ) {
	LGC::Core->ChangeGameState( "TITLE_SCREEN" );
	}
void GameState_Config::HandleCallsign( TextBox* obj ) {
	Settings->SetSetting< std::string >( "Callsign", obj->Text() );
	}
void GameState_Config::HandleCallsignEnter( TextBox* obj ) {
	// don't let it refocus if it's manually selected
	if ( row_index == 3 ) { obj->Focus(); }
	HandleCallsign( obj );
	}

void GameState_Config::HandleDifficulty( Button* obj ) {
	static ColorQuad hilite = ColorQuad(0.35f,0.9f,0.5f,1.0f);
	static ColorQuad white = ColorQuad(1,1,1,1);

	diff0->SetTextColor( white );
	diff1->SetTextColor( white );
	diff2->SetTextColor( white );
	diff3->SetTextColor( white );

	if ( obj->Text() == "Training" ) {
		Settings->SetSetting<int>( "Difficulty", 0 );
		diff0->SetTextColor( hilite );
		diff_index = 0;
		}
	else if ( obj->Text() == "Normal" ) {
		Settings->SetSetting<int>( "Difficulty", 1 );
		diff1->SetTextColor( hilite );
		diff_index = 1;
		}
	else if ( obj->Text() == "Hard" ) {
		Settings->SetSetting<int>( "Difficulty", 2 );
		diff2->SetTextColor( hilite );
		diff_index = 2;
		}
	else if ( obj->Text() == "Punishing" ) {
		Settings->SetSetting<int>( "Difficulty", 3 );
		diff3->SetTextColor( hilite );
		diff_index = 3;
		}
	CompileText();
	}
void GameState_Config::HandleMode( Button* obj ) {
	static ColorQuad hilite = ColorQuad(0.35f,0.9f,0.5f,1.0f);
	static ColorQuad white = ColorQuad(1,1,1,1);
	if ( obj->Text() == "On" ) {
		Settings->SetSetting<bool>( "UseSpecials", true );
		mode0->SetTextColor( hilite );
		mode1->SetTextColor( white );
		mode_index = 0;
		}
	else {
		Settings->SetSetting<bool>( "UseSpecials", false );
		mode0->SetTextColor( white );
		mode1->SetTextColor( hilite );
		mode_index = 1;
		}
	CompileText();
	}
void GameState_Config::HandleSoundVol( float value, Slider* obj ) {
	Settings->SetSetting<float>( "SoundFXVolume", value );
	LGC::Core->SetMasterFXVolume( value );
	if ( !sfx.IsPlaying() ) { sfx.Play(); }
	}
void GameState_Config::HandleMusicVol( float value, Slider* obj ) {
	Settings->SetSetting<float>( "SoundFXVolume", value );
	LGC::Core->SetMasterMusicVolume( value );
	}

const GameState_Config_Factory GameState_Config_Factory::auto_register_me = GameState_Config_Factory();

