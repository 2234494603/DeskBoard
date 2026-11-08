/**
 * @file Calc.h
 * @brief CN：算术函数指针的声明集合。设计意图是把“做什么运算”与“用哪种实现”
 *        解耦，上层只需给指针赋值即可切换算法；但本工程中没有任何位置对这些
 *        指针赋值，因此它们始终是空指针。
 *        -- EN：Declarations of arithmetic function pointers that decouple the
 *        operation from its implementation: assigning a pointer switches the
 *        algorithm. No assignment exists anywhere in this project, so all of
 *        them stay null.
 */

#ifndef _CALC_H_
#define _CALC_H_

#include "System_config.h"


int add(int a, int b);
int subtraction(int a , int b);
int multiplication (int a , int b);
int division (int a , int b);


#endif
