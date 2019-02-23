
#ifndef TEXT_BLOCK_H
#define TEXT_BLOCK_H

#include <vector>
#include <string>
#include <libLevGC/src/core/Fonts.h>
#include <libLevGC/src/UI/widgets/BasicWidget.h>


class TextBlock: public LGC::BasicWidget  {
public:

	TextBlock( float x, float y, float w, float h, LGC::Font font, LGC::BasicWidget* parent=NULL );

	virtual ~TextBlock();

	virtual bool Poll();

	virtual void Draw();

	void AddLine( const char* str ) { texts.push_back( std::string(str) ); compressed = false; }
	void AddLine( std::string str ) { texts.push_back( str ); compressed = false; }
	void ClearText() { texts.clear(); compressed = false; }
	void SetWidth( float x ) { width = x; compressed = false; }
	void SetShadowColor( LGC::ColorQuad color ) { shadow_color = color; }
	void SetBorderPadding( float x ) { border_padding = x; compressed = false; }

protected:

	void DrawText( float x , float y, bool shadow );

	std::vector< std::string > texts;

	bool compressed;

	LGC::ColorQuad shadow_color;
	};


#endif








