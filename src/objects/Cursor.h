#ifndef CURSOR_H
#define CURSOR_H


#include <libLevGC/src/core/Texture.h>
#include <libLevGC/src/core/Sound.h>
#include <libLevGC/src/objects/PollingObject.h>
#include <libLevGC/src/objects/PhysicalObject.h>

#include "ShapeBox.h"

//////////////////////////////
    using namespace LGC;
//////////////////////////////


class Cursor: public LGC::PollingObject, public LGC::PhysicalObject {
public:

	enum CursorCommand { LEFT, RIGHT, UP, DOWN, FIRE, HALT_X, HALT_Y, EMPTY };

	Cursor();

	~Cursor();

	void Do( CursorCommand cmd );

	virtual bool Poll();

	virtual void Draw();

	static void ReserveResources();

	static void DumpResources();

	void Type( ShapeType t );
	inline ShapeType Type() { return this->type; }
	inline int Direction() { return this->direction; }
	void ShootAt( Vector v );

	int XFile() { return xfile; }
	int YFile() { return yfile; }

private:
	ShapeType type; // determines color
	int direction;
	Texture* sprite;
	static Texture img_blue;
	static Texture img_green;
	static Texture img_yellow;
	static Texture img_red;
	static Texture img_purple;
	static Texture color_swap;
	static bool img_loaded;
	static Sound laser_sound;

// 	CursorLaser* laser;

	int xfile;
	int yfile;

	float x_trans;
	float y_trans;

	float trans_speed;

	static float ghost_speed;
	Vector start;

	bool continue_moving_x;
	bool continue_moving_y;

	CursorCommand queud_cmd;

	bool move_instantly;

	struct Ghost {
		float x;
		float y;
		int dir;
		float timeout;
		};

	std::list<Ghost> ghosts;

	void AddGhosts( float x, float y, int dir );
	};

#endif





