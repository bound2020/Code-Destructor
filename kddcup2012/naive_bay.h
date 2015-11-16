#ifndef NAIVE_BAY
#define NAIVE_BAY
#include "rep.h"
struct NaiveBay : public PredictorBase
{
	II2 u, a, q;
	I2 all;
	I2 count;
	void update(const T&);
	double predict(const T&);
	virtual ~NaiveBay(){}
};
#endif
