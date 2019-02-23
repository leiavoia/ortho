#ifndef ScoresMgr_H
#define ScoresMgr_H

#include <string>
#include <vector>
#include <libLevGC/src/utilities/LogManager.h>


// handy shortcut
#define Scores ScoresMgr::Instance()


class ScoresMgr {
public:
	static ScoresMgr* Instance();

	void LoadScores( const std::string& filename );

	void DumpScores( const std::string& filename );

	class Score {
		public:
		std::string player;
		int score;
		int hits;
		int seconds;
		float awesomeness;
		int difficulty;
		int mode; // 0 = pure, 1 = special
		Score ( std::string player, int score, int hits, int seconds, float awesomeness, int difficulty, int mode ) {
			this->player = player;
			this->score = score;
			this->hits = hits;
			this->seconds = seconds;
			this->awesomeness = awesomeness;
			this->difficulty = difficulty;
			this->mode = mode;
			}
		};

	std::vector<Score> GetScores();

	void AddScore( Score score );

private:
	ScoresMgr();
	~ScoresMgr(){}
	std::vector<Score> scores;
	std::string filename;
	};



#endif







