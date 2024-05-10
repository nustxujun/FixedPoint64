
[中文](#64位定点数)
# fixed64
Single-header modern C++ fixed-point arithmetic library. 

requires c++11, recommands c++20
## Features
- 64-bits internal storage
- tweakable fraction precision (QM.N)
- protected from intermediatary overflows
- support rounding and saturation
- evaluated in compile time almostly(totally in c++20) 

## QuickStart
```c++
#include <iostream>
#include "fixed64.hpp"

int main(int argc, char* argv[]) 
{
    f64::fixed64<40> a = 1.23; // Q24.40

    std::cout << (a + 1);
    return 0;

}
```
## Description
fixed64 is designed to find a balance between percision and performance. It is stored by int64. The target application scenario is physics engine，which has many distance calculation that cause the overflow on 32-bits fixed-point.

## Contents
## File
- fixed64.hpp fixed-point header
- trig_lut.hpp lut for sin, optional
### Supported Functions
```
- Arithmetic: + - * / fmod sqrt
- Trigonometry: sin cos tan asin acos atan atan2
- Exponential: exp exp2 log log2 log10 pow
- Other: abs ceil floor round
```

### Supported Switcher
```c++
#define FIXED_64_ENABLE_ROUNDING // apply rounding 
#define FIXED_64_ENABLE_OVERFLOW // checking overflow
#define FIXED_64_ENABLE_SATURATING // saturate result
#define FIXED_64_ENABLE_TRIG_LUT // use lut for trigonometric function
#define FIXED_64_FORCE_EVALUATE_IN_COMPILE_TIME //make all function be with constexpr, clz will use soft implemention
```
## Compare with other fixed-point arithmetic libraries
- **[fpm](https://github.com/MikeLankamp/fpm)** good coding style,but has no overflow protection/alert, need to provide int128 as intermediate type by yourself
- **[libfixmath64](https://github.com/jussihi/libfixmath64)** gives me great inspiration. but it is c-style and can not evaluate fixed-point value in compile time.
- **[fixmath](https://github.com/MichaelSuen-thePointer/fixmath)** I think the best one. support hardware int128, but need to include too many headers
- **[fixed_point](https://github.com/johnmcfarlane/fixed_point)**, **[fp](https://github.com/mizvekov/fp)** perform incorrect result in multiply operation when the fraction has large bits(intermediatary overflow happens).




# 64位定点数
## 特性
- 64位存储
- 可变小数位精度
- 越界保护/警告
- 支持四舍五入和越界限制
- 支持大部分函数的编译其计算（c++20全部支持）

## 快速上手
```c++
#include <iostream>
#include "fixed64.hpp"

int main(int argc, char* argv[]) 
{
    f64::fixed64<40> a = 1.23; // Q24.40

    std::cout << (a + 1);
    return 0;

}
```

## 简介
fixed64综合考虑了精度与性能的问题，使用了int64存储。个人的应用场景是物理引擎。物理引擎中存在大量距离计算，使用32位的定点数容易发生溢出。

## 内容
## 文件
- fixed64.hpp 定点数头文件
- trig_lut.hpp 三角函数查表文件，不是必须
### 支持的函数
```
- 算数操作: + - * / fmod sqrt
- 三角函数: sin cos tan asin acos atan atan2
- 指数函数: exp exp2 log log2 log10 pow
- 其他函数: abs ceil floor round
```

### 开关
```c++
#define FIXED_64_ENABLE_ROUNDING // 使用四舍五入
#define FIXED_64_ENABLE_OVERFLOW // 使用越界检查
#define FIXED_64_ENABLE_SATURATING // 使用越界限制
#define FIXED_64_ENABLE_TRIG_LUT // 三角函数使用查表方法
#define FIXED_64_FORCE_EVALUATE_IN_COMPILE_TIME //强制使函数支持编译期运算，主要是改变clz的实现
```


## 与其他定点数库对比
- **[fpm](https://github.com/MikeLankamp/fpm)** 优秀的代码风格，但是没有计算溢出检查，需要自己提供int128来支持64位的定点数实现。
- **[libfixmath64](https://github.com/jussihi/libfixmath64)** 给我很大启发. 但是是C风格代码，缺少编译期运算支持。
- **[fixmath](https://github.com/MichaelSuen-thePointer/fixmath)** 个人认为最好的定点数库. 支持硬件int128运算，但是头文件太多。
- **[fixed_point](https://github.com/johnmcfarlane/fixed_point)**, **[fp](https://github.com/mizvekov/fp)** 这些库没有考虑当小数部分占位数较多时，乘除法计算中间值越界问题,这将导致计算结果错误。
