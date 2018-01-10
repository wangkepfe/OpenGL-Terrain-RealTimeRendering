#include "utilityFunc.h"
#include<cstdlib>

float rand0to1()
{
	return ((float)rand()) / RAND_MAX;
}

float randm1to1()
{
	return rand0to1() * 2.f - 1.f;
}