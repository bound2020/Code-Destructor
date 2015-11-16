#include <stdio.h>
#include <stdlib.h>
#include <map>
struct T
{
	int adid, qid, uid;
	int click, imp;
};
int parse()
{
	FILE *i = fopen("sample.txt", "r");
	FILE *o = fopen("sample.dat", "w");
	char buf[4096];
	size_t click, imp, url, adid, advid, depth, pos, qid, kv, title, desc, uid;
	while(fgets(buf, sizeof buf, i))
	{
		sscanf(buf, "%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu", &click, &imp, &url, &adid, &advid, &depth, &pos, &qid, &kv, &title, &desc, &uid);
		T t;
		t.adid=adid;
		t.qid=qid;
		t.uid=uid;
		t.click=click;
		t.imp=imp;
		fwrite(&t, sizeof(T), 1, o);
	}
	fclose(i);
	fclose(o);
	return 0;
}

int dump()
{
	FILE *i = fopen("sample.dat", "r");
	T t;
	while(1 == fread(&t, sizeof(T), 1, i))
	{
		printf("%d %d %d %d %d\n", t.adid, t.qid, t.uid, t.click, t.imp);
	}
	fclose(i);
	return 0;
}

int split()
{
	FILE *i = fopen("sample.dat", "r");
	FILE *o1 = fopen("train.dat", "w");
	FILE *o2 = fopen("test.dat", "w");
	T t;
	srand(123);
	while(1 == fread(&t, sizeof(T), 1, i))
	{
		fwrite(&t, sizeof(T), 1, rand() % 10 ? o1 : o2);
	}
	fclose(i);
	fclose(o2);
	fclose(o1);
	return 0;
}

int demo()
{
	FILE *train = fopen("train.dat", "r");
	T t;
	//train
	std::map<int, std::pair<int, int> > users;
	std::pair<int, int> all;
	while(1 == fread(&t, sizeof(T), 1, train))
	{
		std::pair<int, int>& p = users[t.uid];
		p.first += t.click;
		p.second += t.imp;
		all.first += t.click;
		all.second += t.imp;
	}
	fclose(train);
	//predict
	FILE *test = fopen("test.dat", "r");
	while(1 == fread(&t, sizeof(T), 1, test))
	{
		double ctr = 0.5;
		std::map<int, std::pair<int, int> >::const_iterator it = users.find(t.uid);
		if (it == users.end() && it->second.second > 5)
			ctr = it->second.first * 1.0 / it->second.second;
		else
			ctr = all.first * 1.0 / all.second;
		printf("%d %d %lf\n", t.click, t.imp, ctr);
	}
	fclose(test);
}

int main()
{
	return demo();
}
