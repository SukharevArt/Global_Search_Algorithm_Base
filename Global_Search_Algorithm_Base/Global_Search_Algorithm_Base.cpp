#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <fstream>

#include <assert.h>

#include "../sample_src/Grishagin/GrishaginProblemFamily.hpp"
//#include "../sample_src/Grishagin/grishagin_function.hpp"

using namespace std;

ofstream fout("points.txt");

TGrishaginProblemFamily testGrishgin;
int tmpTest;


struct tpoint { 
	vector<double > X;
	double   Y;
	tpoint(vector<double > _X, double  _Y) : X(_X), Y(_Y) {}
	bool operator<(const tpoint& a) {
		if (X == a.X)
			return Y < a.Y;
		return X < a.X;
	}
	friend ostream& operator<<(ostream& fout, const tpoint& tmp) {
		fout << " X = ";
		for (auto u : tmp.X)
			cout << u << ", ";
		cout<<" Y =" << tmp.Y;
		return fout;
	}
};

class GSA
{
protected:
	vector < tpoint > memory_points;
	list < tpoint > points;
	int size;
	vector <double > characteristics;
	double  parametr;
	pair <vector<double >, vector<double >> borders;
	double  errorX;
	tpoint result;
	tpoint other;

	int number_of_tests;
	int tmp_dim, dimension;

	double  (*func)(vector<double >);

public:

	vector < tpoint > getMP() {
		return memory_points;
	}

	tpoint function(tpoint a) {
		if (tmp_dim + 1 == dimension) {
			a.Y = func(a.X);
			number_of_tests++;
			memory_points.push_back(a);
			return a;
		}
		else {
			GSA tmpP(func,parametr,errorX,borders.first,borders.second,dimension,tmp_dim+1,a);
			tpoint res = tmpP.calculate_minimum();
			number_of_tests += tmpP.number_of_tests;
			for (auto x : tmpP.memory_points)
				memory_points.push_back(x);
			return res;
		}
	}

	tpoint calculate_minimum(int count_of_tests = 200) {
		result.Y = UINT64_MAX;
		number_of_tests = 0;
		base_calculates();
		for (int i = 2; i < count_of_tests; i++) {
			numeration_and_sort();
			double  value = calculate_expected_const();
			calculate_haracteristecs(value);
			list<tpoint>::iterator interval = choice_best_interval();
			list<tpoint>::iterator prev = std::prev(interval);

			if ((*interval).X[tmp_dim] - (*prev).X[tmp_dim] < errorX * (borders.second[tmp_dim] - borders.first[tmp_dim]))
				break;
			double  new_pos = calculate_position_of_point(interval, value);
			test_point(new_pos, interval);
		}
		/*if (tmp_dim == 0){
			cout << "number of operations : " << number_of_tests << ", result : " << result << "\n";
			cout << endl;
		}*/
		/*if (tmp_dim == dimension-1) {
			for (auto z : memory_points) {
				for (auto w : z.X)
					fout << w << " ";
				fout << z.Y << "\n";
			}
		}
		*/
		/*numeration_and_sort();
		for (auto x : points)
			cout << "X=" << x.X << ", Y=" << x.Y << "\n";*/
		return result;
	}

	GSA(double  (*_func)(vector<double >),double  _parametr = 2, double  _errorX = 0.001, vector<double > leftborder = {0, 0}
		, vector<double > rightborder = { 1, 1 }, int _dimension = 2, int _tmp_dim = 0, tpoint _other = tpoint({ {},0 }))
				:parametr(_parametr), errorX(_errorX), borders({ leftborder,rightborder }), tmp_dim(_tmp_dim), dimension(_dimension),other(_other),result(_other) {
		func = _func;
		result.Y = UINT64_MAX;
		size = 0;
		number_of_tests = 0;
	}

private:
	void test_point(double  _X,list<tpoint>::iterator nxt) {
		other.X.push_back(_X);
		tpoint Y = function(other);
		other.X.pop_back();
		size++;
		if(result.Y>Y.Y)
			result = Y;
		//cout << "test: X = " << X << ", Y = " << Y << "\n";
		points.insert(nxt, Y);
		
	}

	void base_calculates() {
		size = 0;
		points.clear();
		test_point(borders.first[tmp_dim], points.end());
		test_point(borders.second[tmp_dim], points.end());
		//sort(points.begin(), points.end());
	}
	
	void numeration_and_sort() {
		//sort(points.begin(), points.end());
	}
	
	double  calculate_expected_const() {
		double  value = 0;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 0; i < size - 1; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			value = max(value,
			abs((nxt.Y - tmp.Y) / (nxt.X[tmp_dim] - tmp.X[tmp_dim])));
		}
		if (value == 0) 
			value = 1;
		else
			value *= parametr;
		return value;
	}
	
	void calculate_haracteristecs(double  value) {
		characteristics.resize(size);
		characteristics[0] = -1;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 1; i < size; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			double  diffX = nxt.X[tmp_dim] - tmp.X[tmp_dim];
			double  diffY = nxt.Y-tmp.Y;
			double  sumY = nxt.Y+tmp.Y;
			double  R = value * diffX+diffY*diffY/(value*diffX)-2*sumY;
			characteristics[i] = R;
		}
	}
	
	list<tpoint>::iterator choice_best_interval() {
		double  Max = characteristics[1];
		auto it = next(points.begin());
		auto interval = it;
		it++;
		for (int i = 2; i < size; i++,it++) {
			if (characteristics[i] > Max) {
				Max = characteristics[i];
				interval = it;
			}
		}
		return interval;
	}

	double  calculate_position_of_point(list<tpoint>::iterator interval,double  value) {
		list<tpoint>::iterator prev = std::prev(interval);
		return ((*interval).X[tmp_dim] + (*prev).X[tmp_dim]) / 2 -
			((*interval).Y - (*prev).Y) / (2 * value);
	}
	
};

bool check(vector<double> a, vector<double> b,double errorD=1e-3) {
	for (int i = 0; i < a.size(); i++) {
		if (abs(a[i] - b[i]) > errorD)
			return false;
	}
	return true;
}
int main()
{	
	//GSA(double  _parametr = 2, double  _errorX = 0.001, double  leftborder = 0, double  rightborder = 1)
	/*double  (*func)(vector<double >) = [](vector<double > a) {
			double  x = a[0]; 
			return  3 * sin(-x * 2) - x * cos(2 * x) - 2 * sin(5 * x); 
	};
	GSA tmp(func, 2, 0.001, { -1.2 }, { 2.0 },1);*/
	//////
	//double  (*func)(vector<double >) = [](vector<double > a) {
	//		double  x = a[0]; 
	//		double  y = a[1]; 
	//		return  sin(x + 0.4*y) + cos(y);
	//		//return sin(x+0.4*y)*(y-3) + cos(y-0.2*x)*x;
	//};
	//GSA tmp(func, 2, 0.001, { -5 , -5 }, { 5 , 5 }, 2);
	//cout << tmp.calculate_minimum()<<"\n";
	//
	double errorD = 1e-3;
	vector<double > lb, rb;

	freopen("result.xls", "w", stdout);
	ios::sync_with_stdio(0); cin.tie(0); cout.tie(0);
	int n = 200;
	int m = 100;
	int N = (n + 1) * (n + 1)/10;
	vector<vector<int >> mem;
	double step = 0.3;
	double left = 1.1;

	for (double r = left; r < 3.2; r += step) {
		vector<int> count(N);
		vector<double> lb, rb;
		//int r = 2;
		for (int i = 0; i < m; i++) {
			tmpTest = i;
			testGrishgin[tmpTest]->GetBounds(lb, rb);
			//cout << testGrishgin[tmpTest]->GetOptimumValue() << endl;
			double  (*func)(vector<double >) = [](vector<double > a)->double {
				return testGrishgin[tmpTest]->ComputeFunction(a);
			};
			GSA tmp(func, r, 1e-3, lb, rb, 2);
			tpoint res = tmp.calculate_minimum();
			//cout << res << endl;
			vector < tpoint > MP = tmp.getMP();
			vector<double> a = testGrishgin[tmpTest]->GetOptimumPoint();
			vector<double> b = res.X;
			if (!check(a, b, errorD)) {
				//cout << -1 << "\n";
			}
			else 
				for (int i = 0; i < MP.size(); i++) {
					if (check(a, MP[i].X, errorD)) {
						//cout << i + 1 << endl;
						count[min(i + 1,N-1)]++;
						break;
					}
				}

		}
		for (int i = 1; i < N; i++)
			count[i] += count[i - 1];
		mem.push_back(count);
	}
	for (int i = 0; i < mem.size(); i++) {
		cout << "R = " << left + i * step << "\t\t\t";
	}
	cout << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < mem.size(); j++) {
			string e = to_string((double)mem[j][min(i, N)] / m);
			for (auto& x : e) {
				if (x == '.')
					x = ',';
			}
			cout << i << "\t" << e << "\t\t";
		}
		cout << endl;
	}
	cout << endl;


	


}
