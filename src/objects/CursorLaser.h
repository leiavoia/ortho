#ifndef CURSORLASER_H
#define CURSORLASER_H

#include <libLevGC/src/objects/PollingObject.h>
#include <libLevGC/src/algo/collision/Vector.h>
#include <libLevGC/src/core/Texture.h>
#include <libLevGC/src/core/Sound.h>

//////////////////////////////
    using namespace LGC;
//////////////////////////////

class CursorLaser: public LGC::PollingObject {
public:

	CursorLaser( float x1, float y1, float x2, float y2 );

	~CursorLaser();

	virtual bool Poll();

	virtual void Draw();

	static void ReserveResources();
	static void DumpResources();

private:
	Vector start;
	Vector end;
	static Texture light;
	static Sound sfx;
	};

#endif





