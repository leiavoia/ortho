#include "Cursor.h"
#include "CursorLaser.h"
#include <libLevGC/src/core/CoreInterface.h>
#include <libLevGC/src/core/InputHandler.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/fx/light/LightFlare.h>
#include <libLevGC/src/fx/ParticleSource.h>
#include <libLevGC/src/etc/ColorCurve.h>
#include <libLevGC/src/core/DrawingTools.h>
#include <libLevGC/src/core/extra_math.h>
#include <libLevGC/src/etc/Settings.h>


Texture Cursor::img_blue = Texture();
Texture Cursor::img_green = Texture();
Texture Cursor::img_yellow = Texture();
Texture Cursor::img_red = Texture();
Texture Cursor::img_purple = Texture();
Texture Cursor::color_swap = Texture();
bool Cursor::img_loaded = false;
float Cursor::ghost_speed = 0.18f;
Sound Cursor::laser_sound = Sound();




Cursor::Cursor() : LGC::PhysicalObject( 0, 0 )  {
	LGC::RM->ReserveResourcesFor("Cursor");
	LGC::RM->ReserveResourcesFor("CursorLaser");
	type = SHAPE_BLUE;
	sprite = &img_blue;
	direction = 0;
	start = Vector(340, 240);
	MoveTo( 340, 240 );
// 	laser = NULL;
	xfile = 0;
	yfile = 0;
	trans_speed = 250.0;
	continue_moving_x = false;
	continue_moving_y = false;
	queud_cmd = EMPTY;
	move_instantly = Settings->GetSetting<bool>("SnapMode");
	}

Cursor::~Cursor() {
	LGC::RM->DumpResourcesFor("Cursor");
	LGC::RM->DumpResourcesFor("CursorLaser");
	}

void Cursor::Do( CursorCommand cmd ) {
	switch ( cmd ) {
		case HALT_X: {
			continue_moving_x = false;
			break;
			}
		case HALT_Y: {
			continue_moving_y = false;
			break;
			}
		case LEFT: {
			if ( move_instantly ) {
				AddGhosts( Xpos(), Ypos(), 270 );
				Snap( -40, 0 );
				this->direction = 270;
				xfile--;
				}
			else if ( !YMovement() && !XMovement() ) {
				this->direction = 270;
				AddImpulse( Vector( -trans_speed, 0 ) );
				xfile--;
				continue_moving_x = true;
				queud_cmd = EMPTY;
				}
			else if ( YMovement() ) { queud_cmd = LEFT; }
			break;
			}
		case RIGHT: {
			if ( move_instantly ) {
				AddGhosts( Xpos(), Ypos(), 90 );
				Snap( 40, 0 );
				this->direction = 90;
				xfile++;
				}
			else if ( !YMovement() && !XMovement() ) {
				this->direction = 90;
				AddImpulse( Vector( trans_speed, 0 ) );
				xfile++;
				continue_moving_x = true;
				queud_cmd = EMPTY;
				}
			else if ( YMovement() ) { queud_cmd = RIGHT; }
			break;
			}
		case UP: {
			if ( move_instantly ) {
				AddGhosts( Xpos(), Ypos(), 0 );
				Snap( 0, -40 );
				this->direction = 0;
				yfile--;
				}
			else if ( !YMovement() && !XMovement() ) {
				this->direction = 0;
				AddImpulse( Vector( 0, -trans_speed ) );
				yfile--;
				continue_moving_y = true;
				queud_cmd = EMPTY;
				}
			else if ( XMovement() ) { queud_cmd = UP; }
			break;
			}
		case DOWN: {
			if ( move_instantly ) {
				AddGhosts( Xpos(), Ypos(), 180 );
				Snap( 0, 40 );
				this->direction = 180;
				yfile++;
				}
			else if ( !YMovement() && !XMovement() ) {
				this->direction = 180;
				AddImpulse( Vector( 0, trans_speed ) );
				yfile++;
				continue_moving_y = true;
				queud_cmd = EMPTY;
				}
			else if ( XMovement() ) { queud_cmd = DOWN; }
			break;
			}
		case FIRE: {
		 	laser_sound.Play();
		 	ColorCurve cc;
		 	cc.AddPoint(0, 0.03f, 0.1f, 0.4f, 1.0f);
		 	cc.AddPoint(0.8f, 0.03f, 0.1f, 0.4f, 1.0f);
		 	cc.AddPoint(1, 0.03f, 0.1f, 0.4f, 0.0f);
			LGC::Core->RegisterObject(
				new LightFlare(
					Xpos(),
					Ypos(),
					0,
					25.0f,
					50.0f,
					0.2f,
					0.4f,
					cc,
					color_swap // custom texture
					),
				4 // UI Layer
				);
			LGC::Core->RegisterObject(
				new LightFlare(
					Xpos(),
					Ypos(),
					20,
					35.0f,
					60.0f,
					0.15f,
					0.28f,
					cc,
					color_swap // custom texture
					),
				4 // UI Layer
				);
			LGC::Core->RegisterObject(
				new ParticleSourceSparkler_GlitterCloud( Xpos(), Ypos(), 20 ),
				4 // UI Layer
				);

		 	break;
		 	}
		default: ;;
		}
	// clamp to center
	if ( Xpos() < 340 ) { MoveTo( 340, Ypos() ); StopMovement(); }
	if ( Xpos() > 460 ) { MoveTo( 460, Ypos() ); StopMovement(); }
	if ( Ypos() < 240 ) { MoveTo( Xpos(), 240 ); StopMovement(); }
	if ( Ypos() > 360 ) { MoveTo( Xpos(), 360 ); StopMovement(); }
	xfile = Clamp<int>( xfile, 0, 3 );
	yfile = Clamp<int>( yfile, 0, 3 );
	}


bool Cursor::Poll() {
	if ( queud_cmd != EMPTY ) { this->Do( queud_cmd ); }

	UpdatePosition( TimeFromLast() );
	if ( direction == 270 && Xpos() < (start.x + (xfile*40.0)) ) {
		if ( continue_moving_x && xfile > 0 ) { xfile--; }
		else { MoveTo( (start.x + (xfile*40.0)), Ypos() ); StopMovement(); }
		}
	else if ( direction == 90  && Xpos() > (start.x + (xfile*40.0)) ) {
		if ( continue_moving_x && xfile < 3  ) { xfile++; }
		else { MoveTo( (start.x + (xfile*40.0)), Ypos() ); StopMovement(); }
		}
	else if ( direction == 0   && Ypos() < (start.y + (yfile*40.0)) ) {
		if ( continue_moving_y && yfile > 0  ) { yfile--; }
		else { MoveTo( Xpos(), (start.y + (yfile*40.0)) ); StopMovement(); }
		}
	else if ( direction == 180 && Ypos() > (start.y + (yfile*40.0)) ) {
		if ( continue_moving_y && yfile < 3  ) { yfile++; }
		else { MoveTo( Xpos(), (start.y + (yfile*40.0)) ); StopMovement(); }
		}

	for (std::list<Ghost>::iterator i = ghosts.begin(); i != ghosts.end();) {
		i->timeout -= TimeFromLast();
		if (  i->timeout <= 0  ) { i = ghosts.erase(i); }
		else { ++i; }
		}

	return true;
	}

void Cursor::Type( ShapeType t ) {
 	this->type = t;

 	ColorCurve cc;
 	ColorQuad color;
	switch ( t ) {
		case SHAPE_BLUE: {
			this->sprite = &img_blue;
			color = ColorQuad(0.03f, 0.1f, 0.4f, 1.0);
			break;
			}
		case SHAPE_RED: {
			this->sprite = &img_red;
			color = ColorQuad(0.4f, 0.1f, 0.04f, 1.0);
			break;
			}
		case SHAPE_GREEN: {
			this->sprite = &img_green;
			color = ColorQuad(0.04f, 0.3f, 0.1f, 1.0);
			break;
			}
		case SHAPE_YELLOW: {
			this->sprite = &img_yellow;
			color = ColorQuad(0.3f, 0.3f, 0.05f, 0.6f);
			break;
			}
		case SHAPE_PURPLE: {
			this->sprite = &img_purple;
			color = ColorQuad(0.8f, 0.26f, 0.58f, 1.0);
			break;
			}
		default: ;;
		}
	cc.AddPoint(0, 	color.R, color.G, color.B, 1.0f);
	cc.AddPoint(0.8f,	color.R, color.G, color.B, 1.0f);
	cc.AddPoint(1,  	color.R, color.G, color.B, 0.0f);
	LGC::Core->RegisterObject(
		new LightFlare(
			Xpos(),
			Ypos(),
			0,
			25.0f,
			50.0f,
			0.2f,
			0.4f,
			cc,
			color_swap // custom texture
			),
		4 // UI Layer
		);
	LGC::Core->RegisterObject(
		new LightFlare(
			Xpos(),
			Ypos(),
			20,
			35.0f,
			60.0f,
			0.15f,
			0.28f,
			cc,
			color_swap // custom texture
			),
		4 // UI Layer
		);

 	}


void Cursor::Draw() {
	// draw ghosts
	for (std::list<Ghost>::iterator i = ghosts.begin(); i != ghosts.end();) {
		glColor4f( 1,1,1, (i->timeout / ghost_speed) );
		sprite->BlitRotoZoomFromCenter( i->x, i->y, float(i->dir) );
		++i;
		}
	glColor4f( 1,1,1,1 );
	sprite->BlitRotoZoomFromCenter( Xpos(), Ypos(), float(direction) );
	}


void Cursor::AddGhosts( float x, float y, int dir ) {
	// no ghosting if attempting to move past the edge
	if (
		(dir == 0   && yfile == 0) ||
		(dir == 90  && xfile == 3) ||
		(dir == 180 && yfile == 3) ||
		(dir == 270 && xfile == 0)
		)
	{ return;  }

	float xsign = 0;
	float ysign = 0;
	switch ( dir ) {
		case 0:   ysign = -1.0; break;
		case 90:  xsign =  1.0; break;
		case 180: ysign =  1.0; break;
		case 270: xsign = -1.0; break;
		default:;;
		}
	for ( int i = 0; i < 5; i++ ) {
		Ghost g;
		g.x = x + (xsign * 40.0f * float(i) * 0.2f );
		g.y = y + (ysign * 40.0f * float(i) * 0.2f );
		g.dir = dir;
		g.timeout = ghost_speed * ((float(i) * 0.2f) + 0.2f);
		ghosts.push_back(g);
		}
	}


void Cursor::ShootAt( Vector v ) {
	Core->RegisterObject(
		new CursorLaser( Xpos(), Ypos(), v.x, v.y ),
		0
		);
	}


void Cursor::ReserveResources() {
	img_blue = LGC::RM->GetTexture( "blue_ptr", "Cursor" );
	img_green = LGC::RM->GetTexture( "green_ptr", "Cursor" );
	img_yellow = LGC::RM->GetTexture( "yellow_ptr", "Cursor" );
	img_red = LGC::RM->GetTexture( "red_ptr", "Cursor" );
	img_purple = LGC::RM->GetTexture( "purple_ptr", "Cursor" );
	color_swap = LGC::RM->GetTexture( "inverted_lightflare", "Cursor" );
	if (	img_blue.IsValid() && img_green.IsValid() &&
		img_yellow.IsValid() && img_red.IsValid() &&
		color_swap.IsValid()
		) {
		img_loaded = true;
		}
	laser_sound =  LGC::RM->GetSound( "laser_sound", "Cursor" );
	}

void Cursor::DumpResources() {
	LGC::RM->DumpTexture( img_blue, "Cursor" );
	LGC::RM->DumpTexture( img_green, "Cursor" );
	LGC::RM->DumpTexture( img_yellow, "Cursor" );
	LGC::RM->DumpTexture( img_red, "Cursor" );
	LGC::RM->DumpTexture( img_purple, "Cursor" );
	LGC::RM->DumpTexture( color_swap, "Cursor" );
	img_loaded = false;
	LGC::RM->DumpSound( laser_sound, "Cursor" );
	}


REGISTER_CLASS_AS_RESOURCE_CLIENT( Cursor )


