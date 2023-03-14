
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

using namespace std;

struct tpoint {
	vector<long double> X;
	long double  Y;
	tpoint(vector<long double> _X, long double _Y) : X(_X), Y(_Y) {}
	bool operator<(const tpoint& a) {
		if (X == a.X)
			return Y < a.Y;
		return X < a.X;
	}
	friend ostream& operator<<(ostream& fout, const tpoint& tmp) {
		fout << "X =";
		for (auto u : tmp.X)
			cout << u << " ";
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
	vector <long double> characteristics;
	long double parametr;
	pair <vector<long double>, vector<long double>> borders;
	long double errorX;
	tpoint result;
	tpoint other;

	int number_of_tests;
	int tmp_dim, dimension;

	long double (*func)(vector<long double>);

public:
	tpoint function(tpoint a) {
		if (tmp_dim + 1 == dimension) {
			a.Y = func(a.X);
			return a;
		}
		else {
			GSA tmpP(func,parametr,errorX,borders.first,borders.second,dimension,tmp_dim+1,a);
			return tmpP.calculate_minimum();
		}
	}

	tpoint calculate_minimum(int count_of_tests = 200) {
		result.Y = UINT64_MAX;
		number_of_tests = 0;
		base_calculates();
		for (; number_of_tests < count_of_tests; ) {
			numeration_and_sort();
			long double value = calculate_expected_const();
			calculate_haracteristecs(value);
			list<tpoint>::iterator interval = choice_best_interval();
			list<tpoint>::iterator prev = std::prev(interval);

			if ((*interval).X[tmp_dim] - (*prev).X[tmp_dim] < errorX * (borders.second[tmp_dim] - borders.first[tmp_dim]))
				break;
			long double new_pos = calculate_position_of_point(interval, value);
			test_point(new_pos, interval);
		}
		cout << "number of operations : " << number_of_tests << ", result = " << result << "\n";
		/*numeration_and_sort();
		for (auto x : points)
			cout << "X=" << x.X << ", Y=" << x.Y << "\n";*/
		cout << endl;
		return result;
	}

	GSA(long double (*_func)(vector<long double>),long double _parametr = 2, long double _errorX = 0.001, vector<long double> leftborder = {0, 0}
		, vector<long double> rightborder = { 1, 1 }, int _dimension = 2, int _tmp_dim = 0, tpoint _other = tpoint({ {},0 }))
				:parametr(_parametr), errorX(_errorX), borders({ leftborder,rightborder }), tmp_dim(_tmp_dim), dimension(_dimension),other(_other),result(_other) {
		func = _func;
		result.Y = UINT64_MAX;
		size = 0;
		number_of_tests = 0;
	}

private:
	void test_point(long double _X,list<tpoint>::iterator nxt) {
		other.X.push_back(_X);
		tpoint Y = function(other);
		other.X.pop_back();
		size++;
		number_of_tests++;
		if(result.Y>Y.Y)
			result = Y;
		//cout << "test: X = " << X << ", Y = " << Y << "\n";
		points.insert(nxt, Y);
		memory_points.push_back(Y);
		
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
	
	long double calculate_expected_const() {
		long double value = 0;
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
	
	void calculate_haracteristecs(long double value) {
		characteristics.resize(size);
		characteristics[0] = -1;
		list<tpoint>::iterator it = points.begin();
		for ( int i = 1; i < size; i++, it++) {
			auto tmp = *it;
			auto nxt = *next(it);
			long double diffX = nxt.X[tmp_dim] - tmp.X[tmp_dim];
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
		return ((*interval).X[tmp_dim] + (*prev).X[tmp_dim]) / 2 -
			((*interval).Y - (*prev).Y) / (2 * value);
	}
	
};

int main()
{	
	//GSA(long double _parametr = 2, long double _errorX = 0.001, long double leftborder = 0, long double rightborder = 1)
	//return 3 * sin(-x * 2) - x * cos(2 * x) - 2 * sin(5 * x);
	//return x * sin(x * 2 + 2) - cos(2 * x);
	//return sin(x * 20 + 2) / x - 5 * x * cos(3 * x + 10);
	GSA tmp([](vector<long double> a) {long double x = a[0]; return  3 * sin(-x * 2) - x * cos(2 * x) - 2 * sin(5 * x); }, 2, 0.001, { -1.2 }, { 2.0 },1);
	cout << tmp.calculate_minimum()<<"\n";

}
