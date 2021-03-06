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

#ifndef __BASIC_SCENE_H__
#define __BASIC_SCENE_H__

#include "../../utils.h"

// forward declarations
class basic_app;
class audio_helper;

class basic_scene : public Scene
{
public:
  using base_class = Scene;

  basic_scene();

  static base_class* create_scene(basic_app* application);

  bool init(basic_app* application);

  Size get_screen_size() const noexcept
  {
    return screen_size_;
  }

  basic_app* get_application() const
  {
    return application_;
  }

  audio_helper* get_audio_helper() const;

protected:

  void onExit() override;

  Size screen_size_;

  basic_app* application_;
};

#endif // __BASIC_SCENE_H__
