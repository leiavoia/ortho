#include "StatusEffectWidget.h"
#include <libLevGC/src/core/DrawingTools.h>
#include <libLevGC/src/core/ResourceMgr.h>


std::map< std::string, Texture > StatusEffectWidget::textures;

StatusEffectWidget::StatusEffectWidget( float x, float y, BasicWidget* parent ) :
	LGC::BasicWidget(x,y,150,150,parent)
	{
	textures["slowdown"] = LGC::RM->GetTexture( "slow_shape", "StatusEffectWidget" );
	textures["speedup"] = LGC::RM->GetTexture( "speed_shape", "StatusEffectWidget" );
	textures["reverse"] = LGC::RM->GetTexture( "reverse_shape", "StatusEffectWidget" );
	}

StatusEffectWidget::~StatusEffectWidget() {
	for( std::map< std::string, Texture >::iterator iter = textures.begin(); iter != textures.end(); iter++ ) {
		LGC::RM->DumpTexture( iter->second, "StatusEffectWidget" );
		}
	}

void StatusEffectWidget::ActivateEffect( std::string effect, float time ) {
	countdowns[effect] = time;
	}

bool StatusEffectWidget::Poll() {
	BasicWidget::Poll();
	static float anim_speed = 0.9f;
	for( std::map< std::string, float >::iterator iter = countdowns.begin(); iter != countdowns.end(); iter++ ) {
		if ( iter->second > 0 ) {
			iter->second -= anim_speed * TimeFromLast();
			if ( iter->second < 0 ) { iter->second = 0; }
			}
		}
	return true;
	}

void StatusEffectWidget::Draw() {
	float xoffset = Xpos();
	float size = 80.0f;
	for( std::map< std::string, float >::iterator iter = countdowns.begin(); iter != countdowns.end(); iter++ ) {
		if ( iter->second > 0 ) {
			SetColor( 1,1,1, (sin( TimeFromBirth() * 10.0f ) + 1.0f) * 0.5f * opacity );
			textures[iter->first].BlitCustomStretch( xoffset, Ypos(), size, size );
			xoffset += size + (size * 0.125f);
			}
		}
	}
