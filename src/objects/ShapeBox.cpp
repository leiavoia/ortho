#include "ShapeBox.h"
#include "Cursor.h"
#include <libLevGC/src/core/CoreInterface.h>
#include <libLevGC/src/core/InputHandler.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/core/extra_math.h>
#include <libLevGC/src/core/DrawingTools.h>
#include <libLevGC/src/fx/ParticleSource.h>
#include <libLevGC/src/fx/light/LightFlare.h>
#include <libLevGC/src/fx/Delay.h>
#include <libLevGC/src/etc/Settings.h>
#include <iostream>

std::map< std::string, Texture > ShapeBox::textures;
std::map< std::string, Sound > ShapeBox::sfx;
bool ShapeBox::img_loaded = false;
float ShapeBox::transition_speed = 5.0f; // = 1/x
float ShapeBox::funnymode_time = 0.5f; // = 1/x
float ShapeBox::spacing = 40.0f;
// Sound ShapeBox::laser_sound = Sound();



float ShapeBox::GetXCoord( unsigned int f, unsigned int r ) {
	switch ( dir ) {
		case   0: return Xpos() + (float(f) * spacing);
		case  90: return Xpos() + (float(r) * spacing);
		case 180: return Xpos() + (float(f) * spacing);
		case 270: return Xpos() + (((shapes[0].size()-1) - float(r)) * spacing);
		}
	return 0;
	}

float ShapeBox::GetYCoord( unsigned int f, unsigned int r ) {
	switch ( dir ) {
		case   0: return Ypos() + (((shapes[0].size()-1) - float(r)) * spacing);
		case  90: return Ypos() + (float(f) * spacing);
		case 180: return Ypos() + (float(r) * spacing);
		case 270: return Ypos() + (float(f) * spacing);
		}
	return 0;
	}


ShapeBox::ShapeBox( float x, float y, int files, int ranks, unsigned int dir ) : LGC::PhysicalObject( x, y )  {
	this->dir = dir;
// 	LGC::RM->ReserveResourcesFor("ShapeBox");
	LGC::RM->ReserveResourcesFor("LightFlare");
	// create the shapes array
	shapes = std::vector< std::vector<ShapeType>  >( files );
	for ( unsigned int x = 0; x < shapes.size(); x++ ) {
		shapes[x] = std::vector<ShapeType>( ranks );
		}
	// NULL all positions - TODO move this to above
	for ( unsigned int x = 0; x < shapes.size(); x++ ) {
		for ( unsigned int y = 0; y < shapes[x].size(); y++ ) {
			shapes[x][y] = SHAPE_NULL;
			}
		}
	transition_files = std::vector<bool>( files, false );
	transition_warning_times = std::vector<float>( files, 1.0 ) ;
	transition_progress = std::vector<float>( files, 0 ) ;
	transition_countdowns = std::vector<float>( files, 0 ) ;
	funnymode_countdown = 0;
	cancelled = 0;
	int difficulty = Settings->GetSetting<int>("Difficulty");
	if ( difficulty == 0 ) { num_shapes = 3; }
	else if ( difficulty > 1 ) { num_shapes = 5; }
	else { num_shapes = 4; }
	}

ShapeBox::~ShapeBox() {
// 	LGC::RM->DumpResourcesFor("ShapeBox");
	LGC::RM->DumpResourcesFor("LightFlare");
	}

Vector ShapeBox::ShootAt(
	unsigned int file,
	Cursor* cursor,
	int &pts,
	int &kills,
	float &mult,
	ShapeType &last_chain_color,
	int &color_chains
	) {

	Vector v = Vector(0,0);

	unsigned int ranks = shapes[file].size();
	kills = 0;
	pts = 0;
	mult = 0.0f;

	for ( unsigned int r = 1 /* 0 is the game ender */; r < ranks; r++ ) {
		// blanks
		if ( shapes[file][r] == SHAPE_NULL ) {
			// end of file, rurn around
			if ( r == ranks-1 ) {
				v = Vector( GetXCoord(file,r), GetYCoord(file,r) );
				}
			// skip blanks
			else { continue; }
			}
		// handle specials differently
		else if ( shapes[file][r] > SHAPE_PURPLE ) {
			switch ( shapes[file][r] ) {
				case SHAPE_FAST: 		{ LGC::Core->SendMessage("speedup"); break; }
				case SHAPE_SLOW: 		{ LGC::Core->SendMessage("slowdown"); break; }
				case SHAPE_HALF: 		{ LGC::Core->SendMessage("half_score"); break; }
				case SHAPE_X2:   		{ LGC::Core->SendMessage("double_score"); break; }
				case SHAPE_X3:   		{ LGC::Core->SendMessage("triple_score"); break; }
				case SHAPE_RANDOMIZE: 	{ LGC::Core->SendMessage("randomize"); break; }
				case SHAPE_REVERSE: 	{ LGC::Core->SendMessage("reverse"); break; }
				case SHAPE_1UP: 		{ LGC::Core->SendMessage("acquire_1up"); break; }
				case SHAPE_BOMB: 		{ LGC::Core->SendMessage("acquire_bomb"); break; }
				case SHAPE_ANTIDOTE: 	{ LGC::Core->SendMessage("acquire_antidote"); break; }
				case SHAPE_CLEARSCREEN:   {
					LGC::Core->SendMessage("clear_screen"); break;
					// STOP HERE!
					return Vector( GetXCoord(file,r), GetYCoord(file,r) );
					}
				default: ;;
				}
			Core->RegisterObject( new ParticleSourceSparkler_GlitterCloud( GetXCoord(file,r), GetYCoord(file,r), 200 ), 3 );
			Core->RegisterObject( new LightFlare( GetXCoord(file,r), GetYCoord(file,r), LightFlare::FLASH_BURN ), 1 );
			shapes[file][r] = SHAPE_NULL;
			kills++;
			pts += kills;
			// end of file, rurn around
			if ( r == ranks-1 ) {
				v = Vector( GetXCoord(file,r), GetYCoord(file,r) );
				}
			}
		// matching color?
		else if ( shapes[file][r] == cursor->Type() ) {
			Core->RegisterObject( new ParticleSourceSparkler_GlitterCloud( GetXCoord(file,r), GetYCoord(file,r), 200 ), 3 );
			Core->RegisterObject( new LightFlare( GetXCoord(file,r), GetYCoord(file,r), LightFlare::FLASH_BURN ), 1 );
			kills++;
			pts += kills;
			// check for color chains
			if ( last_chain_color == shapes[file][r] ) {
				color_chains++;
				if ( color_chains > 10 ) {
					mult += float(color_chains) * 0.01f;
					}
				}
			else {
				last_chain_color = shapes[file][r];
				color_chains = 1;
				}
			// end of file, rurn around
			if ( r == ranks-1 ) {
				v = Vector( GetXCoord(file,r), GetYCoord(file,r) );
				}
			shapes[file][r] = SHAPE_NULL;
			}
		// different color - swap
		else {
			ShapeType temp = shapes[file][r];
			shapes[file][r] = cursor->Type();
			cursor->Type(temp);
			// return hit point
			v = Vector( GetXCoord(file,r), GetYCoord(file,r) );
			sfx["swap"].Play();
			break;
			}
		}

	// bonus points for file-clear
	if ( kills == 5 && ranks-1 == 5 ) {
		pts = 50;
		int r = ranks /2;
		Core->RegisterObject( new ParticleSourceFirework( GetXCoord(file,r), GetYCoord(file,r), 800 ), 4 );
		Core->RegisterObject( new LightFlare( GetXCoord(file,r), GetYCoord(file,r), LightFlare::FLASH_BLINDER ), 4);
		sfx["clear5"].Play();
		}
	else if ( kills == 7 && ranks-1 == 7 ) {
		pts = 100;
		int r = ranks /2;
		Core->RegisterObject( new ParticleSourceFirework( GetXCoord(file,r), GetYCoord(file,r), 800 ), 4 );
		Core->RegisterObject( new LightFlare( GetXCoord(file,r), GetYCoord(file,r), LightFlare::FLASH_BLINDER ), 4);
		sfx["clear7"].Play();
		}
	// regular sound fx
	else {
		switch ( kills ) {
			case 1: sfx["clear_shape1"].Play(); break;
			case 2: sfx["clear_shape2"].Play(); break;
			case 3: sfx["clear_shape3"].Play(); break;
			case 4: sfx["clear_shape4"].Play(); break;
			case 5: // fall
			case 6: sfx["clear_shape5"].Play(); break;
			default: ;;
			}
		}

	return v;
	}


void ShapeBox::AddRandomShape_impl( unsigned int f ) {

	// scoot everyone down
	for ( unsigned int r = 0; r < shapes[f].size()-1; r++ ) {
		shapes[f][r] = shapes[f][r+1];
		// specials rotate
		if ( shapes[f][r] > SHAPE_PURPLE ) {
			shapes[f][r] = ShapeType( RandomNum( (SHAPE_MAX - (SHAPE_PURPLE+1)) ) + SHAPE_PURPLE + 1 );
			// reroll on hardest difficulty if they get a good shape
			if ( Settings->GetSetting<int>("Difficulty") == 3 && shapes[f][r] >= SHAPE_SLOW ) {
				shapes[f][r] = ShapeType( RandomNum( (SHAPE_MAX - (SHAPE_PURPLE+1)) ) + SHAPE_PURPLE + 1 );
				}
			}
		}

	// Create a random shape in the last rank.
	// 50% chance of getting a similar one.
	if ( shapes[f][ shapes[f].size()-1 ] != SHAPE_NULL
		&& shapes[f][ shapes[f].size()-1 ] <= SHAPE_PURPLE
		&& RandomNum(2) == 0 ) {
		shapes[f][ shapes[f].size()-1 ] = shapes[f][ shapes[f].size()-2 ];
		}
	else {
		// chance of getting a special
		if ( Settings->GetSetting<bool>("UseSpecials")
			&& RandomNum( Settings->GetSetting<int>("SpecialChance") ) == 0
			) {
			shapes[f][ shapes[f].size()-1 ] = ShapeType( RandomNum( (SHAPE_MAX - (SHAPE_PURPLE+1)) ) + SHAPE_PURPLE + 1 );
			// reroll on hardest difficulty if they get a good shape
			if ( Settings->GetSetting<int>("Difficulty") == 3 && shapes[f][ shapes[f].size()-1 ] >= SHAPE_SLOW ) {
				shapes[f][ shapes[f].size()-1 ] = ShapeType( RandomNum( (SHAPE_MAX - (SHAPE_PURPLE+1)) ) + SHAPE_PURPLE + 1 );
				}
			}
		// regular shapes
		else {
			shapes[f][ shapes[f].size()-1 ] = (ShapeType)( RandomNum( num_shapes )+1 );
			}
		}

	// check for game over
	if ( shapes[f][0] != SHAPE_NULL ) {
		// specials just disappear
		if ( shapes[f][0] > SHAPE_PURPLE ) {
			float newdir = dir + 90;
			if ( newdir >= 360 ) { newdir = 0; }
			Core->RegisterObject( new ParticleSourceMotionPartyBomb( GetXCoord(f,0), GetYCoord(f,0), 120, 1150, newdir ), 3 );
			Core->RegisterObject( new LightFlare(GetXCoord(f,0), GetYCoord(f,0), LightFlare::FLASH_BURST ), 3 );
			Core->RegisterObject( new LightFlare( GetXCoord(f,0), GetYCoord(f,0), LightFlare::FLASH_BURN ), 3 );
			Core->RegisterObject( new ParticleSourceSparkler_GlitterCloud( GetXCoord(f,0), GetYCoord(f,0), 200 ), 3 );
			shapes[f][0] = SHAPE_NULL;
			sfx["pop_special"].Play();
			}
		else {
			Core->SendMessage( "gameover" );
			}
		}

	// do transition
	transition_progress[f] = spacing;
	}


void ShapeBox::AddRandomShape( float warning_time ) {
	// hilite the next file to get an additional shape.
	// First, find which ones are not occupied
	std::vector<unsigned int> open;
	for ( unsigned int i = 0; i < shapes.size(); i++ ) {
		if ( transition_files[i] == false ) { open.push_back(i); }
		}
	// if there are any open, assign it
	if ( open.size() > 0 ) {
		int file = RandomNum( open.size() ); // pick a random file
		transition_files[ open[file] ] = true;
		if ( warning_time ) {
			transition_countdowns[ open[file] ] = warning_time;
			transition_warning_times[ open[file] ] = warning_time;
			}
		else { // allowing zero is okay - skip germination entirely
			AddRandomShape_impl( open[file] );
			}
		}
	}


bool ShapeBox::Poll() {
	if ( funnymode_countdown > 0 ) {
		funnymode_countdown -= TimeFromLast();
		if ( funnymode_countdown < 0 ) {
			funnymode_countdown = 0;
			}
		}
	for ( unsigned int i = 0; i < transition_files.size(); i++ ) {
		if ( transition_files[i] == true ) {
			if ( transition_countdowns[i] > 0 ) {
				transition_countdowns[i] -= TimeFromLast();
				if ( transition_countdowns[i] < 0 ) {
					transition_countdowns[i] = 0;
					AddRandomShape_impl( i );
					}
				}
			else if ( transition_progress[i] > 0 ) {
				transition_progress[i] -= spacing * transition_speed * TimeFromLast();
				if ( transition_progress[i] < 0 ) {
					transition_progress[i] = spacing;
					transition_files[i] = false;
					}
				}
			}
		}
	// Look to play or stop file warning sound
	bool gotcha = false;
	bool high = false;
	for ( unsigned int x = 0; x < shapes.size(); x++ ) {
		if ( shapes[x][1] != SHAPE_NULL && shapes[x][1] <= SHAPE_PURPLE && !cancelled ) {
			gotcha = true;
			high = transition_countdowns[x] > 0 ? true : high;
			// break; // don't break, need to check each file for impending doom.
			}
		}
	FileWarningSound( dir, gotcha, high );

	return true;
	}

// static prevents all 4 boxes from using the same sound
void ShapeBox::FileWarningSound( int dir, bool x, bool high ) {
	static std::map<int,bool> memory_low;
	static std::map<int,bool> memory_high;
	memory_low[dir] = x;
	memory_high[dir] = high;

	bool gotcha_high = false;
	for ( std::map<int,bool>::iterator i = memory_high.begin(); i != memory_high.end(); i++ ) {
		if ( i->second ) { gotcha_high = true; break; }
		}
	// check for high sound if needed
	if ( gotcha_high ) {
		if ( sfx["file_warn"].IsPlaying() ) {
			sfx["file_warn"].Stop();
			}
		if ( !sfx["file_warn_high"].IsPlaying() ) {
			sfx["file_warn_high"].Play( 30, -1 ); // loop continually
			}
		}
	// otherwise check for low sound
	else {
		// turn off high sound
		if ( sfx["file_warn_high"].IsPlaying() ) {
			sfx["file_warn_high"].Stop(); // slight fadeout
			}
		// enable low sound if needed
		bool gotcha_low = false;
		for ( std::map<int,bool>::iterator i = memory_low.begin(); i != memory_low.end(); i++ ) {
			if ( i->second ) { gotcha_low = true; break; }
			}
		if ( gotcha_low ) {
			if ( !sfx["file_warn"].IsPlaying() ) {
				sfx["file_warn"].Play( 30, -1 ); // loop continually
				}
			}
		else {
			if ( sfx["file_warn"].IsPlaying() ) {
				sfx["file_warn"].Stop( 0.2f ); // slight fadeout
				}
			}
		}
	}

void ShapeBox::Clear() {
	// clear everything out
	for ( unsigned int x = 0; x < shapes.size(); x++ ) {
		for ( unsigned int y = 0; y < shapes[x].size(); y++ ) {
			shapes[x][y] = SHAPE_NULL;
			if ( y > 0 ) {
				float _x = GetXCoord(x,y);
				float _y = GetYCoord(x,y);
				float angle = RadToDeg( atan2( _y - 300, _x - 400 ) );
				Core->RegisterObject( new Delay( new ParticleSourceSparkler_GlitterCloud( GetXCoord(x,y), GetYCoord(x,y), 10 ), (float(y)*0.08), 4) , 4 );
				Core->RegisterObject( new Delay( new LightFlare( _x, _y, LightFlare::FLASH_BURN ), (float(y)*0.08), 4), 4 );
				Core->RegisterObject( new Delay( new ParticleSourceMotionExplosion( _x, _y, 20, 400, angle ), (float(y)*0.08), 4), 4);
				}
			}
		}
	// do some cool explosions
	for ( int n = 0; n < 3; n++ ) {
		float randtime = float(n) * float( RandomNum(5) ) * 0.1f;
		int randfile = RandomNum( shapes.size() );
		int randrank = RandomNum( shapes[0].size()-1 ) + 1;
		float _x = GetXCoord( randfile, randrank );
		float _y = GetYCoord( randfile, randrank );
		Core->RegisterObject( new Delay( new ParticleSourcePartyBomb( _x, _y, 200 ), randtime, 4 ), 4 );
		Core->RegisterObject( new Delay( new LightFlare( _x, _y, LightFlare::FLASH_BURST ), randtime, 4 ), 4 );
		}
	}

void ShapeBox::ClearFirstRank() {
	for ( unsigned int x = 0; x < shapes.size(); x++ ) {
		if ( shapes[x][0] != SHAPE_NULL ) {
			float newdir = dir + 90;
			if ( newdir >= 360 ) { newdir = 0; }
			Core->RegisterObject( new ParticleSourceMotionPartyBomb( GetXCoord(x,0), GetYCoord(x,0), 300, 1450, newdir ), 3 );
			Core->RegisterObject( new LightFlare(GetXCoord(x,0), GetYCoord(x,0), LightFlare::FLASH_BURST ), 3 );
			Core->RegisterObject( new LightFlare( GetXCoord(x,0), GetYCoord(x,0), LightFlare::FLASH_BURN ), 3 );
			Core->RegisterObject( new ParticleSourceSparkler_GlitterCloud( GetXCoord(x,0), GetYCoord(x,0), 400 ), 3 );
			shapes[x][0] = SHAPE_NULL;
			}
		}
	}

void ShapeBox::Randomize() {
	funnymode_countdown = funnymode_time;
	for ( unsigned int x = 0; x < shapes.size(); x++ ) {
		for ( unsigned int y = 1; y < shapes[x].size(); y++ ) {
			if ( shapes[x][y] != SHAPE_NULL ) {
				// chance of getting a special
				if ( Settings->GetSetting<bool>("UseSpecials")
					&& RandomNum( Settings->GetSetting<int>("SpecialChance") ) == 0
					) {
					shapes[x][y] = ShapeType( RandomNum( (SHAPE_MAX - (SHAPE_PURPLE+1)) ) + SHAPE_PURPLE + 1 );
					}
				// regular shapes
				else {
					int random_number = RandomNum( num_shapes );
					shapes[x][y] = (ShapeType)( random_number+1 );
					}
				};
			}
		}
	}


void ShapeBox::Cancel() {
	cancelled = true;
	// clear everything out
// 	for ( unsigned int i = 0; i < transition_warning_times.size(); i++ ) {
// 		transition_warning_times[i] = 1.0;
// 		}
	for ( unsigned int i = 0; i < transition_progress.size(); i++ ) {
		transition_progress[i] = 0;
		}
// 	for ( unsigned int i = 0; i < transition_files.size(); i++ ) {
// 		transition_files[i] = false;
// 		}
	for ( unsigned int i = 0; i < transition_countdowns.size(); i++ ) {
		transition_countdowns[i] = 0;
		}
	}

void ShapeBox::Draw() {

	for ( unsigned int x = 0; x < shapes.size(); x++ ) {


		// if this file is threatening, draw a warning light
		if ( shapes[x][1] != SHAPE_NULL && shapes[x][1] <= SHAPE_PURPLE && !cancelled) {
			if ( transition_countdowns[x] > 0 ) {
				float alpha = (sin( TimeFromBirth() * 40.0f ) + 1.0f ) ;
				SetColor( 1,1,1, alpha );
				}
			else {
				SetColor( 0.75f, 0.15f, 0.15f, (sin( TimeFromBirth() * 24.0f ) + 1.0f ) / 3.0f  );
				}
			float x1, y1, x2, y2;
			x1 = y1 = x2 = y2 = 0; // avoids compiler warning
			switch ( this->dir ) {
				case 0: {
					x1 = GetXCoord( x, 1 ) - (spacing/2.0f);
					x2 = x1 + spacing;
					y1 = GetYCoord( x, shapes[x].size()-1 ) - (spacing/2.0f);
					y2 = GetYCoord( x, 1 ) + (spacing/2.0f);
					break;
					}
				case 90: {
					x1 = GetXCoord( x, 1 ) - (spacing/2.0f);
					x2 = GetXCoord( x, shapes[x].size()-1 ) + (spacing/2.0f);
					y1 = GetYCoord( x, 1 ) - (spacing/2.0f);
					y2 = y1 + spacing;
					break;
					}
				case 180: {
					x1 = GetXCoord( x, 1 ) - (spacing/2.0f);
					x2 = x1 + spacing;
					y1 = GetYCoord( x, 1 ) - (spacing/2.0f);
					y2 = GetYCoord( x, shapes[x].size()-1 ) + (spacing/2.0f);
					break;
					}
				case 270: {
					x1 = GetXCoord( x, shapes[x].size()-1 ) - (spacing/2.0f);
					x2 = GetXCoord( x, 1 ) + (spacing/2.0f);
					y1 = GetYCoord( x, 1 ) - (spacing/2.0f);
					y2 = y1 + spacing;
					break;
					}
				}
			DrawQuadByCoords( x1, y1, x2, y2 );
			SetColor(1,1,1,1);
			}

		float y_offset = 0;
		float x_offset = 0;
		float rot = 0;
		float scale = 1.0;
		if ( transition_files[x] ) {

			// materializer effect
			float warning_percent = (transition_countdowns[x] / transition_warning_times[x]);
			if ( warning_percent ) {
				float alpha = 1.0f - warning_percent;
				float scale = 8.0f * warning_percent + 0.5f;
				SetColor( 1.0f, 1.0f, 1.0f, alpha / 2.0f );
				textures["sparkle_base"].BlitRotoZoomFromCenter( GetXCoord(x, shapes[0].size() ), GetYCoord(x, shapes[0].size() ), -warning_percent*90.0f, scale * 1.2f );
				textures["sparkle"].BlitRotoZoomFromCenter( GetXCoord(x, shapes[0].size() ), GetYCoord(x, shapes[0].size() ), warning_percent*90.0f, scale, BLEND_ADD );
				}
			SetColor( 1.0f, 1.0f, 1.0f, 1.0f); // reset

			// draw transition progress, if in transition
			if ( !transition_countdowns[x] && transition_progress[x] ) {
				switch ( dir ) {
					case   0:
						y_offset = -transition_progress[x];
						rot = (transition_progress[x] / spacing) * -360.0f;
						break;
					case  90:
						x_offset =  transition_progress[x];
						rot = (transition_progress[x] / spacing) * 360.0f;
						break;
					case 180:
						y_offset =  transition_progress[x];
						rot = (transition_progress[x] / spacing) * 360.0f;
						break;
					case 270:
						x_offset = -transition_progress[x];
						rot = (transition_progress[x] / spacing) * -360.0f;
						break;
					}
				}
			}

		// funny mode effect
		if ( funnymode_countdown ) {
			rot = (funnymode_countdown / funnymode_time) * 360.0f;
			scale = (funnymode_countdown / funnymode_time) + 1.0;
			SetColor( 1.0f, 1.0f, 1.0f, 1.0f-(funnymode_countdown / funnymode_time) ); // reset
			}

		for ( unsigned int y = 0; y < shapes[x].size(); y++ ) {
			ShapeType shape = shapes[x][y];
			switch ( shape ) {
				case SHAPE_RED: {
					textures["red_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_GREEN: {
					textures["green_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_BLUE: {
					textures["blue_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_YELLOW: {
					textures["yellow_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_PURPLE: {
					textures["purple_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_FAST: {
					textures["speed_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_HALF: {
					textures["half_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_X2: {
					textures["x2_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_X3: {
					textures["x3_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_BOMB: {
					textures["bomb_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_1UP: {
					textures["1up_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_ANTIDOTE: {
					textures["antidote_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_RANDOMIZE: {
					textures["dice_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_CLEARSCREEN: {
					textures["pop"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_REVERSE: {
					textures["reverse_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				case SHAPE_SLOW: {
					textures["slow_shape"].BlitRotoZoomFromCenter( GetXCoord(x,y) + x_offset, GetYCoord(x,y) + y_offset, rot, scale ); break;
					}
				default: ;; // do nothing
				}
			}
		}
//
	SetColor( 1.0f, 1.0f, 1.0f, 1.0f); // reset
	}





void ShapeBox::ReserveResources() {
	if ( !img_loaded ) {
		textures["blue_shape"] = LGC::RM->GetTexture( "blue_shape", "ShapeBox" );
		textures["red_shape"] = LGC::RM->GetTexture( "red_shape", "ShapeBox" );
		textures["green_shape"] = LGC::RM->GetTexture( "green_shape", "ShapeBox" );
		textures["yellow_shape"] = LGC::RM->GetTexture( "yellow_shape", "ShapeBox" );
		textures["purple_shape"] = LGC::RM->GetTexture( "purple_shape", "ShapeBox" );
		textures["speed_shape"] = LGC::RM->GetTexture( "speed_shape", "ShapeBox" );
		textures["half_shape"] = LGC::RM->GetTexture( "half_shape", "ShapeBox" );
		textures["x2_shape"] = LGC::RM->GetTexture( "x2_shape", "ShapeBox" );
		textures["x3_shape"] = LGC::RM->GetTexture( "x3_shape", "ShapeBox" );
		textures["sparkle_base"] = LGC::RM->GetTexture( "sparkle_base", "ShapeBox" );
		textures["sparkle"] = LGC::RM->GetTexture( "sparkle", "ShapeBox" );
		textures["pop"] = LGC::RM->GetTexture( "pop_shape", "ShapeBox" );
		textures["antidote_shape"] = LGC::RM->GetTexture( "antidote_shape", "ShapeBox" );
		textures["dice_shape"] = LGC::RM->GetTexture( "dice_shape", "ShapeBox" );
		textures["slow_shape"] = LGC::RM->GetTexture( "slow_shape", "ShapeBox" );
		textures["1up_shape"] = LGC::RM->GetTexture( "1up_shape", "ShapeBox" );
		textures["reverse_shape"] = LGC::RM->GetTexture( "reverse_shape", "ShapeBox" );
		textures["bomb_shape"] = LGC::RM->GetTexture( "bomb_shape", "ShapeBox" );

		sfx["clear5"] = LGC::RM->GetSound( "clear5", "ShapeBox" );
		sfx["clear7"] = LGC::RM->GetSound( "clear7", "ShapeBox" );
		sfx["file_warn"] = LGC::RM->GetSound( "file_warn", "ShapeBox" );
		sfx["file_warn_high"] = LGC::RM->GetSound( "file_warn_high", "ShapeBox" );
		sfx["swap"] = LGC::RM->GetSound( "swap", "ShapeBox" );
		sfx["clear_shape1"] = LGC::RM->GetSound( "clear_shape1", "ShapeBox" );
		sfx["clear_shape2"] = LGC::RM->GetSound( "clear_shape2", "ShapeBox" );
		sfx["clear_shape3"] = LGC::RM->GetSound( "clear_shape3", "ShapeBox" );
		sfx["clear_shape4"] = LGC::RM->GetSound( "clear_shape4", "ShapeBox" );
		sfx["clear_shape5"] = LGC::RM->GetSound( "clear_shape5", "ShapeBox" );
		sfx["pop_special"] = LGC::RM->GetSound( "pop_special", "ShapeBox" );

		img_loaded = true;
		}
	}

void ShapeBox::DumpResources() {
	if ( img_loaded ) {
		for( std::map< std::string, Texture >::iterator iter = textures.begin(); iter != textures.end(); iter++ ) {
			LGC::RM->DumpTexture( iter->second, "ShapeBox" );
			}
		for( std::map< std::string, Sound >::iterator iter = sfx.begin(); iter != sfx.end(); iter++ ) {
			LGC::RM->DumpSound( iter->second, "ShapeBox" );
			}
		img_loaded = false;
		}
// 	LGC::RM->DumpSound( laser_sound, "ShapeBox" );
	}


REGISTER_CLASS_AS_RESOURCE_CLIENT( ShapeBox )


