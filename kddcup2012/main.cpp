#include <stdio.h>
#include "naive_bay.h"
using namespace dlib;
int main()
{
	//test_parse();
	//sample_parse();
	//sample_split();
	//NaiveBay().run_test();
	repo_save();
	repo_dump();
	test_mmap();
	return 0;
}
