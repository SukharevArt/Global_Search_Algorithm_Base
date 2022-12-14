
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

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

class IGOAWLT
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

	long double calculate_minimum(int count_of_tests = 200) {
		result = UINT64_MAX;
		number_of_tests = 0;
		base_calculates();
		for ( ;number_of_tests < count_of_tests; ){
			numeration_and_sort();
			long double valueM = calculate_expected_const();
			long double valueX = calculate_max_len();
			calculate_haracteristecs(valueM,valueX);
			list<tpoint>::iterator interval = choice_best_interval();
			list<tpoint>::iterator prev = std::prev(interval);

			if ((*interval).X - (*prev).X < errorX*(borders.second-borders.first))
				break;
			long double new_pos = calculate_position_of_point(interval,valueM,valueX);
			test_point(new_pos,interval);
		}
		cout << "number of operations : " << number_of_tests+1  << ", result = " << result << "\n";
		/*numeration_and_sort();
		for (auto x : points)
			cout << "X=" << x.X << ", Y=" << x.Y << "\n";*/
		cout << endl;
		return result;
	}

	IGOAWLT(long double _parametr = 2, long double _errorX = 0.001, long double leftborder = 0, long double rightborder = 1)
		:parametr(_parametr), errorX(_errorX), borders({ leftborder,rightborder }) {
		result = UINT64_MAX;
		size = 0;
		number_of_tests = 0;
	}

private:
	//+
	void test_point(long double X,list<tpoint>::iterator nxt) {
		long double Y = function(X);
		size++;
		number_of_tests++;
		result = min(result, Y);
		cout << "test: X = " << X << ", Y = " << Y << "\n";
		points.insert(nxt, tpoint(X, Y));
		memory_points.push_back(tpoint(X, Y));
		
	}
	//+
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
	//+
	long double calculate_expected_const() {
		long double valueM = 0;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 0; i < size - 1; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			valueM = max(valueM,abs((nxt.Y - tmp.Y) / (nxt.X - tmp.X)));
		}
		return valueM;
	}
	//+
	long double calculate_max_len() {
		long double valueX = 0;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 0; i < size - 1; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			valueX = max(valueX,abs(nxt.X - tmp.X));
		}
		return valueX;
	}
	//+
	void calculate_haracteristecs(long double valueM,long double valueX) {
		characteristics.resize(size);
		characteristics[0] = -1;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 1; i < size; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			long double diffX = nxt.X-tmp.X;
			long double diffY = nxt.Y-tmp.Y;
			long double sumY = nxt.Y+tmp.Y;
			long double value_m = calculate_m(it, valueM, valueX);

			long double R = value_m*parametr * diffX+diffY*diffY/(parametr*value_m*diffX)-2*sumY;
			characteristics[i] = R;
		}
	}
	//+
	long double calculate_m(list<tpoint>::iterator tmp, long double valueM, long double valueX) {
		auto it2 = tmp;
		auto it3 = next(tmp);
		auto it4 = next(it3);

		long double value_m = valueM * ((*it3).X-(*it2).X) / valueX;
		if (it2 != points.begin()) {
			auto it1 = prev(tmp);
			value_m = max(value_m,abs((*it2).Y-(*it1).Y)/((*it2).X - (*it1).X));
		}
		value_m = max(value_m,abs((*it3).Y-(*it2).Y)/((*it3).X - (*it2).X));
		if (it4 != points.end()) {
			value_m = max(value_m,abs((*it4).Y-(*it3).Y)/((*it4).X - (*it3).X));
		}
		return value_m;
	}
	//+
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
	//+
	long double calculate_position_of_point(list<tpoint>::iterator interval, long double valueM, long double valueX) {
		list<tpoint>::iterator prev = std::prev(interval);
		long double optvalue_m = calculate_m(prev, valueM, valueX);
		if (optvalue_m == 0) {
			return ((*interval).X + (*prev).X) / 2;
		}
		return ((*interval).X + (*prev).X) / 2 -
			((*interval).Y - (*prev).Y) / (2 *parametr* optvalue_m);
	}
	
};

int main()
{	
	//GSA(long double _parametr = 2, long double _errorX = 0.001, long double leftborder = 0, long double rightborder = 1)
	IGOAWLT tmp(2, 0.001, -1.2, 2.0);
	cout << tmp.calculate_minimum()<<"\n";

}
