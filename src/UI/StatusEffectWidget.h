#ifndef StatusEffectWidget_H
#define StatusEffectWidget_H

#include <libLevGC/src/UI/widgets/BasicWidget.h>
#include <libLevGC/src/core/Texture.h>
#include <map>
#include <vector>

using namespace LGC;

class StatusEffectWidget: public LGC::BasicWidget {
public:

	StatusEffectWidget( float x, float y, BasicWidget* parent );

	virtual ~StatusEffectWidget();

	void ActivateEffect( std::string effect, float time );

	virtual bool Poll();

	virtual void Draw();
protected:
	std::map< std::string, float > countdowns;
	static std::map< std::string, Texture > textures;
	};


#endif



