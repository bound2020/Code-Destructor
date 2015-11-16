#include "naive_bay.h"
void NaiveBay::update(const T& t)
{
	all.first += t.click; all.second += t.imp;
	u[t.uid].first += t.click; u[t.uid].second += t.imp;
	a[t.adid].first += t.click; a[t.adid].second += t.imp;
	q[t.qid].first += t.click; q[t.qid].second += t.imp;
	if (++count.first % 1000000 == 0)
		fprintf(stderr, "%dM: u=%lu q=%lu\n", count.first/1000000, u.size(), q.size());
}

double NaiveBay::predict(const T& t)
{
	const static int th = 10;
	double c = all.first * 1.0 / all.second;
	double ac = c, uc = c, qc = c;
	II2::iterator it;
	it = a.find(t.adid);
	if (it != a.end() && it->second.second > th) ac = it->second.first * 1.0 / it->second.second;
	it = u.find(t.uid);
	if (it != u.end() && it->second.second > th) uc = it->second.first * 1.0 / it->second.second;
	it = q.find(t.qid);
	if (it != q.end() && it->second.second > th) qc = it->second.first * 1.0 / it->second.second;
	return (ac * uc * qc) / (c * c);
}
