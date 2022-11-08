#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>

#include "../IOptProblem.hpp"

using std::vector;

#define NUM_HILL_PROBLEMS 1000
#define NUM_HILL_COEFF 14

// double aHill[NUM_HILL_PROBLEMS][NUM_HILL_COEFF];
// double bHill[NUM_HILL_PROBLEMS][NUM_HILL_COEFF];
// double maxHill[NUM_HILL_PROBLEMS][2];
// double lConstantHill[NUM_HILL_PROBLEMS];
// double minHill[NUM_HILL_PROBLEMS][2];

#include "HillProblem.cpp"

// Hill problem
class THillProblem : public IOptProblem
{
  virtual double Compute(int index, const vector<double>& y) const
  {
    double res = 0.0;

    for (int i = 0; i < NUM_HILL_COEFF; i++)
    {
      res = res + aHill[mProblemIndex][i] * sin(2 * i * M_PI * y[0]) + bHill[mProblemIndex][i] *
        cos(2 * i * M_PI * y[0]);
    }
    return res;
  }
  virtual vector<double> ComputeDerivatives(int index, const vector<double>& y) const
  {
    double res = 0.0;

    for (int i = 0; i < NUM_HILL_COEFF; i++)
    {
      res = res + 2 * i * M_PI * aHill[mProblemIndex][i] * cos(2 * i * M_PI * y[0]) + 2 * i *
        M_PI * bHill[mProblemIndex][i] * sin(2 * i * M_PI * y[0]);
    }

    return{ res };
  }

public:
  THillProblem(int functionIndex = 0) : IOptProblem(1, { 0.0 }, { 1.0 }, { minHill[functionIndex][1] },
    minHill[functionIndex][0], functionIndex)
  {
    mProblemIndex = functionIndex;
    mFunctions[mFunctionIndex].mIsDerivativesKnown = true;
    SetLipschitzConstant(lConstantHill[functionIndex]);
    SetFunctionMax({ maxHill[functionIndex][1] }, maxHill[functionIndex][0]);
  }
};
