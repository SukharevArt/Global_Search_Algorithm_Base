
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


class GSA
{
protected:
	vector < pair <long double, long double> > points;
	int size;
	vector <long double> characteristics;
	long double parametr;
	pair <long double, long double> borders;
	long double errorX;
	long double result;

	int number_of_tests;

public:
	long double function(long double x) {
		return x * x - cos(2 * x);
	}

	long double calculate_minimum(int count_of_tests = 200) {
		result = UINT64_MAX;
		number_of_tests = 0;
		base_calculates();
		for ( ;number_of_tests < count_of_tests; ){
			numeration_and_sort();
			long double value = calculate_expected_const();
			calculate_haracteristecs(value);
			int interval = choice_best_interval();
			if (points[interval].first - points[interval-1].first < errorX)
				break;
			long double new_pos = calculate_position_of_point(interval, value);
			test_point(new_pos);
		}
		cout << "number of operations : " << number_of_tests  << ", result = " << result << "\n";
		/*numeration_and_sort();
		for (auto x : points)
			cout << "X=" << x.first << ", Y=" << x.second << "\n";*/
		cout << endl;
		return result;
	}

	GSA(long double _parametr = 2, long double _errorX = 0.001, long double leftborder = 0, long double rightborder = 1)
		:parametr(_parametr), errorX(_errorX), borders({ leftborder,rightborder }) {
		result = UINT64_MAX;
		size = 0;
		number_of_tests = 0;
	}

private:
	void test_point(long double X) {
		long double Y = function(X);
		points.push_back({ X, Y});
		result = min(result, Y);
		size++;
		number_of_tests++;
		cout << "test: X = " << X << ", Y = " << Y << "\n";
	}

	void base_calculates() {
		size = 0;
		points.clear();
		test_point(borders.first);
		test_point(borders.second);
	}
	
	void numeration_and_sort() {
		sort(points.begin(), points.end());
	}
	
	long double calculate_expected_const() {
		long double value = 0;
		for (int i = 1; i < size; i++) {
			value = max(value,
			abs((points[i].second - points[i-1].second) / (points[i].first - points[i-1].first)));
		}
		if (value < 1e-8) 
			value = 1;
		else
			value *= parametr;
		return value;
	}
	
	void calculate_haracteristecs(long double value) {
		characteristics.resize(size);
		characteristics[0] = -1;
		for (int i = 1; i < size; i++) {
			long double diffX = points[i].first-points[i-1].first;
			long double diffY = points[i].second-points[i-1].second;
			long double sumY = points[i].second+points[i-1].second;
			long double R = value * diffX+diffY*diffY/(value*diffX)-2*sumY;
			characteristics[i] = R;
		}
	}
	
	int choice_best_interval() {
		long double Max = characteristics[1];
		int interval = 1;
		for (int i = 2; i < size; i++) {
			if (characteristics[i] > Max) {
				Max = characteristics[i];
				interval = i;
			}
		}
		return interval;
	}

	long double calculate_position_of_point(int interval,long double value) {
		return (points[interval].first + points[interval-1].first) / 2 -
			(points[interval].second - points[interval-1].second) / (2 * value);
	}
	
};

int main()
{	
	//GSA(long double _parametr = 2, long double _errorX = 0.001, long double leftborder = 0, long double rightborder = 1)
	GSA tmp(2,0.01,-1.2,2);
	cout << tmp.calculate_minimum()<<"\n";

}
