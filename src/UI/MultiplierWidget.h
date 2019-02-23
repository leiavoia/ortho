#ifndef MULTIPLIERWIDGET_H
#define MULTIPLIERWIDGET_H

#include <libLevGC/src/UI/widgets/Label.h>
#include <libLevGC/src/core/Fonts.h>

using namespace LGC;

class MultiplierWidget: public LGC::Label {
public:

	MultiplierWidget( float x, float y, BasicWidget* parent);

	virtual ~MultiplierWidget();

	void AddToValue( float x ) { mult += x; }
	void MultiplyValue( float x ) { mult *= x; }
	float Value() { return mult; }

	void StopAdjustments() { do_adjustments = false; }
	virtual bool Poll();

	virtual void Draw();

protected:
	float mult;
	bool do_adjustments;
	};


#endif



