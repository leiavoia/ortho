#include "ScoreWidget.h"
#include <libLevGC/src/core/DrawingTools.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/etc/Commaize.h>
#include <libLevGC/src/utilities/LogManager.h>

const float ScoreWidget::floatie_time = 2.2f;

ScoreWidget::ScoreWidget( float x, float y, BasicWidget* parent ) :
	LGC::Label(x,y,1,1,parent)
	{
	font = RM->GetFont("UILabelFont", "ScoreWidget");
	floatie_font = RM->GetFont("ScoreFloatieFont", "ScoreWidget");
	SetTextColor( LGC::ColorQuad(1,1,1,1) );
	SetTextAlign(TEXT_LEFT);
	SetText("0000000000.00");
	score = 0;
	if ( this->font.Valid() && text != "" ) {
		float w, h;
		this->font.GetStringSize(w,h,text);
		SetWidth( w + (BorderPadding()*2) + (BorderWidth()*2) );
		SetHeight( h + (BorderPadding()*2) + (BorderWidth()*2) );
		}
	else {
		SetFont( default_label_font );
		}
	SetText("0");
	draw_centered = false;
	enable_floaties = true;
	}

ScoreWidget::~ScoreWidget() {
	RM->DumpFont(font, "ScoreWidget");
	RM->DumpFont(floatie_font, "ScoreWidget");
	}

void ScoreWidget::AddToScore( float x ) {
	this->score += float(x);
	TimeScore ts;
	ts.time = floatie_time;
	ts.score = int(x);
	floaties.push_back( ts );
	SetText( commaize( int(this->score) ).c_str() );
	}

bool ScoreWidget::Poll() {
	LGC::Label::Poll();
	// Mark all the time progress down. Remove dead ones.
	if ( floaties.size() ) {
		for ( std::vector< TimeScore >::iterator i = floaties.begin(); i != floaties.end(); ) {
			i->time -= TimeFromLast();
			if ( i->time <= 0 ) {
				i = floaties.erase( i );
				}
			else { i++; }
			}
		}
	return true;
	}

void ScoreWidget::Draw() {
	Label::Draw();
	// draw floaties
	static float floatie_dist = 90.0f; // px/sec
	if ( enable_floaties ) {
		for ( unsigned int i = 0; i < floaties.size(); i++ ) {
			std::string t = commaize( floaties[i].score );
			float _y = floatie_dist - ((floaties[i].time / floatie_time) * floatie_dist) ;
			SetColor( 1,1,1, (floaties[i].time / floatie_time) );
			floatie_font.RenderFontToScreen(
				t,
				this->Xpos() ,
				this->YFar() + _y
				);
			}
		}
	SetColor( 1,1,1,1 ); // reset
	}

