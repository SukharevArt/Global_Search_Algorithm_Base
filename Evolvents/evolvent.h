/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//             LOBACHEVSKY STATE UNIVERSITY OF NIZHNY NOVGOROD             //
//                                                                         //
//                       Copyright (c) 2015 by UNN.                        //
//                          All Rights Reserved.                           //
//                                                                         //
//  File:      evolvent.h                                                  //
//                                                                         //
//  Purpose:   Header file for evolvent classes                            //
//                                                                         //
//  Author(s): Barkalov K., Sysoyev A.                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __EVOLVENT_H__
#define __EVOLVENT_H__

#include "common.h"
#include <vector>

// ------------------------------------------------------------------------------------------------
class TEvolvent
{
protected:
  int      m;             // accuracy of decomposition of hypercube
  int      N;             // dimension
  double   A[MaxDim];     // left and
  double   B[MaxDim];     // right bounds of search area

  double*  y;             // y point from hypercube [-1/2, 1/2]^N

  const double extNull; // = 0.0; //double(0.0);
  const double extOne;  // = 1.0; //double(1.0);
  const double extHalf; // = 0.5; //double(0.5);
  double nexpExtended;

  void CalculateNumbr(double *s, int *u, int *v, int *l);
  void CalculateNode(double is, int n, int *u, int *v, int *l);
  void transform_P_to_D(); // transformation from hypercube P to hyperinterval D
  void transform_D_to_P(); // transformation from hyperinterval D to hypercube P
  double* GetYOnX(const double& _x);
  double GetXOnY();
public:
  TEvolvent(int _N = 2, int _m = 10);
  TEvolvent(const TEvolvent& evolvent);
  virtual ~TEvolvent();
  void SetBounds(const double* _A, const double* _B);
  //x-->y
  virtual void GetImage(const double& x, double* _y, int EvolventNum = 0);
  //y-->x
  void GetInverseImage(double* _y, double& x);
  //y-->x
  virtual void GetPreimages(double* _y, double* x);
  TEvolvent& operator=(const TEvolvent& evolvent);

  /// Вычисляет функцию существования точки в развертки EvolventNum для y, <0 - существует
  virtual double ZeroConstraintCalc(const double* _y, int EvolventNum = 0);

};

class TShiftedEvolvent : public TEvolvent
{
protected:
  int    L; // number of evolvents - 1, 0 by default, means one evolvent is used
  double PowOf2[MaxDim * MaxL + 1]; // degrees of 2
  void transform_P_to_Pl(int EvolventNum); //
  void transform_Pl_to_P(int EvolventNum);
  double ZeroConstraint();
public:
  TShiftedEvolvent(int _N = 2, int _m = 10, int _L = 0);
  virtual ~TShiftedEvolvent();
  virtual void GetImage(const double& x, double* _y, int EvolventNum = 0);
  virtual void GetPreimages(double* _y, double* x);
  /// Вычисляет функцию существования точки в развертки EvolventNum для y, <0 - существует
  virtual double ZeroConstraintCalc(const double* _y, int EvolventNum = 0);
};

class TRotatedEvolvent : public TEvolvent
{
protected:
  int L;          // number of evolvents, 1 by default, means one evolvent is used
  int PlaneCount; // current number of planes
  int Planes[MaxDim * (MaxDim - 1) / 2][2]; // axes array
  double PowOfHalf[MaxM + 2]; // degrees of 1/2

  void GetAllPlanes();
public:
  TRotatedEvolvent(int _N = 2, int _m = 10, int _L = 0);
  virtual ~TRotatedEvolvent();
  virtual void GetImage(const double& x, double* _y, int EvolventNum = 0);
  virtual void GetPreimages(double* _y, double* x);
};

class TSmoothEvolvent : public TEvolvent
{
protected:
  double h; //smoothness
  int smoothPointCount;
  bool continuously;
  std::vector<double> tmp_y, tmp_y_;
  void GetYOnXSmooth(double x, std::vector<double>&y, std::vector<double>&y_);
public:
  TSmoothEvolvent(int _N = 2, int _m = 10, double _h=0.25);
  virtual ~TSmoothEvolvent();
  virtual void GetImage(const double& x, double* _y, int EvolventNum = 0);
  void GetInverseImage(double* _y, double& x);
};

class TLinearEvolvent : public TEvolvent
{
protected:
  double mneExtended;
public:
  TLinearEvolvent(int _N = 2, int _m = 10);
  virtual ~TLinearEvolvent();
  virtual void GetImage(const double& x, double* _y, int EvolventNum = 0);
  void GetInverseImage(double* _y, double& x);
};

class TNoninjectiveEvolvent : public TEvolvent
{
protected:
  int max_preimages;
  double mneExtended;
public:
  TNoninjectiveEvolvent(int _N = 2, int _m = 10, int _max_preimages=64);
  virtual ~TNoninjectiveEvolvent();
  virtual void GetImage(const double& x, double* _y, int EvolventNum = 0);
  virtual int GetNoninjectivePreimages(double* _y, double* x);
  int GetMaxPreimagesNumber() const { return max_preimages; };
};

#endif
// - end of file ----------------------------------------------------------------------------------
