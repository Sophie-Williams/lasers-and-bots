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

#ifndef __LASER_CLASS__
#define __LASER_CLASS__

#include "../utils/utils.h"

class laser_object final : public Node
{
public:
  // base_class
  using base_class = Node;

  // constructor
  laser_object();

  // create the object
  static laser_object* create(const float initial_angle);

  // init this object
  bool init(const float initial_angle);

  // update our laser
  void update(float delta) override;

private:

  // create a emitter
  void create_emitter(const Vec2& point);

  // laser angle
  float angle_;

  // laser draw node
  DrawNode* draw_;

  // the physics world
  PhysicsWorld* physics_world_;

  // maximum laser length
  static constexpr float max_laser_length = 10000.0f;
};

#endif // __LASER_CLASS__