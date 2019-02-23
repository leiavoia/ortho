#ifndef WIDGETMOVER_H
#define WIDGETMOVER_H

#include <libLevGC/src/UI/widgets/BasicWidget.h>
#include <libLevGC/src/objects/PollingObject.h>

using namespace LGC;

class WidgetMover: public LGC::PollingObject {
public:

	WidgetMover( float x, float y, BasicWidget* widget, float speed = 1.0 );

	virtual ~WidgetMover() {};

	virtual bool Poll();

	virtual void Draw();

protected:
	float target_x;
	float target_y;
	float starting_x;
	float starting_y;
	float speed;
	BasicWidget* widget;
	};


#endif



