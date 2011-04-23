#ifndef __STLHEADER_HH
#define __STLHEADER_HH

//ansi c library
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>

//stl container
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <utility>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>

// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#if _MSC_VER >1300 && !defined(_STLP_MSVC)
#include <hash_map>
#include <hash_set>
#endif

#  undef min
#  undef max

#endif