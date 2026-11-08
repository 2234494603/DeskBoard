/**
 * @file Calc.c
 * @brief CN：四则运算实现文件，每个函数都在 LOG_SWITCH 控制下输出一条调试
 *        轨迹，再完成一次纯计算并返回结果，不产生任何副作用。
 *        -- EN：Implementation of the four basic arithmetic operations. Each
 *        function optionally prints a debug trace under LOG_SWITCH, performs a
 *        single pure computation and returns the result without side effects.
 */

#include "Calc.h"

int (*add_pointer)(int a, int b)            = add;
int (*subtraction_pointer)(int a, int b)    = subtraction;
int (*multiplication_pointer)(int a, int b) = multiplication;
int (*division_pointer)(int a, int b)       = division;


/**
 * @brief CN：返回两个 int 之和。函数先按 LOG_SWITCH 决定是否输出调试轨迹，再用
 *        局部变量 c 暂存加法结果并返回；形参只读，不做溢出检测。
 *        -- EN：Return the sum of two ints. A LOG_SWITCH branch optionally
 *        prints a trace, then the local c holds the sum and is returned; the
 *        parameters stay read-only and no overflow check is performed.
 *
 * @param a CN：输入，第一个加数，int 类型，按值传递，函数内不修改。
 *        -- EN：Input first addend, int passed by value and never modified.
 *
 * @param b CN：输入，第二个加数，int 类型，按值传递，函数内不修改。
 *        -- EN：Input second addend, int passed by value and never modified.
 *
 * @return CN：返回 a + b 的结果；若结果超出 int 可表示范围，C 标准未定义其行为，
 *         本函数也不做截断或饱和处理。
 *         -- EN：Return a + b; if the result exceeds the int range the C
 *         standard leaves the behavior undefined and no clamping is applied.
 */
int add(int a , int b)
{
#if LOG_SWITCH
    printf("运行函数add\n");  // CN：输出函数调用轨迹，受 LOG_SWITCH 控制，该宏为 0 时本行不参与编译。-- EN：Print a call trace guarded by LOG_SWITCH; with the macro set to 0 this line is not compiled.
#endif

    int c = 0 ;           // CN：定义结果变量并初始化为 0，随后用于保存两数之和。-- EN：Define the result variable initialized to 0, later holding the sum.

    c = a + b ;           // CN：执行有符号整数加法，把两个形参之和写入 c。-- EN：Perform signed integer addition and store the sum of both parameters in c.

    return c;             // CN：返回计算结果。-- EN：Return the computed sum.
}

/**
 * @brief CN：返回两个 int 之差。流程与 add() 完全对称：先按 LOG_SWITCH 决定是否
 *        打印轨迹，再计算 a - b 并返回；不修改形参，不做溢出检测。
 *        -- EN：Return the difference of two ints. The flow mirrors add(): a
 *        LOG_SWITCH branch optionally prints a trace, then a - b is computed
 *        and returned; parameters stay read-only and no overflow check is done.
 *
 * @param a CN：输入，被减数，int 类型，按值传递。
 *        -- EN：Input minuend, int passed by value.
 *
 * @param b CN：输入，减数，int 类型，按值传递。
 *        -- EN：Input subtrahend, int passed by value.
 *
 * @return CN：返回 a - b 的结果；结果溢出 int 范围时行为遵循 C 标准的未定义规则。
 *         -- EN：Return a - b; overflow of the int range follows the undefined
 *         behavior defined by the C standard.
 */
int subtraction(int a , int b)
{
#if LOG_SWITCH
    printf("运行函数add\n");  // CN：输出调试轨迹，字符串内容仍是 add（原样保留，见文末缺陷说明）。-- EN：Print the debug trace; the string still says add (left untouched, see the defect notes).
#endif

    int c = 0 ;           // CN：定义结果变量并初始化为 0，随后保存差值。-- EN：Define the result variable initialized to 0, later holding the difference.

    c = a - b ;           // CN：执行有符号整数减法，把差值写入 c。-- EN：Perform signed integer subtraction and store the difference in c.

    return c ;            // CN：返回差值。-- EN：Return the difference.
}

/**
 * @brief CN：返回两个 int 之积。仍在 LOG_SWITCH 控制下打印轨迹，再计算 a * b；
 *        乘法不检查结果是否溢出，也不对形参做任何范围约束。
 *        -- EN：Return the product of two ints. The LOG_SWITCH trace is printed
 *        first, then a * b is computed; neither the result overflow nor the
 *        parameter ranges are checked.
 *
 * @param a CN：输入，第一个乘数，int 类型，按值传递。
 *        -- EN：Input first factor, int passed by value.
 *
 * @param b CN：输入，第二个乘数，int 类型，按值传递。
 *        -- EN：Input second factor, int passed by value.
 *
 * @return CN：返回 a * b 的结果；乘积超出 int 范围时行为未定义，调用方需自行保证
 *         取值不会溢出。
 *         -- EN：Return a * b; when the product exceeds the int range the
 *         behavior is undefined, so the caller must keep the inputs in range.
 */
int multiplication (int a , int b)
{
#if LOG_SWITCH
    printf("运行函数add\n");  // CN：输出调试轨迹，字符串内容仍是 add（原样保留）。-- EN：Print the debug trace; the string still says add (left untouched).
#endif

    int c = 0 ;           // CN：定义结果变量并初始化为 0，随后保存乘积。-- EN：Define the result variable initialized to 0, later holding the product.

    c = a * b ;           // CN：执行有符号整数乘法，把乘积写入 c。-- EN：Perform signed integer multiplication and store the product in c.

    return c ;            // CN：返回乘积。-- EN：Return the product.
}

/**
 * @brief CN：返回 a / b 的整数商。先判断除数是否为 0：非 0 时执行整数除法并把商
 *        写入 c；为 0 时只打印错误提示，c 保持初始值 0 并被返回。因此调用方无法
 *        通过返回值区分“商为 0”和“除零出错”。
 *        -- EN：Return the integer quotient a / b. The divisor is tested first:
 *        when non-zero the division is performed and stored in c; when zero an
 *        error message is printed and c keeps its initial 0, which is returned.
 *        The caller therefore cannot distinguish "quotient is 0" from "error".
 *
 * @param a CN：输入，被除数，int 类型，按值传递。
 *        -- EN：Input dividend, int passed by value.
 *
 * @param b CN：输入，除数，int 类型，按值传递；为 0 时只打印错误，不执行除法。
 *        -- EN：Input divisor, int passed by value; when 0 only an error is
 *        printed and no division is performed.
 *
 * @return CN：返回整数商（向零截断，小数部分丢弃）；除数为 0 时返回初始值 0。
 *         -- EN：Return the integer quotient truncated toward zero; return the
 *         initial 0 when the divisor is 0.
 */
int division (int a , int b)
{
#if LOG_SWITCH
    printf("运行函数add\n");           // CN：输出调试轨迹，字符串内容仍是 add（原样保留）。-- EN：Print the debug trace; the string still says add (left untouched).
#endif

    int c = 0;                     // CN：定义结果变量并初始化为 0，同时作为除零时的返回值。-- EN：Define the result variable initialized to 0, which is also the value returned on division by zero.

    if(b != 0)                     // CN：检查除数是否非 0，只有非 0 才允许执行除法，避免整数除零异常。-- EN：Check that the divisor is non-zero; division is only safe in that branch.
    {
        c = a / b;                 // CN：执行整数除法，商向零截断后写入 c。-- EN：Perform integer division; the quotient truncates toward zero and is stored in c.
    }
    else
    {
        printf("输入有误，被除数应不为0\n");  // CN：除数为 0 时打印提示，避免触发除零异常；文案中的“被除数”实为除数（原样保留）。-- EN：Print a warning when the divisor is 0 instead of dividing; the wording says dividend but means divisor (left untouched).
    }

    return c;                      // CN：返回当前结果：正常时为商，除零时为初始值 0。-- EN：Return the current result: the quotient normally, the initial 0 on division by zero.
}
