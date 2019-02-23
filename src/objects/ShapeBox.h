#ifndef SHAPEBOX_H
#define SHAPEBOX_H

#include <vector>
#include <map>
#include <libLevGC/src/core/Texture.h>
#include <libLevGC/src/core/Sound.h>
#include <libLevGC/src/objects/PollingObject.h>
#include <libLevGC/src/objects/PhysicalObject.h>
#include <libLevGC/src/algo/collision/Vector.h>

//////////////////////////////
    using namespace LGC;
//////////////////////////////

//predec
class Cursor;


enum ShapeType {
	SHAPE_NULL = 0,
	SHAPE_RED = 1,
	SHAPE_BLUE = 2,
	SHAPE_GREEN = 3,
	SHAPE_YELLOW = 4,
	SHAPE_PURPLE = 5,
	SHAPE_HALF,
	SHAPE_FAST,
	SHAPE_RANDOMIZE,
	SHAPE_REVERSE,
	SHAPE_SLOW,
	SHAPE_1UP,
	SHAPE_X2,
	SHAPE_X3,
	SHAPE_BOMB,
	SHAPE_ANTIDOTE,
	SHAPE_CLEARSCREEN,
	SHAPE_MAX
	};

class ShapeBox: public LGC::PollingObject, public LGC::PhysicalObject {
public:

	ShapeBox( float x, float y, int files, int ranks, unsigned int dir );

	~ShapeBox();

	void AddRandomShape( float warning_time );

	void Clear();
	void Cancel();
	void Randomize();
	void ClearFirstRank();
	Vector ShootAt(
		unsigned int file,
		Cursor* cursor,
		int &pts,
		int &kills,
		float &mult,
		ShapeType &last_chain_color,
		int &color_chains
		);

	virtual bool Poll();

	virtual void Draw();

	static void ReserveResources();

	static void DumpResources();

	float GetXCoord( unsigned int f, unsigned int r );
	float GetYCoord( unsigned int f, unsigned int r );

private:

	void AddRandomShape_impl( unsigned int f );

	// int direction;
	static std::map< std::string, Texture > textures;
	static std::map< std::string, Sound > sfx;
	static bool img_loaded;
// 	static Sound laser_sound;
	std::vector< std::vector<ShapeType> > shapes;

	static float transition_speed;
	std::vector<float> transition_warning_times;
	std::vector<float> transition_progress;
	std::vector<bool>  transition_files;
	std::vector<float> transition_countdowns;

	static float spacing;

	unsigned int dir;

	int num_shapes; // depends on difficulty level

	bool cancelled;

	static float funnymode_time;
	float funnymode_countdown;

	static void FileWarningSound( int dir, bool x, bool high );

	};

#endif





