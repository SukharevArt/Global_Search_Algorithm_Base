#include <iostream>
#include "evolvent.h"

using namespace std;

//������������� ��������� ���������
int TestShiftedEvolvents()
{
	int N = 2;  //����������� ������
	int m = 12; //��������� ��������� (2^-m)
	int L = 10;  //����� ��������� 

	//���� �����, ���������� ��� double
	if ((m + 1) * N <= 50)
	{
		Extended::SetTypeID(etDouble);
	}
	//�������������� ��������� ��������
	Extended::SetPrecision(1 / (pow(2., (m + 1) * N)));

	//������������� ������� ����������� �������
	double* A = new double[N];
	double* B = new double[N];
	for (int i = 0; i < N; i++)
	{
		A[i] = -1;
		B[i] = 1;
	}

	Extended* x1 = new Extended[L];
	Extended* x = new Extended[L];


	//���������� ����� ������� �� ������� ����� ���� double
	double h = 1e-3;
	// ����� �� ����������
	double* y1 = new double[N];
	y1[0] = -0.15;
	y1[1] = 0.15;
	double* y2 = new double[N];
	y2[0] = 0.25;
	y2[1] = 0.25;
	double* y = new double[N];
	y[0] = 0.15;
	y[1] = 0.15;

	// ������� ��������� �� �������, ��������� �������� = (����� ��������� - 1)
	TShiftedEvolvent Evolvent(N, m, L - 1);
	Evolvent.SetBounds(A, B);
	Evolvent.GetPreimages(y1, x1);

	for (int i = 0; i < L; i++)
		std::cout << x1[i].toDouble() << " ";

	std::cout << std::endl;

	for (int i = 0; i < L; i++)
	{
		Evolvent.GetImage(x1[i], y, i);
		std::cout << y[0] << " " << y[1] << std::endl;
	}

	delete[] A;
	delete[] B;
	delete[] y;
	delete[] y1;
	delete[] y2;
	delete[] x;
	delete[] x1;
	return 0;

}

//������������� ����� ���������
int TestEvolvent()
{
	int N = 2;  //����������� ������
	int m = 10; //��������� ��������� (2^-m)

	//���� �����, ���������� ��� double
	if (m * N <= 50)
	{
		Extended::SetTypeID(etDouble);
	}
	//�������������� ��������� ��������
	Extended::SetPrecision(1 / (pow(2., m * N)));

	//������������� ������� ����������� �������
	double* A = new double[N];
	double* B = new double[N];
	for (int i = 0; i < N; i++)
	{
		A[i] = -1;
		B[i] = 1;
	}

	// ���������
	TEvolvent Evolvent(N, m);
	Evolvent.SetBounds(A, B);

	// x \in [0,1]
	Extended x;

	// y \in D
	double* y = new double[N];
	y[0] = 1.0;
	y[1] = -1.0;

	//����������� y->x
	Evolvent.GetInverseImage(y, x);
	cout << "x = " << x.toDouble() << " ";

	cout << endl;

	//����������� x->y
	Evolvent.GetImage(x, y);
	for (int i = 0; i < N; i++)
	{
		cout << "y["<<i<<"] = "<< y[i] << " ";
	}
	cout << endl;


	delete[] A;
	delete[] B;
	delete[] y;
	return 0;

}



int main()
{
	TestEvolvent();
	return 0;
}