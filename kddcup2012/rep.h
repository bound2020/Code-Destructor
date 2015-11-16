#if !defined(REP_H)
#define REP_H
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <dlib/static_map.h>

struct T
{
	int adid, qid, uid;
	int click, imp;
};

typedef std::pair<int, int> I2;
typedef std::map<int, I2> II2;
typedef std::vector<int> Ivector;

void sample_parse();
void test_parse();
void sample_split();
void repo_save();
void repo_dump();
void test_mmap();

class MMap
{
public:
	MMap(const char* name);
	~MMap();
	void get(int id, Ivector&);
private:
	FILE * idx;
	FILE * dat;
	long max_id;
};

struct User_Repo : public dlib::static_map<int, int>
{
};

struct Token_Repo
{
};

struct PredictorBase
{
	virtual void update(const T&) = 0;
	virtual double predict(const T&) = 0;
	virtual ~PredictorBase(){}
	void run();
	void run_test();
};

#endif
