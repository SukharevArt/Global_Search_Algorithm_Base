#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <assert.h>


#include "../sample_src/Hill/HillProblem.hpp"
#include "../sample_src/Hill/HillProblemFamily.hpp"
#include "../sample_src/Shekel/ShekelProblem.hpp"
#include "../sample_src/Shekel/ShekelProblemFamily.hpp"


using namespace std;

struct tpoint {
	double X, Y;
	tpoint(double _X = 0, double _Y = double(INT32_MAX)) : X(_X), Y(_Y) {}
	bool operator<(const tpoint& a) {
		if (X == a.X)
			return Y < a.Y;
		return X < a.X;
	}
	tpoint& operator=(const int _Y) {
		X = 0;
		Y = double(_Y);
		return *this;
	}
	friend ostream& operator<<(ostream& fout, const tpoint& tmp) {
		fout << "X =" << tmp.X << " Y =" << tmp.Y;
		return fout;
	}
};

class IGOAWLT
{
protected:
	vector < tpoint > memory_points;
	list < tpoint > points;
	int size;
	vector <double> characteristics;
	double parametr;
	pair <double,double> borders;
	double errorX;
	tpoint result;

	int number_of_tests;

public:
	virtual double function(double x) {
		return 3 * sin(-x * 2) - x * cos(2 * x) - 2 * sin(5 * x);
		//return x * sin(x * 2 + 2) - cos(2 * x);
		//return sin(x * 20 + 2) / x - 5 * x * cos(3 * x + 10);
	}

	int calculate_minimum(int count_of_tests, double Xmin, double error) {
		result = UINT64_MAX;
		number_of_tests = 0;
		base_calculates();
		for (; number_of_tests < count_of_tests; ) {
			numeration_and_sort();
			double valueM = calculate_expected_const();
			double valueX = calculate_max_len();
			calculate_haracteristecs(valueM, valueX);
			list<tpoint>::iterator interval = choice_best_interval();
			list<tpoint>::iterator prev = std::prev(interval);

			if ((*interval).X - (*prev).X < errorX * (borders.second - borders.first))
				break;
			double new_pos = calculate_position_of_point(interval, valueM, valueX);
			test_point(new_pos, interval);
		}
		if (abs(result.X - Xmin) >= error * (borders.second - borders.first)) {
			return -1;
		}
		int num = -1;
		for (int i = 0; i < size; i++) {
			if (abs(memory_points[i].X - Xmin) < error * (borders.second - borders.first)) {
				num = i + 1;
				break;
			}
		}
		return num;
	}

	IGOAWLT(double _parametr = 2,double _errorX = 0.001,double leftborder = 0,double rightborder = 1)
		:parametr(_parametr), errorX(_errorX), borders({ leftborder,rightborder }) {
		result = UINT64_MAX;
		size = 0;
		number_of_tests = 0;
	}

private:
	//+
	void test_point(double X, list<tpoint>::iterator nxt) {
		double Y = function(X);
		size++;
		number_of_tests++;
		if (result.Y > Y)
			result = tpoint(X, Y);
		//cout << "test: X = " << X << ", Y = " << Y << "\n";
		points.insert(nxt, tpoint(X, Y));
		memory_points.push_back(tpoint(X, Y));

	}
	//+
	void base_calculates() {
		size = 0;
		points.clear();
		test_point(borders.first, points.end());
		test_point(borders.second, points.end());
		//sort(points.begin(), points.end());
	}

	void numeration_and_sort() {
		//sort(points.begin(), points.end());
	}
	//+
	double calculate_expected_const() {
		double valueM = 0;
		list<tpoint>::iterator it = points.begin();
		for (int i = 0; i < size - 1; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			valueM = max(valueM, abs((nxt.Y - tmp.Y) / (nxt.X - tmp.X)));
		}
		return valueM;
	}
	//+
	double calculate_max_len() {
		double valueX = 0;
		list<tpoint>::iterator it = points.begin();
		for (int i = 0; i < size - 1; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			valueX = max(valueX, abs(nxt.X - tmp.X));
		}
		return valueX;
	}
	//+
	void calculate_haracteristecs(double valueM,double valueX) {
		characteristics.resize(size);
		characteristics[0] = -1;
		list<tpoint>::iterator it = points.begin();
		for (int i = 1; i < size; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			double diffX = nxt.X - tmp.X;
			double diffY = nxt.Y - tmp.Y;
			double sumY = nxt.Y + tmp.Y;
			double value_m = calculate_m(it, valueM, valueX);

			double R = value_m * parametr * diffX + diffY * diffY / (parametr * value_m * diffX) - 2 * sumY;
			characteristics[i] = R;
		}
	}
	//+
	double calculate_m(list<tpoint>::iterator tmp,double valueM,double valueX) {
		auto it2 = tmp;
		auto it3 = next(tmp);
		auto it4 = next(it3);

		double value_m = valueM * ((*it3).X - (*it2).X) / valueX;
		if (it2 != points.begin()) {
			auto it1 = prev(tmp);
			value_m = max(value_m, abs((*it2).Y - (*it1).Y) / ((*it2).X - (*it1).X));
		}
		value_m = max(value_m, abs((*it3).Y - (*it2).Y) / ((*it3).X - (*it2).X));
		if (it4 != points.end()) {
			value_m = max(value_m, abs((*it4).Y - (*it3).Y) / ((*it4).X - (*it3).X));
		}
		return value_m;
	}
	//+
	list<tpoint>::iterator choice_best_interval() {
		double Max = characteristics[1];
		auto it = next(points.begin());
		auto interval = it;
		it++;
		for (int i = 2; i < size; i++, it++) {
			if (characteristics[i] > Max) {
				Max = characteristics[i];
				interval = it;
			}
		}
		return interval;
	}
	//+
	double calculate_position_of_point(list<tpoint>::iterator interval,double valueM,double valueX) {
		list<tpoint>::iterator prev = std::prev(interval);
		double optvalue_m = calculate_m(prev, valueM, valueX);
		if (optvalue_m == 0) {
			return ((*interval).X + (*prev).X) / 2;
		}
		return ((*interval).X + (*prev).X) / 2 -
			((*interval).Y - (*prev).Y) / (2 * parametr * optvalue_m);
	}

};

class IGOAWLT_func_Hill : public IGOAWLT {
public:
	static THillProblemFamily testHill;
protected:
	int num_test;
public:
	IGOAWLT_func_Hill(double _parametr = 2, double _errorX = 0.001, double leftborder = 0, double rightborder = 1,int _num_test = 0)
		:IGOAWLT(_parametr,_errorX,leftborder,rightborder) {
		num_test = _num_test;

	}
protected:
	double function(double x) {
		double y = testHill[num_test]->ComputeFunction({x});
		return y;
	}
};
THillProblemFamily IGOAWLT_func_Hill::testHill;

class IGOAWLT_func_Shekel : public IGOAWLT {
public:
	static TShekelProblemFamily testShekel;
protected:
	int num_test;
public:
	IGOAWLT_func_Shekel(double _parametr = 2, double _errorX = 0.001, double leftborder = 0, double rightborder = 1,int _num_test = 0)
		:IGOAWLT(_parametr,_errorX,leftborder,rightborder) {
		num_test = _num_test;

	}
protected:
	double function(double x) {
		return testShekel[num_test]->ComputeFunction({x});
	}
};
TShekelProblemFamily IGOAWLT_func_Shekel::testShekel;

int main()
{	

	freopen("result.xls", "w", stdout);
	ios::sync_with_stdio(0); cin.tie(0); cout.tie(0);
	int n = 250;
	int m = 1000;
	vector<vector<int >> mem;
	double step=0.3;
	double left = 1.2;

	for (double r = left; r < 3.5; r += step) {
		vector<int> count(n + 1);
		vector<double> lb, rb;
		//int r = 2;
		for (int i = 0; i < m; i++) {
			IGOAWLT_func_Hill::testHill[i]->GetBounds(lb, rb);
			IGOAWLT_func_Hill tmp(r, 1e-3, lb[0], rb[0], i);
			int result = tmp.calculate_minimum(n, IGOAWLT_func_Hill::testHill[i]->GetOptimumPoint()[0], 1e-3);
			if (result >= 0) {
				count[result]++;
			}
		}
		for (int i = 1; i <= n; i++)
			count[i] += count[i - 1];
		mem.push_back(count);
	}
	for (int i = 0; i < mem.size(); i++) {
		cout << "R = " << left+i*step <<"\t\t\t";
	}
	cout << endl;
	for(int i=0;i<n;i++){
		for (int j = 0; j < mem.size(); j++) {
			string e = to_string((double)mem[j][min(i,n)] / m);
			for (auto& x : e) {
				if (x == '.')
					x = ',';
			}
			cout << i << "\t" << e << "\t\t";
		}
		cout << endl;
	}
	cout << endl;
	mem.clear();

	for (double r = left; r < 3.5; r += step) {
		vector<int> count(n + 1);
		vector<double> lb, rb;
		//int r = 2;
		for (int i = 0; i < m; i++) {
			IGOAWLT_func_Shekel::testShekel[i]->GetBounds(lb, rb);
			IGOAWLT_func_Shekel tmp(r, 1e-3, lb[0], rb[0], i);
			int result = tmp.calculate_minimum(n, IGOAWLT_func_Shekel::testShekel[i]->GetOptimumPoint()[0], 1e-3);
			if (result >= 0) {
				count[result]++;
			}
		}
		for (int i = 1; i <= n; i++)
			count[i] += count[i - 1];
		mem.push_back(count);
	}
	for (int i = 0; i < mem.size(); i++) {
		cout << "R = " << left + i * step << "\t\t\t";
	}
	cout << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < mem.size(); j++) {
			string e = to_string((double)mem[j][min(i, n)] / m);
			for (auto& x : e) {
				if (x == '.')
					x = ',';
			}
			cout << i << "\t" << e << "\t\t";
		}
		cout << endl;
	}


}
