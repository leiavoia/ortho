#include "WidgetMover.h"


WidgetMover::WidgetMover( float x, float y, BasicWidget* widget, float speed ) {
	starting_x = widget->Xpos();
	starting_y = widget->Ypos();
	target_x = x;
	target_y = y;
	this->speed = speed;
	this->widget = widget;
	}

bool WidgetMover::Poll() {
	float ratio = TimeFromBirth() / speed;
	if ( ratio > 1.0f ) { ratio = 1.0f; }
	widget->SetXPos( starting_x + ((target_x - starting_x) * ratio) );
	widget->SetYPos( starting_y + ((target_y - starting_y) * ratio) );
	return ratio < 1.0f;
	}

void WidgetMover::Draw() { ;; }

