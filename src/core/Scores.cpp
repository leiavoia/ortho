#include "Scores.h"
#include <iostream>
#include <fstream>
#include <libLevGC/src/core/cast.h>
#include <libLevGC/src/utilities/LogManager.h>



ScoresMgr* ScoresMgr::Instance() {
	static ScoresMgr x = ScoresMgr();
	return &x;
	}

ScoresMgr::ScoresMgr( /*std::string filename*/ ) {
// 	this->filename = filename;
// 	LoadScores(filename);
	}

void ScoresMgr::LoadScores( const std::string& filename ) {
	this->filename = filename;
	std::ifstream file(filename.c_str());
	if (file.bad()) { LGC::LogMgr::Write(LGC::LOGMSG_ERROR, "High Scores file '%s' stream was corrupt or bad.", filename.c_str()); }
	if (!file.is_open()) { LGC::LogMgr::Write(LGC::LOGMSG_ERROR, "High Scores file '%s' could not be opened.", filename.c_str()); }
	while (file.good()) {
		std::string player, score, hits, seconds, awesomeness, mode, difficulty;
		file >> player >> score >> hits >> seconds >> awesomeness >> difficulty >> mode;
		if ( !player.empty() && !score.empty() && !hits.empty() && !seconds.empty() && !awesomeness.empty() && !mode.empty() && !difficulty.empty() ) {
			scores.push_back(
				Score(
					player,
					LGC::cast::lexical_cast< int, std::string >(score),
					LGC::cast::lexical_cast< int, std::string >(hits),
					LGC::cast::lexical_cast< int, std::string >(seconds),
					LGC::cast::lexical_cast< float, std::string >(awesomeness),
					LGC::cast::lexical_cast< int, std::string >(difficulty),
					LGC::cast::lexical_cast< int, std::string >(mode)
					)
				);
			}
		}
	}


void ScoresMgr::DumpScores( const std::string& filename ) {
	std::ofstream file(filename.c_str());
	if (file.bad()) { LGC::LogMgr::Write(LGC::LOGMSG_ERROR, "High Scores file '%s' stream was corrupt or bad.", filename.c_str()); }
	if (!file.is_open()) { LGC::LogMgr::Write(LGC::LOGMSG_ERROR, "High Scores file '%s' could not be opened.", filename.c_str()); }
	for (std::vector< Score >::iterator i = scores.begin(); i != scores.end(); i++) {
		if (!file.good()) {
			LGC::LogMgr::Write(LGC::LOGMSG_ERROR, "High Scores file writing stream broken. Aborted writing.", filename.c_str());
			break;
			}
		file << i->player << '\t'
			<< i->score << '\t'
			<< i->hits << '\t'
			<< i->seconds << '\t'
			<< i->awesomeness << '\t'
			<< i->difficulty << '\t'
			<< i->mode;
		if ( i != scores.end()-1 ) { file << std::endl; }
		}
	LGC::LogMgr::Write(LGC::LOGMSG_DEBUG, "Scores dumped." );
	}


std::vector<ScoresMgr::Score> ScoresMgr::GetScores() {
	return scores;
	}


void ScoresMgr::AddScore( Score score ) {
	if ( !score.score ) { return; }
	unsigned int max_scores = 15;
	if ( scores.empty() ) {
		scores.push_back( score );
		DumpScores( filename );
		}
	else {
		bool gotcha = false;
		for (std::vector< Score >::iterator i = scores.begin(); i != scores.end(); i++) {
			if ( score.awesomeness > i->awesomeness ) {
				scores.insert( i, score );
				if ( scores.size() > max_scores ) { scores.pop_back(); }
				DumpScores( filename );
				gotcha = true;
				break;
				}
			}
		if ( !gotcha && scores.size() < max_scores ) {
			scores.push_back( score );
			DumpScores( filename );
			}
		}
	}





