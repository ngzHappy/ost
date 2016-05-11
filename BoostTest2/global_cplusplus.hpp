#ifndef _PRAGMA_ONCE_CPLUSPLUS__HXX_0x00_
#define _PRAGMA_ONCE_CPLUSPLUS__HXX_0x00_

#ifdef __cplusplus
#include <new>
#include <array>
#include <mutex>
#include <bitset>
#include <memory>
#include <limits>
#include <cerrno>
#include <thread>
#include <future>
#include <atomic>
#include <chrono>
#include <complex>
#include <cassert>
#include <utility>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <cstdbool>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <initializer_list>
using namespace std::chrono_literals;
#else
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#endif

#ifndef cvalue
#define cvalue(_value_,...) __VA_ARGS__ /**/_value_
#endif

#ifndef cvar
#define cvar(_value_,...) __VA_ARGS__ /**/var##_value_
#endif

#ifndef carg
#define carg(_value_,...) __VA_ARGS__ /**/arg##_value_
#endif

#ifndef ctype
#define ctype(...) __VA_ARGS__
#endif

#ifndef cexpr
#define cexpr(...) (__VA_ARGS__)
#endif

#ifndef cfun
#define cfun auto
#endif

#ifdef __cplusplus
#include <set>
#include <map>
#include <list>
#include <stack>
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#endif

#endif/*cpp base def*/
/*as c++14*/

