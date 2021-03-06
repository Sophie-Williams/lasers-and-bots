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

#ifndef __AUDIO_HELPER_H__
#define __AUDIO_HELPER_H__

#include "../utils.h"

// class that represent a audio engine
class audio_helper final : public Ref
{
public:
  typedef Ref parent;

  audio_helper();
  ~audio_helper();

  int play_effect(const std::string& file_name, const bool loop = false, const float volume = 1.0f) const;
  void play_music(const std::string& file_name, const float volume = 1.0f);

  void pre_load_effect(const std::string& file_name);
  void pre_load_music(const std::string& file_name);
  void unload_effect(const std::string& file_name);
  void stop_all_sounds();

  void toggle_sound();
  void toggle_music();
  void pause_music() const;
  void resume_music() const;

  void app_exit();

  void stop_sound(const int sound);
  void pause_sound(const int sound);
  void resume_sound(const int sound);

  bool get_music_muted() const noexcept
  {
    return music_muted_;
  }

  void set_music_muted(const bool var) noexcept
  {
    music_muted_ = var;
  };

  bool get_effects_muted() const noexcept
  {
    return effects_muted_;
  }

  void set_effects_muted(const bool var) noexcept
  {
    effects_muted_ = var;
  };

  int get_last_music() const noexcept
  {
    return last_music_;
  };

  void end();

private:

  bool init();

  bool initiated_;
  bool music_muted_;
  bool effects_muted_;
  int last_music_;
};

#endif // __AUDIO_HELPER_H__
