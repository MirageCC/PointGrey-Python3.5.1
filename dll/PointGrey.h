#pragma once
#include "stdafx.h"
#include "FlyCapture2.h"
#include <iomanip>
#include <iostream>
#include <sstream>


using namespace FlyCapture2;
using namespace std;


#define FlyCapture extern "C" _declspec(dllexport)   

FlyCapture int PT_Init();
FlyCapture int PT_Grab(char* FileName);
FlyCapture int PT_Stop();

