#include "CollectablesWidget.h"
#include <libLevGC/src/core/DrawingTools.h>
#include <libLevGC/src/core/ResourceMgr.h>
#include <libLevGC/src/core/CoreInterface.h>
#include <libLevGC/src/fx/ParticleSource.h>
#include <libLevGC/src/fx/light/LightFlare.h>
#include <string>
#include <sstream>

std::map< std::string, Texture > CollectablesWidget::textures;

CollectablesWidget::CollectablesWidget( float x, float y, BasicWidget* parent ) :
	LGC::BasicWidget(x,y,300,30,parent)
	{
	font = RM->GetFont("CollectablesFont", "CollectablesWidget");
	textures["bandaid"] = LGC::RM->GetTexture( "antidote_shape", "CollectablesWidget" );
	textures["heart"] = LGC::RM->GetTexture( "1up_shape", "CollectablesWidget" );
	textures["bomb"] = LGC::RM->GetTexture( "bomb_shape", "CollectablesWidget" );
	SetText("");

	hearts = 0;
	bandaids = 0;
	bombs = 0;

	countdowns.push_back( std::map< int, float >() );
	countdowns.push_back( std::map< int, float >() );
	countdowns.push_back( std::map< int, float >() );

	vpsace = 1.1f;
	icon_scale = 0.55f;
	spacing = 4.0f;
	SetHeight( (textures["bomb"].Height() * icon_scale * 3.0f) + (vpsace * 2.0f) );

	}

void CollectablesWidget::AddHeart() {
	hearts++;
	countdowns[0][hearts-1] = 0.6f;
	}

void CollectablesWidget::AddBandaid() {
	bandaids++;
	countdowns[1][bandaids-1] = 0.6f;
	}

void CollectablesWidget::AddBomb() {
	bombs++;
	countdowns[2][bombs-1] = 0.6f;
	}

bool CollectablesWidget::UseHeart() {
	if ( hearts ) {
		--hearts;
		BlowUpLast( textures["heart"], 0 );
		return true;
		}
	return false;
	}

bool CollectablesWidget::UseBandaid() {
	if ( bandaids ) {
		--bandaids;
		BlowUpLast( textures["bandaid"], 1 );
		return true;
		}
	return false;
	}

bool CollectablesWidget::UseBomb() {
	if ( bombs ) {
		--bombs;
		BlowUpLast( textures["bomb"], 2 );
		return true;
		}
	return false;
	}


void CollectablesWidget::BlowUpLast( Texture &t, int v ) {
	float tempy = 0;
	float tempx = 0;
	if ( v == 0 ) {
		tempy = Ypos() + t.Height() * icon_scale * 0.5f;
		tempx = XFar() - ((t.Width() * icon_scale * 0.5f) + (t.Width() * icon_scale * float(hearts)) + (spacing * float(hearts) ) );
		}
	else if ( v == 1 ) {
		tempy = (Ypos() + (t.Height() * icon_scale * 0.5f)) + (t.Height() * icon_scale * vpsace);
		tempx = XFar() - ((t.Width() * icon_scale * 0.5f) + (t.Width() * icon_scale * float(bandaids)) + (spacing * float(bandaids) ));
		}
	else {
		tempy = (Ypos() + (t.Height() * icon_scale * 0.5f)) + (t.Height() * icon_scale * vpsace * 2.0f);
		tempx = XFar() - ((t.Width() * icon_scale * 0.5f) + (t.Width() * icon_scale * float(bombs)) + (spacing * float(bombs) ));
		}
	Core->RegisterObject( new ParticleSourceSparkler_GlitterCloud( tempx, tempy, 50 ), 3 );
	Core->RegisterObject( new LightFlare( tempx, tempy, LightFlare::FLASH_BURN ), 3 );
	}


CollectablesWidget::~CollectablesWidget() {
	RM->DumpFont(font, "CollectablesWidget");
	for( std::map< std::string, Texture >::iterator iter = textures.begin(); iter != textures.end(); iter++ ) {
		LGC::RM->DumpTexture( iter->second, "CollectablesWidget" );
		}
	}

bool CollectablesWidget::Poll() {
	BasicWidget::Poll();
	static float anim_speed = 0.9f;
	for ( unsigned int i = 0; i < countdowns.size(); i++ ) {
		for( std::map< int, float >::iterator iter = countdowns[i].begin(); iter != countdowns[i].end(); iter++ ) {
			if ( iter->second > 0 ) {
				iter->second -= anim_speed * TimeFromLast();
				if ( iter->second < 0 ) { iter->second = 0; }
				}
			else if ( iter->second < 0 ) {
				iter->second += anim_speed * TimeFromLast();
				if ( iter->second > 0 ) { iter->second = 0; }
				}
			}
		}
	return true;
	}

void CollectablesWidget::Draw() {
// 	BasicWidget::Draw();

// 	float DROPSHADOW_OFFSET = 2.0f;
// 	float xprog = Xpos();
// 	float tempw = 0;
// 	float tempy = 0;
// 	std::stringstream ss;
//
// 	// HEARTS
// 	ss.str(""); // clear
// 	ss << this->hearts;
// 	textures["heart"].BlitRotoZoomFromCenter(
// 		xprog + (textures["heart"].Width() * icon_scale * 0.5f),
// 		YFar() - (textures["heart"].Height() * icon_scale * 0.5f),
// 		0,
// 		icon_scale
// 		);
// 	xprog += (textures["heart"].Width() * icon_scale) + spacing,
// 	glColor4f( 0,0,0,text_color.A * opacity );
// 	font.RenderFontToScreen( ss.str(), xprog + DROPSHADOW_OFFSET, Ypos() + DROPSHADOW_OFFSET );
// 	glColor4f( text_color.R, text_color.G, text_color.B, text_color.A * opacity );
// 	font.RenderFontToScreen( ss.str(), xprog, Ypos() );
// 	font.GetStringSize( tempw, tempy, ss.str() );
// 	xprog += tempw + spacing;
//
// 	xprog += spacing * 3.0f;
//
// 	// BANDAIDS
// 	ss.str(""); // clear
// 	ss << this->bandaids;
// 	textures["bandaid"].BlitRotoZoomFromCenter(
// 		xprog + (textures["bandaid"].Width() * icon_scale * 0.5f),
// 		YFar() - (textures["bandaid"].Height() * icon_scale * 0.5f),
// 		0,
// 		icon_scale
// 		);
// 	xprog += (textures["bandaid"].Width() * icon_scale) + spacing,
// 	glColor4f( 0,0,0,text_color.A * opacity );
// 	font.RenderFontToScreen( ss.str(), xprog + DROPSHADOW_OFFSET, Ypos() + DROPSHADOW_OFFSET );
// 	glColor4f( text_color.R, text_color.G, text_color.B, text_color.A * opacity );
// 	font.RenderFontToScreen( ss.str(), xprog, Ypos() );
// 	font.GetStringSize( tempw, tempy, ss.str() );
// 	xprog += tempw + spacing;
//
// 	xprog += spacing * 3.0f;
//
// 	// BOMBS
// 	ss.str(""); // clear
// 	ss << this->bombs;
// 	textures["bomb"].BlitRotoZoomFromCenter(
// 		xprog + (textures["bomb"].Width() * icon_scale * 0.5f),
// 		YFar() - (textures["bomb"].Height() * icon_scale * 0.5f),
// 		0,
// 		icon_scale
// 		);
// 	xprog += (textures["bomb"].Width() * icon_scale) + spacing,
// 	glColor4f( 0,0,0,text_color.A * opacity );
// 	font.RenderFontToScreen( ss.str(), xprog + DROPSHADOW_OFFSET, Ypos() + DROPSHADOW_OFFSET );
// 	glColor4f( text_color.R, text_color.G, text_color.B, text_color.A * opacity );
// 	font.RenderFontToScreen( ss.str(), xprog, Ypos() );
// 	font.GetStringSize( tempw, tempy, ss.str() );
// 	xprog += tempw + spacing;



	RepeatTexture( textures["heart"], hearts, Ypos() + (textures["heart"].Height() * icon_scale * 0.5f), 0 );

	RepeatTexture( textures["bandaid"], bandaids, (Ypos() + (textures["bandaid"].Height() * icon_scale * 0.5f)) + (textures["bandaid"].Height() * icon_scale * vpsace), 1 );

	RepeatTexture( textures["bomb"], bombs, (Ypos() + (textures["bomb"].Height() * icon_scale * 0.5f)) + (textures["bomb"].Height() * icon_scale * vpsace * 2.0f), 2 );

	}

void CollectablesWidget::RepeatTexture( Texture &t, int n, float y,int v ) {
	for ( int i = 0; i < n; i++ ) {
		float scale = icon_scale;
		if ( countdowns[v][i] != 0 ) {
			float ratio = countdowns[v][i] / 0.6f;
			scale = (15.0f * ratio) + icon_scale;
			SetColor( 1,1,1, (1.0f - ratio) * opacity );
			}
		else {
			SetColor( 1,1,1,opacity );
			}
		t.BlitRotoZoomFromCenter(
			XFar() - ((t.Width() * icon_scale * 0.5f)
				+ (t.Width() * icon_scale * float(i))
				+ (spacing * float(i) ))
				,
			y,
			0,
			scale
			);
		}
	SetColor( 1,1,1,1 ); // reset
	}


