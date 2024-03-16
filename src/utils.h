/*
 * Note to the students: to better understand this test utils, you could check these references:
 * Standard predefined GCC macros: https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
 */
#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <source_location>

using namespace std;
/* The following macro is defined at compile time using the "C" code style, but it is really unsafe, since it
 * does not allow for type checking. Your task is to modify the code using C++ compile-time features (i.e. templates)
 * to obtain a similar result, but with type check and other controls
 *
 * TDD HW3 #1: refactor the code using templates and without changing the firm of the TEST macro (use functional
 * objects)
 */
//extern int tests_total;
//extern int tests_fail;
//#define TEST(EX) do {\
//  if(!(EX))fprintf(stderr, "Test %3d FAILED (%3d failed so far): [%20s] testing [%20s] in %20s, line %5d\n", tests_total++, tests_fail++,  __FUNCTION__, #EX, __FILE__, __LINE__); \
//  else     fprintf(stderr, "Test %3d PASSED (%3d failed so far): [%20s] testing [%20s] in %20s, line %5d\n", tests_total++, tests_fail  ,  __FUNCTION__, #EX, __FILE__, __LINE__); \
//  } while (0)

/*
 * The following solution is designed to do not modify the test0.cpp file, but only the utils.h and utils.cpp.
 * Removing this constraint would allow for a better solution, cleaner and easy to understand. In this case, we are
 * trying to mimic the C macro function that was defined before (see above).
 */
struct MyTest {
    static int tests_total;
    static int tests_fail;

public:
    inline void operator()(bool expr, source_location fun = source_location::current()){
        if(!expr) {
            fprintf(stderr, "Test %3d FAILED (%3d failed so far): on function [%20s] in %20s, line %5d\n",
                    tests_total++, tests_fail++,  fun.function_name(), fun.file_name(), fun.line());
        }
        else {
            fprintf(stderr, "Test %3d FAILED (%3d failed so far): on function [%20s] in [%20s, line %5d\n",
                    tests_total++, tests_fail, fun.function_name(), fun.file_name(), fun.line());
        }
    }
};

extern const int& tests_total;
extern const int& tests_fail;
extern MyTest TEST; //the actual funciton object has to be declared global to work as the C macro



constexpr float TEST_EPS=0.005;
inline bool within_eps(float a, float b) { return a-TEST_EPS<b && b<a+TEST_EPS; }



class __ProfileScopeClass
{
unsigned long long start;
const std::string fname;
const std::string name;
int level;
int line;

static unsigned long long get_cpu_time_raw_int(void)
  {
  //struct timespec t1;
  //clock_gettime(CLOCK_REALTIME,&t1);
  //return (unsigned long long)t1.tv_sec*(unsigned long long)1000000000+(unsigned long long)t1.tv_nsec;
  long long c1=std::chrono::high_resolution_clock::now().time_since_epoch().count();
  return c1;
  }


public:


__ProfileScopeClass(int line,const std::string& fname,int level,const std::string& name="")
  :fname(fname), name(name), level(level), line(line)
  {
  start=get_cpu_time_raw_int();
  }

~__ProfileScopeClass()
  {
  if(level==1)printf("%30s(%4d) :  %lf ms\n",(name+"@"+fname).c_str(),line,double(get_cpu_time_raw_int()-start)/1e6);
  if(level==2)printf("%30s(%4d) :  %lf us\n",(name+"@"+fname).c_str(),line,double(get_cpu_time_raw_int()-start)/1e3);
  if(level==3)printf("%30s(%4d) :  %lf ns\n",(name+"@"+fname).c_str(),line,double(get_cpu_time_raw_int()-start));
  }

};

#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)


#define TIME(...) __ProfileScopeClass COMBINE(__t,__LINE__) (__LINE__,__FUNCTION__,__VA_ARGS__)


#define NOT_IMPLEMENTED() do{static bool done=false;if(!done)fprintf(stderr,"Function \"%s\"  in file \"%s\" line %d not implemented yet!!!\n",__FUNCTION__, __FILE__, __LINE__);done=true;}while(0)


