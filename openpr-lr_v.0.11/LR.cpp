/********************************************************************
* Logistic Regression Classifier V0.11
* Implemented by Rui Xia(rxiacn@gmail.com) , Wang Tao(linclonwang@163.com)
* Last updated on 2012-10-25.
*********************************************************************/
#include "LR.h"

LR::LR()
{

}

LR::~LR()
{
}

void LR::save_model(string model_file)
{
	cout << "Saving model..." << endl;
    ofstream fout(model_file.c_str());
    //存储每个属性对应每个类别的参数值
	for (int k = 0; k < feat_set_size; k++) {
		for (int j = 0; j < class_set_size; j++) {
			fout << omega[k][j] << " ";
		}
		fout << endl;
	}
    fout.close();
}


void LR::load_model(string model_file)
{
	cout << "Loading model..." << endl;
	omega.clear();
    ifstream fin(model_file.c_str());
    if(!fin) {
        cerr << "Error opening file: " << model_file << endl;
        exit(0);
    }
    string line_str;
    while (getline(fin, line_str)) {
        vector<string> line_vec = string_split(line_str, " ");
        vector<float>  line_omega;
        for (vector<string>::iterator it = line_vec.begin(); it != line_vec.end(); it++) {
			float weight = (float)atof(it->c_str());
			line_omega.push_back(weight);
		}
		omega.push_back(line_omega);
	}
	fin.close();
	feat_set_size = (int)omega.size();
	class_set_size = (int)omega[0].size();
}


void LR::read_samp_file(string samp_file, vector<sparse_feat> &samp_feat_vec, vector<int> &samp_class_vec) {
    ifstream fin(samp_file.c_str());
    if(!fin) {
        cerr << "Error opening file: " << samp_file << endl;
        exit(0);
    }
    string line_str;
    while (getline(fin, line_str))
	{
        size_t class_pos = line_str.find_first_of("\t");   //读取类别的下标
        int class_id = atoi(line_str.substr(0, class_pos).c_str());
        samp_class_vec.push_back(class_id);
        string terms_str = line_str.substr(class_pos+1);
        sparse_feat samp_feat;
        samp_feat.id_vec.push_back(0); // bias
        samp_feat.value_vec.push_back(1); // bias
        if (terms_str != "")
		{
		    //格式，类别 特征名:特征值 特证名:特征值 ...
			vector<string> fv_vec = string_split(terms_str, " ");
			for (vector<string>::iterator it = fv_vec.begin(); it != fv_vec.end(); it++)
			{
				size_t feat_pos = it->find_first_of(":");
				int feat_id = atoi(it->substr(0, feat_pos).c_str());
				float feat_value = (float)atof(it->substr(feat_pos+1).c_str());
				samp_feat.id_vec.push_back(feat_id);
				samp_feat.value_vec.push_back(feat_value);
			}
        }
        samp_feat_vec.push_back(samp_feat);
    }
    fin.close();
}


void LR::load_training_file(string training_file)
{
	cout << "Loading training data..." << endl;
	read_samp_file(training_file, samp_feat_vec, samp_class_vec);
	feat_set_size = 0;
	class_set_size = 0;
	for (size_t i = 0; i < samp_class_vec.size(); i++) {
		if (samp_class_vec[i] > class_set_size) {
			class_set_size = samp_class_vec[i];
		}
		if (samp_feat_vec[i].id_vec.back() > feat_set_size) {
			feat_set_size = samp_feat_vec[i].id_vec.back();
		}
	}
	class_set_size += 1;
	feat_set_size += 1;
}

void LR::init_omega()
{
	 float init_value = 0.0;
	//float init_value = (float)1/class_set_size;
	for (int i = 0; i < feat_set_size; i++) {
		vector<float> temp_vec(class_set_size, init_value);
		omega.push_back(temp_vec);
	}
}

// Stochastic Gradient Descent (SGD) Optimization
int LR::train_sgd( int max_loop, double loss_thrd, float learn_rate, float lambda, int avg)
{
	int id = 0;
	double loss = 0.0;
	double loss_pre = 0.0;
	vector< vector<float> > omega_pre=omega;
	float acc=0.0;

	vector< vector<float> > omega_sum(omega);

    //最多迭代(最大循环 * 训练样本类别个数)次
	while (id <= max_loop*(int)samp_class_vec.size())
	{
		if (id%samp_class_vec.size() == 0)	// 完成一次迭代，预处理工作。
		{
			int loop = id/(int)samp_class_vec.size();               //check loss
		    loss = 0.0;
		    acc = 0.0;

			calc_loss(&loss, &acc); //计算损失
			cout.setf(ios::left);
			cout << "Iter: " << setw(8) << loop << "Loss: " << setw(18) << loss << "Acc: " << setw(8) << acc << endl;
			//如果损失值小于阈值，停止迭代
			if ((loss_pre - loss) < loss_thrd && loss_pre >= loss && id != 0)
			{
				cout << "Reaching the minimal loss decrease!" << endl;
				break;
			}
			loss_pre = loss;    //保留上次损失值
			if (id)            //表示第一次不做正则项计算
			{
				for (int i=0;i!=omega_pre.size();i++)
					for (int j=0;j!=omega_pre[i].size();j++)
						omega[i][j]+=omega_pre[i][j]*lambda;   //lambda为权重衰减项
			}
			omega_pre=omega;
		}
		// update omega
		//随机选择样本
		int r = (int)(rand()%samp_class_vec.size());
		sparse_feat samp_feat = samp_feat_vec[r];    //随机样本的特征
		int samp_class = samp_class_vec[r];    //随机样本的类别

		update_online(samp_class, samp_feat, learn_rate, lambda);

		if (avg == 1 && id%samp_class_vec.size() == 0)
		{
			for (int i = 0; i < feat_set_size; i++)
			{
				for (int j = 0; j < class_set_size; j++)
				{
					omega_sum[i][j] += omega[i][j];
				}
			}
		}
		id++;
	} //迭代结束
	if (avg == 1)
	{
		for (int i = 0; i < feat_set_size; i++)
		{
			for (int j = 0; j < class_set_size; j++)
			{
				omega[i][j] = (float)omega_sum[i][j] / id;
			}
		}
	}
	return 1;
}

void LR::update_online(int samp_class, sparse_feat &samp_feat, float learn_rate, float lambda)
{
	vector<float> score_vec = calc_score(samp_feat); //(W'*X)
	vector<float> softmax_vec = softmax(score_vec);
	for (int i=0;i<class_set_size;i++)                          //对于每一个类
	{

		float error_term=((int)(i==samp_class)-softmax_vec[i]);
		for (int j=0;j<samp_feat.id_vec.size();j++)             //对于每个类中的每个样本，更新参数
		{
			int feat_id=samp_feat.id_vec[j];
			float feat_value=samp_feat.value_vec[j];
			float delt=error_term*feat_value;    //梯度 = x^i * (指示函数 - y)
			//float regul = lambda * omega[feat_id][i];
			omega[feat_id][i]+=learn_rate*delt;    //更新参数，学习率 * 梯度
		}
	}
}

void LR::calc_loss(double *loss, float *acc)
{
	double neg_log_likeli = 0.0;
	int err_num = 0;
	//对每个样本的类别计算
	for (size_t i = 0; i < samp_class_vec.size(); i++)
	{
		int samp_class = samp_class_vec[i];
		sparse_feat samp_feat = samp_feat_vec[i];     //获得类别i下所有的样本
		vector<float> score_vec = calc_score(samp_feat);    //计算该类别所有样本的分数
		int pred_class = score_to_class(score_vec);    //将分数映射到类别
		if (pred_class != samp_class)
			err_num += 1;
		vector<float> softmax_vec = softmax(score_vec);
	    for (int j = 0; j < class_set_size; j++)
	    {
			if (j == samp_class)
			{
				double pj = softmax_vec[j];
				double temp = pj < LOG_LIM ? LOG_LIM : pj;
				neg_log_likeli += log(temp);
			}
		}
	}
	*loss = -neg_log_likeli ; // CE(Cross Entropy) loss equals negative log-likelihood in LR
	*acc = 1 - (float)err_num / samp_class_vec.size();
}

//计算样本的分数，即 sigma(y) = w * x'，每个类别每个样本都要计算
vector<float> LR::calc_score(sparse_feat &samp_feat)
{
	vector<float> score(class_set_size, 0);
	for (int j = 0; j < class_set_size; j++)
	{
		for (size_t k = 0; k < samp_feat.id_vec.size(); k++)
		{
			int feat_id = samp_feat.id_vec[k];
			float feat_value = samp_feat.value_vec[k];
			score[j] += omega[feat_id][j] * feat_value;
		}
	}
    return score;
}

vector<float> LR::score_to_prb(vector<float> &score)
{
	vector<float> softmax_vec(class_set_size);
	float max_score=*(max_element(score.begin(),score.end()));
	float soft_max_sum=0;
	//对每个类别都要计算
    for (int j=0;j<class_set_size;j++)
    {
    	softmax_vec[j]=exp(score[j]-max_score);
    	soft_max_sum+=softmax_vec[j];                             //同时除最大的score;
    }
	for (int k=0;k<class_set_size;k++)
		softmax_vec[k]/=soft_max_sum;
	return softmax_vec;
}

// 遍历每个类别下的分数，将样本分到分数最大的类
int LR::score_to_class(vector<float> &score)
{
	int pred_class = 0;
	float max_score = score[0];
	for (int j = 1; j < class_set_size; j++) {
		if (score[j] > max_score) {
			max_score = score[j];
			pred_class = j;
		}
	}
    return pred_class;
}

float LR::classify_testing_file(string testing_file, string output_file, int output_format)
{
	cout << "Classifying testing file..." << endl;
	vector<sparse_feat> test_feat_vec;
	vector<int> test_class_vec;
	vector<int> pred_class_vec;
	read_samp_file(testing_file, test_feat_vec, test_class_vec);
	ofstream fout(output_file.c_str());
	for (size_t i = 0; i < test_class_vec.size(); i++)
	{
		int samp_class = test_class_vec[i];
		sparse_feat samp_feat = test_feat_vec[i];
		vector<float> pred_score = calc_score(samp_feat);
		int pred_class = score_to_class(pred_score);
		pred_class_vec.push_back(pred_class);
		fout << pred_class << "\t"<<samp_class<<"\t";
		if (output_format == 1)
		{
			for (int j = 0; j < class_set_size; j++)
			{
				fout << j << ":" << pred_score[j] << ' ';
			}
		}
		else if (output_format == 2)
		{
			vector<float> pred_prb = score_to_prb(pred_score);
			for (int j = 0; j < class_set_size; j++)
			{
				fout << j << ":" << pred_prb[j] << ' ';
			}
		}

		fout << endl;
	}
	fout.close();
	float acc = calc_acc(test_class_vec, pred_class_vec);
	return acc;
}

float LR::calc_acc(vector<int> &test_class_vec, vector<int> &pred_class_vec)
{
	size_t len = test_class_vec.size();
	if (len != pred_class_vec.size()) {
		cerr << "Error: two vectors should have the same lenght." << endl;
		exit(0);
	}
	int err_num = 0;
	for (size_t id = 0; id != len; id++) {
		if (test_class_vec[id] != pred_class_vec[id]) {
			err_num++;
		}
	}
	return 1 - ((float)err_num) / len;
}

float LR::sigmoid(float x)
{
	double sgm = 1 / (1+exp(-(double)x));
	return sgm;
}

vector<float> LR::softmax(vector<float> &score_vec)
{
	vector<float> softmax_vec(class_set_size);   //长度为类别的个数
	double score_max = *(max_element(score_vec.begin(), score_vec.end()));
	double e_sum = 0;
	//计算每个类别下的softmax值，归一化处理
    for (int j=0; j<class_set_size; j++) {
    	softmax_vec[j] = exp((double)score_vec[j]- score_max);
    	e_sum += softmax_vec[j];
    }
	for (int k=0; k<class_set_size; k++)
		softmax_vec[k] /= e_sum;
	return softmax_vec;
}


vector<string> LR::string_split(string terms_str, string spliting_tag)
{
	vector<string> feat_vec;
    size_t term_beg_pos = 0;
    size_t term_end_pos = 0;
    while ((term_end_pos = terms_str.find_first_of(spliting_tag, term_beg_pos)) != string::npos)
	{
        if (term_end_pos > term_beg_pos)
		{
            string term_str = terms_str.substr(term_beg_pos, term_end_pos - term_beg_pos);
            feat_vec.push_back(term_str);
        }
        term_beg_pos = term_end_pos + 1;
    }
	if (term_beg_pos < terms_str.size())
	{
		string end_str = terms_str.substr(term_beg_pos);
		feat_vec.push_back(end_str);
	}
    return feat_vec;
}
