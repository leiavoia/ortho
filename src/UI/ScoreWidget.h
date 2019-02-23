#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include <libLevGC/src/UI/widgets/Label.h>
#include <libLevGC/src/core/Fonts.h>
#include <map>

using namespace LGC;

class ScoreWidget: public LGC::Label {
public:

	ScoreWidget( float x, float y, BasicWidget* parent);

	virtual ~ScoreWidget();

	void AddToScore( float x );
	float Score() { return score; }
	void ResetScore() { score = 0; }
	void DrawFloaties( bool x ) { enable_floaties = x; }

	virtual bool Poll();

	virtual void Draw();

protected:
	float score;
	Font floatie_font;
	struct TimeScore { float time; int score; };
	std::vector< TimeScore > floaties;
	const static float floatie_time;
	bool enable_floaties;
	};


#endif



