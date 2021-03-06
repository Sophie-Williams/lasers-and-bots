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
#ifndef __ROBOT_FRAGMENT_CLASS__
#define __ROBOT_FRAGMENT_CLASS__

#include "../utils/physics/physics_game_object.h"

class robot_fragment final : public physics_game_object
{
public:
  using base_class = physics_game_object;

  robot_fragment();

  static robot_fragment* create(physics_shape_cache* physics_shape_cache, const int fragment_number);

  bool create_smoke_emitter();

  bool init(physics_shape_cache* physics_shape_cache, const int fragment_number);

  void explode(const Vec2& velocity);

  void pause() override;

protected:

  void update(float delta) override;

private:

  ParticleSystemQuad* smoke_;

  bool exploding_;
};

#endif // __ROBOT_FRAGMENT_CLASS__
