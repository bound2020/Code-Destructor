/********************************************************************
* Logistic Regression Classifier V0.11
* Implemented by Rui Xia(rxiacn@gmail.com) , Wang Tao(linclonwang@163.com)
* Last updated on 2012-10-25. 
*********************************************************************/
#include <cstdlib>
#include <iostream>
#include <cstring>

#include "LR.h"

using namespace std;


void print_help() 
{
	cout << "\nOpenPR-LR training module, " << VERSION << ", " << VERSION_DATE << "\n\n"
		<< "usage: lr_train [options] training_file model_file [pre_model_file]\n\n"
		<< "options: -h        -> help\n"
		<< "         -n int    -> maximal iteration loops (default 200)\n"
		<< "         -m double -> minimal loss value decrease (default 1e-03)\n"
		<< "         -r double -> lambda of gaussian prior regularization (default 0)\n"		
		<< "         -l float  -> learning rate (default 1.0)\n"
		<< "         -a        -> 0: final weight (default)\n"
		<< "                   -> 1: average weights of all iteration loops\n"
		<< "         -u [0,1]  -> 0: initial training model (default)\n"
		<< "                   -> 1: updating model (pre_model_file is needed)\n" 
		<< endl;
}

void read_parameters(int argc, char *argv[], char *training_file, char *model_file, 
					 int *max_loop, double *loss_thrd, float *learn_rate, float *lambda,
						int *avg, int *update, char *pre_model_file)
{
	// set default options
	*max_loop = 200;
	*loss_thrd = 1e-3;
	*learn_rate = 1.0;
	*lambda = 0.0;
	*avg = 0;
	*update = 0;
	int i;
	for (i = 1; (i<argc) && (argv[i])[0]=='-'; i++) 
	{
		switch ((argv[i])[1]) {
			case 'h':
				print_help();
				exit(0);
			case 'n':
				*max_loop = atoi(argv[++i]);
				break;
			case 'm':
				*loss_thrd = atof(argv[++i]);
				break;
			case 'l':
				*learn_rate = (float)atof(argv[++i]);
				break;
			case 'r':
				*lambda = (float)atof(argv[++i]);
				break;
			case 'a':
				*avg = atoi(argv[++i]);
				break;
			case 'u':
				*update = atoi(argv[++i]);
				break;
			default:
				cout << "Unrecognized option: " << argv[i] << "!" << endl;
				print_help();
				exit(0);
		}
	}
	
	if ((i+1)>=argc) 
	{
		cout << "Not enough parameters!" << endl;
		print_help();
		exit(0);
	}

	strcpy (training_file, argv[i]);
	strcpy (model_file, argv[i+1]);
	if (*update) 
	{
		if ((i+2)>=argc) 
		{
			cout << "Previous model file is needed in update mode!" << endl;
			print_help();
			exit(0);
		}
		strcpy (pre_model_file, argv[i+2]);
	}
}

int lr_train(int argc, char *argv[])
{
	char training_file[200];
	char model_file[200];
	int max_loop;
	double loss_thrd;
	float learn_rate;
	float lambda;
	int avg;
	int update;
	char pre_model_file[200];
	read_parameters(argc, argv, training_file, model_file, &max_loop, &loss_thrd, &learn_rate, &lambda, &avg, &update, pre_model_file);
    
    LR LR;
    LR.load_training_file(training_file);
    if (update) 
	{
		LR.load_model(pre_model_file);
	}
	else 
	{
		LR.init_omega();	
	}
    LR.train_sgd(max_loop, loss_thrd, learn_rate, lambda, avg);
	LR.save_model(model_file);
	return 0;
}

int main(int argc, char *argv[])
{
    return lr_train(argc, argv);
}
