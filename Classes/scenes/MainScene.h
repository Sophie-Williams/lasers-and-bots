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
#ifndef _MAIN_SCENE__
#define _MAIN_SCENE__

#include "../utils/utils.h"
#include "../utils/base/scene/TiledScene.h"

class MainScene : public TiledScene
{
public:
  // parent
  typedef TiledScene parent;

  // constructor
  MainScene();

  // destructor
  ~MainScene();

  // create the object
  static MainScene* create();

  // create the scene
  static Scene* scene();

  // init this object
  virtual bool init();

  virtual void onEnter() override;

protected:

private:

  bool createKeybordListener();
  void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
  void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

  bool moveLeft;
  bool moveRight;
  bool moveUp;
  bool moveDown;
  virtual void update(float delta);

  Sprite *bot;

  //cpSpace* _space; // strong ref
  void initPhysics();

};

#endif // _MAIN_SCENE__
