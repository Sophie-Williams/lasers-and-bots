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

#include "audio_helper.h"
#include "audio/include/AudioEngine.h"

using namespace experimental;
audio_helper::audio_helper():
  initiated_(false),
  music_muted_(false),
  effects_muted_(false),
  last_music_(AudioEngine::INVALID_AUDIO_ID)
{
  // init object
  init();
}

audio_helper::~audio_helper()
{
  // if we are init we need to end
  if (initiated_)
  {
    end();
  }
}

bool audio_helper::init()
{
  // get configuration
  set_effects_muted(UserDefault::getInstance()->getBoolForKey("effects_muted", effects_muted_));
  set_music_muted(UserDefault::getInstance()->getBoolForKey("music_muted", music_muted_));

  // not music played, yet
  last_music_ = AudioEngine::INVALID_AUDIO_ID;

  AudioEngine::lazyInit();

  // object is init
  initiated_ = true;

  return true;
}

void audio_helper::end()
{
  AudioEngine::stopAll();
  AudioEngine::uncacheAll();

  // end audio engine
  AudioEngine::end();

  // save config values
  save_values();

  // we are not init
  initiated_ = false;
}

int audio_helper::play_effect(const std::string& file_name) const
{
  // if we are muted exit
  if (get_effects_muted())
  {
    return AudioEngine::INVALID_AUDIO_ID;
  }
  // play effect
  const auto id = AudioEngine::play2d(file_name);
  AudioEngine::setVolume(id, .5f);
  return id;
}

void audio_helper::play_music(const std::string& file_name)
{
  // if we are muted exit
  if (get_music_muted())
  {
    return;
  }
  // play music
  last_music_ = AudioEngine::play2d(file_name, true);
}

void audio_helper::pre_load_effect(const std::string& file_name)
{
  // pre-load effect
  AudioEngine::preload(file_name);
}

void audio_helper::pre_load_music(const std::string& file_name)
{
  // pre-load music
  AudioEngine::preload(file_name);
}

void audio_helper::unload_effect(const std::string& file_name)
{
  AudioEngine::uncache(file_name);
}

void audio_helper::toggle_sound()
{
  // if we are muted
  if (get_effects_muted())
  {
    // un mute
    set_effects_muted(false);
  }

  else
  {
    // stop all effects and set to mute
    AudioEngine::stopAll();
    set_effects_muted(true);
  }

  // save config values
  save_values();
}

void audio_helper::toggle_music()
{
  // if we are mute
  if (get_music_muted())
  {
    // un mute
    set_music_muted(false);

    // if we have last music
    if (last_music_ != AudioEngine::INVALID_AUDIO_ID)
    {
      // play again
      AudioEngine::resume(last_music_);
    }
  }

  else
  {
    if (last_music_ != AudioEngine::INVALID_AUDIO_ID)
    {
      // stop music and mute
      AudioEngine::pause(last_music_);
    }
    set_music_muted(true);
  }

  // save config values
  save_values();
}

void audio_helper::app_to_bg() const
{
  // if we wasn't muted pause music
  if (!get_music_muted())
  {
    if (last_music_ != AudioEngine::INVALID_AUDIO_ID)
    {
      AudioEngine::pause(last_music_);
    }
  }
}

void audio_helper::app_to_fg() const
{
  // if we wasn't muted resume music
  if (!get_music_muted())
  {
    if (last_music_ != AudioEngine::INVALID_AUDIO_ID)
    {
      AudioEngine::resume(last_music_);
    }
  }
}

void audio_helper::save_values() const
{
  // save config values
  UserDefault::getInstance()->setBoolForKey("effects_muted", effects_muted_);
  UserDefault::getInstance()->setBoolForKey("music_muted_", music_muted_);

  UserDefault::getInstance()->flush();
}

void audio_helper::app_exit()
{
  end();
}

void audio_helper::stop_all_sounds()
{
  AudioEngine::stopAll();
}
