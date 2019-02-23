
#include "TextBlock.h"
#include <libLevGC/src/core/Fonts.h>

static const float DROPSHADOW_OFFSET = 1.0;

TextBlock::TextBlock( float x, float y, float w, float h, LGC::Font font, LGC::BasicWidget* parent ) :
	LGC::BasicWidget(x,y,w,h,parent),
	compressed(false),
	shadow_color( LGC::ColorQuad(0,0,0,1) )
	{
	this->font = font;
	}

TextBlock::~TextBlock() {}

void TextBlock::Draw() {
	if (opacity == 0) { return; }
	StartClipping();
	glPushMatrix();
		DrawFoundation();
		DrawBorder();
		DrawText( border_padding, border_padding, true );
		// scootch over and draw kids
		glTranslatef( Xpos(), Ypos(), 0 );
		engine.DrawAll();
	glPopMatrix();
	StopClipping();
	}

bool TextBlock::Poll() {
	LGC::BasicWidget::Poll();
	if ( !compressed ) {
		font.CompressText( texts, "textblock", Width() - (BorderPadding() * 2) - 10 );
		compressed = true;
		}
	return true;
	}

// only left-align is supported at this time
void TextBlock::DrawText( float x , float y, bool shadow ) {
	if (opacity == 0) { return; }
	float target_x = Xpos();
	float target_y = Ypos();
	glPushMatrix();
		if (shadow) {
			glTranslatef( target_x + x + DROPSHADOW_OFFSET, target_y + y + DROPSHADOW_OFFSET, 0 );
   glColor4f( shadow_color.R, shadow_color.G, shadow_color.B, shadow_color.A * opacity );
			font.UseStoredText( "textblock", 0, 0 );
			glTranslatef( -DROPSHADOW_OFFSET, -DROPSHADOW_OFFSET, 0 );
			glColor4f( text_color.R, text_color.G, text_color.B, text_color.A * opacity );
			font.UseStoredText( "textblock", 0, 0 );
			}
		else {
			glTranslatef( target_x + x, target_y + y, 0 );
			glColor4f( text_color.R, text_color.G, text_color.B, text_color.A * opacity );
			font.UseStoredText( "textblock", 0, 0 );;
			}
	glPopMatrix();
	}
