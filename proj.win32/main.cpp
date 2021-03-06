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

#include "main.h"
#include "laser_and_bots_app.h"
#include "laser_and_bots_launcher.h"

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
  const auto launcher_app = new laser_and_bots_launcher();
  auto result = launcher_app->run();

  const auto want_to_play = launcher_app->get_want_to_play();
  const auto screen_width = launcher_app->get_screen_width();
  const auto screen_height = launcher_app->get_screen_height();
  const auto full_screen = launcher_app->is_full_screen();

  delete launcher_app;

  if (want_to_play)
  {
    const auto app = new laser_and_bots_app(screen_width, screen_height, full_screen);
    result = app->run();
    delete app;
  }

  return result;
}
