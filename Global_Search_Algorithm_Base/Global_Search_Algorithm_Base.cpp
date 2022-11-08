
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
	long double X, Y;
	tpoint(long double _X, long double _Y) : X(_X), Y(_Y) {}
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
	vector <long double> characteristics;
	long double parametr;
	pair <long double, long double> borders;
	long double errorX;
	long double result;

	int number_of_tests;

public:
	long double function(long double x) {
		return 3 * sin(-x * 2) - x * cos(2 * x) - 2 * sin(5 * x);
		//return x * sin(x * 2 + 2) - cos(2 * x);
		//return sin(x * 20 + 2) / x - 5 * x * cos(3 * x + 10);
	}

	long double calculate_minimum(int count_of_tests, long double Xmin, long double error) {
		result = UINT64_MAX;
		number_of_tests = 0;
		base_calculates();
		for (; number_of_tests < count_of_tests; ) {
			numeration_and_sort();
			long double value = calculate_expected_const();
			calculate_haracteristecs(value);
			list<tpoint>::iterator interval = choice_best_interval();
			list<tpoint>::iterator prev = std::prev(interval);

			if ((*interval).X - (*prev).X < errorX * (borders.second - borders.first))
				break;
			long double new_pos = calculate_position_of_point(interval, value);
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

	GSA(long double _parametr = 2, long double _errorX = 0.001, long double leftborder = 0, long double rightborder = 1)
		:parametr(_parametr), errorX(_errorX), borders({ leftborder,rightborder }) {
		result = UINT64_MAX;
		size = 0;
		number_of_tests = 0;
	}

private:
	void test_point(long double X,list<tpoint>::iterator nxt) {
		long double Y = function(X);
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
	
	long double calculate_expected_const() {
		long double value = 0;
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
	
	void calculate_haracteristecs(long double value) {
		characteristics.resize(size);
		characteristics[0] = -1;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 1; i < size; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			long double diffX = nxt.X-tmp.X;
			long double diffY = nxt.Y-tmp.Y;
			long double sumY = nxt.Y+tmp.Y;
			long double R = value * diffX+diffY*diffY/(value*diffX)-2*sumY;
			characteristics[i] = R;
		}
	}
	
	list<tpoint>::iterator choice_best_interval() {
		long double Max = characteristics[1];
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

	long double calculate_position_of_point(list<tpoint>::iterator interval,long double value) {
		list<tpoint>::iterator prev = std::prev(interval);
		return ((*interval).X + (*prev).X) / 2 -
			((*interval).Y - (*prev).Y) / (2 * value);
	}
	
};

int main()
{	
	//GSA(long double _parametr = 2, long double _errorX = 0.001, long double leftborder = 0, long double rightborder = 1)
	GSA tmp(2, 0.001, -1.2, 2.0);
	THillProblemFamily testHill; 
	int n = 10;
	for (int i = 0; i < n; i++)
		cout << "HillProblem [" << i << "] (" << 0.5 + double(i) / 2000.0 << ") = " <<
		testHill[i]->GetOptimumValue() << std::endl;
	for (int i = 0; i < n; i++)
		cout << "HillProblem [" << i << "] (" << 0.5 + double(i) / 2000.0 << ") = " <<
		testHill[i]->GetMaxValue() << std::endl;
	for (int i = 0; i < n; i++)
		cout << "HillProblem [" << i << "] (" << 0.5 + double(i) / 2000.0 << ") = " <<
		testHill[i]->GetOptimumPoint()[0] << std::endl;
	for (int i = 0; i < n; i++)
		cout << "HillProblem [" << i << "] (" << 0.5 + double(i) / 2000.0 << ") = " <<
		testHill[i]->ComputeFunction(testHill[i]->GetMaxPoint()) << std::endl;
	long double Xmin = 0;
	long double error = 0.001;
	//cout << tmp.calculate_minimum(300,Xmin,error)<<"\n";

}
