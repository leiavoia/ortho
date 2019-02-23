#include "MultiplierWidget.h"
#include <libLevGC/src/core/DrawingTools.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/etc/Settings.h>
#include <sstream>
#include <string>
#include <iomanip>

MultiplierWidget::MultiplierWidget( float x, float y, BasicWidget* parent ) :
	LGC::Label(x,y,1,1,parent)
	{
	font = RM->GetFont("UILabelFont", "MultiplierWidget");
	SetTextAlign(TEXT_LEFT);
	SetTextColor( LGC::ColorQuad( 0.35f, 0.55f, 0.9f, 1.0f ) );
	SetText("x000.00");
	mult = 1.0f;
	if ( this->font.Valid() && text != "" ) {
		float w, h;
		this->font.GetStringSize(w,h,text);
		SetWidth( w + (BorderPadding()*2) + (BorderWidth()*2) );
		SetHeight( h + (BorderPadding()*2) + (BorderWidth()*2) );
		}
	else {
		SetFont( default_label_font );
		}
	SetText("x1.00");
	draw_centered = false;
	do_adjustments = true;
	}

MultiplierWidget::~MultiplierWidget() {
	RM->DumpFont(font, "MultiplierWidget");
	}

bool MultiplierWidget::Poll() {
	LGC::Label::Poll();
	static bool setup_speed = false;
	static float mult_speed_down = 0;
	static float mult_speed_up = 0;
	if ( !setup_speed ) {
		mult_speed_down = Settings->GetSetting<float>("MultSpeedDown");
		mult_speed_up = Settings->GetSetting<float>("MultSpeedUp");
		mult_speed_down = mult_speed_down ? mult_speed_down : 0.012; // per second
		mult_speed_up = mult_speed_up ? mult_speed_up : 0.02f; // 0.02 takes about 1:00 from 0.5->1.0
		setup_speed = true;
		}
	if ( do_adjustments ) {
		if ( mult < 1.0f ) {
			mult += TimeFromLast() * mult_speed_up * (1.2f - mult);
			if ( mult > 1.0f ) { mult = 1.0f; }
			}
		else if ( mult > 1.0f ) {
			mult -= TimeFromLast() * mult_speed_down * mult;
			if ( mult < 1.0f ) { mult = 1.0f; }
			}
		std::stringstream mss;
		mss << 'x' << std::fixed << std::setprecision( 2 ) << mult;
		std::string mstr = mss.str();
		SetText( mstr );
		}
	return true;
	}

void MultiplierWidget::Draw() {
	Label::Draw();
// 	SetColor( 1,1,1,1 ); // reset
	}

