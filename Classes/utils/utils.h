/****************************************************************************
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

// include cocos2d headers
#include "cocos2d.h"

// using cocos2d namespace
USING_NS_CC;
using namespace std;

#define DESKTOP_GAME  101
#define MOBILE_GAME   102

#ifndef GAME_PLATFORM
  #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    #define GAME_PLATFORM DESKTOP_GAME
  #else
    #define GAME_PLATFORM MOBILE_GAME
  #endif
#endif

// log a line with the format <class::function> : <data to be logged> -> <file>(<line number>)
#define UTILS_LOG(s, ...) CCLOG(string(string("%s : ") + string(s) + string(" -> %s (%d)")).c_str(), __FUNCTION__, ##__VA_ARGS__, __FILE__, __LINE__)

// Replace for CC_BREAK_IF that logs when condition is true
#define UTILS_BREAK_IF(cond)            \
    if (cond)                           \
    {                                   \
        UTILS_LOG("(%s)", "" #cond ""); \
        break;                          \
    }

std::string string_format(std::string fmt_str, ...);

static const float fuzzy_range = 5.f;

// helper for fuzzy equals
bool fuzzy_equals(const float a, const float b, const float var = fuzzy_range) noexcept;

#endif // __UTILS_H__
