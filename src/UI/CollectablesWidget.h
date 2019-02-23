#ifndef CollectablesWidget_H
#define CollectablesWidget_H

#include <libLevGC/src/UI/widgets/Label.h>
#include <libLevGC/src/core/Fonts.h>
#include <libLevGC/src/core/Texture.h>
#include <map>
#include <vector>

using namespace LGC;

class CollectablesWidget: public LGC::BasicWidget {
public:

	CollectablesWidget( float x, float y, BasicWidget* parent );

	virtual ~CollectablesWidget();

	void AddBandaid();
	void AddHeart();
	void AddBomb();

	bool UseBandaid();
	bool UseHeart();
	bool UseBomb();

	virtual bool Poll();

	virtual void Draw();
protected:
	int hearts;
	int bandaids;
	int bombs;
	static std::map< std::string, Texture > textures;
	std::vector< std::map< int, float > > countdowns;
	float vpsace;
	float icon_scale;
	float spacing;

	void BlowUpLast( Texture &t, int v );

private:
	void RepeatTexture( Texture &t, int n, float y, int v );
	};


#endif



