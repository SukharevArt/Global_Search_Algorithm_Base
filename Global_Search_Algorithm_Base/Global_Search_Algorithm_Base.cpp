
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>


#include "../sample_src/Hill/HillProblem.hpp"
#include "../sample_src/Hill/HillProblemFamily.hpp"
#include "../sample_src/Shekel/ShekelProblem.hpp"
#include "../sample_src/Shekel/ShekelProblemFamily.hpp"


using namespace std;

struct tpoint {
	double X, Y;
	tpoint(double _X, double _Y) : X(_X), Y(_Y) {}
	bool operator<(const tpoint& a) {
		if (X == a.X)
			return Y < a.Y;
		return X < a.X;
	}
	friend ostream& operator<<(ostream& fout, const tpoint& tmp) {
		fout << "X =" << tmp.X << " Y =" << tmp.Y;
		return fout;
	}
};

class GSA
{
protected:
	vector < tpoint > memory_points;
	list < tpoint > points;
	int size;
	vector <double> characteristics;
	double parametr;
	pair <double, double> borders;
	double errorX;
	double result;

	int number_of_tests;

public:
	double function(double x) {
		return 3 * sin(-x * 2) - x * cos(2 * x) - 2 * sin(5 * x);
		//return x * sin(x * 2 + 2) - cos(2 * x);
		//return sin(x * 20 + 2) / x - 5 * x * cos(3 * x + 10);
	}

	double calculate_minimum(int count_of_tests, double Xmin, double error) {
		result = UINT64_MAX;
		number_of_tests = 0;
		base_calculates();
		for (; number_of_tests < count_of_tests; ) {
			numeration_and_sort();
			double value = calculate_expected_const();
			calculate_haracteristecs(value);
			list<tpoint>::iterator interval = choice_best_interval();
			list<tpoint>::iterator prev = std::prev(interval);

			if ((*interval).X - (*prev).X < errorX * (borders.second - borders.first))
				break;
			double new_pos = calculate_position_of_point(interval, value);
			test_point(new_pos, interval);
		}
		//cout << "number of operations : " << number_of_tests << ", result = " << result << "\n";
		/*numeration_and_sort();
		for (auto x : points)
			cout << "X=" << x.X << ", Y=" << x.Y << "\n";*/
		//cout << endl;
		if ((memory_points.back().X - Xmin) >= error) {
			return -1;
		}
		int num = -1;
		for (int i = 0; i < size;i++) {
			if ((memory_points[i].X - Xmin) >= error) {
				num = i + 1;
				break;
			}
		}
		return num;
	}

	GSA(double _parametr = 2, double _errorX = 0.001, double leftborder = 0, double rightborder = 1)
		:parametr(_parametr), errorX(_errorX), borders({ leftborder,rightborder }) {
		result = UINT64_MAX;
		size = 0;
		number_of_tests = 0;
	}

protected:
	void test_point(double X,list<tpoint>::iterator nxt) {
		double Y = function(X);
		size++;
		number_of_tests++;
		result = min(result, Y);
		cout << "test: X = " << X << ", Y = " << Y << "\n";
		points.insert(nxt, tpoint(X, Y));
		memory_points.push_back(tpoint(X, Y));
		
	}

	void base_calculates() {
		size = 0;
		points.clear();
		test_point(borders.first,points.end());
		test_point(borders.second, points.end());
		//sort(points.begin(), points.end());
	}
	
	void numeration_and_sort() {
		//sort(points.begin(), points.end());
	}
	
	double calculate_expected_const() {
		double value = 0;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 0; i < size - 1; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			value = max(value,
			abs((nxt.Y - tmp.Y) / (nxt.X - tmp.X)));
		}
		if (value == 0) 
			value = 1;
		else
			value *= parametr;
		return value;
	}
	
	void calculate_haracteristecs(double value) {
		characteristics.resize(size);
		characteristics[0] = -1;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 1; i < size; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			double diffX = nxt.X-tmp.X;
			double diffY = nxt.Y-tmp.Y;
			double sumY = nxt.Y+tmp.Y;
			double R = value * diffX+diffY*diffY/(value*diffX)-2*sumY;
			characteristics[i] = R;
		}
	}
	
	list<tpoint>::iterator choice_best_interval() {
		double Max = characteristics[1];
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

	double calculate_position_of_point(list<tpoint>::iterator interval,double value) {
		list<tpoint>::iterator prev = std::prev(interval);
		return ((*interval).X + (*prev).X) / 2 -
			((*interval).Y - (*prev).Y) / (2 * value);
	}
	
};

class GSA_func_Hill : public GSA {
public:
	static THillProblemFamily testHill;
protected:
	int num_test;
public:
	GSA_func_Hill(double _parametr = 2, double _errorX = 0.001, double leftborder = 0, double rightborder = 1,int _num_test = 0)
		:GSA(_parametr,_errorX,leftborder,rightborder) {
		num_test = _num_test;

	}
protected:
	double function(double x) {
		return testHill[num_test]->ComputeFunction({x});
	}
};
THillProblemFamily GSA_func_Hill::testHill;

class GSA_func_Shekel : public GSA {
public:
	static TShekelProblemFamily testShekel;
protected:
	int num_test;
public:
	GSA_func_Shekel(double _parametr = 2, double _errorX = 0.001, double leftborder = 0, double rightborder = 1,int _num_test = 0)
		:GSA(_parametr,_errorX,leftborder,rightborder) {
		num_test = _num_test;

	}
protected:
	double function(double x) {
		return testShekel[num_test]->ComputeFunction({x});
	}
};
TShekelProblemFamily GSA_func_Shekel::testShekel;


int main()
{	
	//GSA(double _parametr = 2, double _errorX = 0.001, double leftborder = 0, double rightborder = 1)
	//GSA tmp(2, 0.001, -1.2, 2.0);
	//THillProblemFamily testHill; 
	int n = 10;
	for (int i = 0; i < n; i++)
		cout << "HillProblem [" << i << "] (" << 0.5 + double(i) / 2000.0 << ") = " <<
		GSA_func_Hill::testHill[i]->GetOptimumValue() << std::endl;
	for (int i = 0; i < n; i++)
		cout << "HillProblem [" << i << "] (" << 0.5 + double(i) / 2000.0 << ") = " <<
		GSA_func_Hill::testHill[i]->ComputeFunction(GSA_func_Hill::testHill[i]->GetOptimumPoint()) << std::endl;
	//double Xmin = 0;
	//double error = 0.001;
	//cout << tmp.calculate_minimum(300,Xmin,error)<<"\n";

}
