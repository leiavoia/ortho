#include "CursorLaser.h"
#include <libLevGC/src/etc/ColorCurve.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/core/DrawingTools.h>
#include <GL/gl.h>


Texture CursorLaser::light = Texture();
Sound CursorLaser::sfx = Sound();

CursorLaser::CursorLaser( float x1, float y1, float x2, float y2 ) {
	start = Vector( x1, y1 );
	end = Vector( x2, y2 );
	sfx.Play();
	}

CursorLaser::~CursorLaser() {
	}


bool CursorLaser::Poll() {
	return !StoppedPolling();
	}

void CursorLaser::Draw() {
	static LGC::ColorCurve* cc = NULL;
	const static float laser_speed = 0.22f;
	if ( !cc ) {
		cc = new LGC::ColorCurve( LGC::Interpolator::TRIANGLE );
		cc->AddPoint( 0.000f * laser_speed, 0.1f, 0.3f, 6.0f, 0.0f );
		cc->AddPoint( 0.125f * laser_speed, 0.1f, 0.3f, 6.0f, 1.0f );
		cc->AddPoint( 0.250f * laser_speed, 0.1f, 0.3f, 6.0f, 1.0f );
		cc->AddPoint( 0.375f * laser_speed, 0.1f, 0.3f, 6.0f, 0.0f );
		cc->AddPoint( 0.500f * laser_speed, 0.1f, 0.3f, 6.0f, 0.0f );
		cc->AddPoint( 0.625f * laser_speed, 0.1f, 0.3f, 6.0f, 0.0f );
		cc->AddPoint( 0.750f * laser_speed, 0.1f, 0.3f, 6.0f, 1.0f );
		cc->AddPoint( 0.875f * laser_speed, 0.1f, 0.3f, 6.0f, 1.0f );
		cc->AddPoint( 1.000f * laser_speed, 0.1f, 0.3f, 6.0f, 0.0f );
		}

	// we're done - bail out
	if ( TimeFromBirth() > laser_speed ) {
		StopPolling();
		return;
		}

// 	float _x;
// 	float _y;
//
// 	if ( TimeFromBirth() < laser_speed/2.0f ) {
// 		_x = start.x + (end.x - start.x) * ( TimeFromBirth() / (laser_speed/2.0f) ) ;
// 		_y = start.y + (end.y - start.y) * ( TimeFromBirth() / (laser_speed/2.0f) ) ;
// 		}
// 	else {
// 		_x = start.x + ( start.x - end.x) * ( (TimeFromBirth()-laser_speed) / (laser_speed/2.0f) ) ;
// 		_y = start.y + ( start.y - end.y) * ( (TimeFromBirth()-laser_speed) / (laser_speed/2.0f) ) ;
// 		}

// 	float rot = (TimeFromBirth() / laser_speed) * 360.0f;

	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_TEXTURE_BIT );
// 	SetColor( 1, 1, 1, 0.22f);
// 	light.BlitRotoZoomFromCenter( _x, _y, rot, 0.35f, BLEND_ADD );

	// not done yet - keep drawing
	glBindTexture( GL_TEXTURE_2D, 0 );
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glLineWidth(20);

	glBegin( GL_LINE_STRIP );
		glColor4f( cc->R(TimeFromBirth()), cc->G(TimeFromBirth()), cc->B(TimeFromBirth()), cc->A(TimeFromBirth()) );
		glVertex2f( start.x, start.y );
		glColor4f( cc->R(TimeFromBirth()+(laser_speed*0.5f)), cc->G(TimeFromBirth()+(laser_speed*0.5f)), cc->B(TimeFromBirth()+(laser_speed*0.5f)), cc->A(TimeFromBirth()+(laser_speed*0.5f)) );
		glVertex2f( end.x, end.y );
	glEnd();
//
	glPopAttrib();

	}



void CursorLaser::ReserveResources() {
	light = LGC::RM->GetTexture( "light_sphere_sharp", "CursorLaser" );
	sfx = LGC::RM->GetSound( "laser_sound", "CursorLaser" );
	}

void CursorLaser::DumpResources() {
	LGC::RM->DumpTexture( light, "CursorLaser" );
	LGC::RM->DumpSound( sfx, "CursorLaser" );
	}


REGISTER_CLASS_AS_RESOURCE_CLIENT( CursorLaser )
