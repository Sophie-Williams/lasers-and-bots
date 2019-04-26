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

#include "virtual_joy_stick.h"
#include "game_ui.h"
#include "on_screen_button.h"

virtual_joy_stick::virtual_joy_stick():
  key_left_(false),
  key_right_(false),
  key_up_(false),
  button_a_keyboard_(false),
  button_b_keyboard_(false),
  on_screen_button_left_(nullptr),
  on_screen_button_right_(nullptr),
  on_screen_button_up_(nullptr)
{
}

virtual_joy_stick* virtual_joy_stick::create()
{
  virtual_joy_stick* ret = nullptr;

  do
  {
    auto object = new virtual_joy_stick();
    UTILS_BREAK_IF(object == nullptr);

    if (object->init())
    {
      object->autorelease();
    }
    else
    {
      delete object;
      object = nullptr;
    }

    ret = object;
  }
  while (false);

  // return the object
  return ret;
}

on_screen_button* virtual_joy_stick::add_on_screen_button(const std::string& normal_sprite_frame_name,
                                                          const std::string& pushed_sprite_frame_name)
{
  on_screen_button* button = nullptr;
  do
  {
    button = on_screen_button::create(normal_sprite_frame_name, pushed_sprite_frame_name);
    UTILS_BREAK_IF(button == nullptr);

    addChild(button);

    on_screen_buttons_.push_back(button);
  }
  while (false);

  return button;
}

bool virtual_joy_stick::add_on_screen_buttons()
{
  auto ret = false;
  do
  {
    // left button
    on_screen_button_left_ = add_on_screen_button("02_joystick_left_01.png", "02_joystick_left_02.png");
    UTILS_BREAK_IF(on_screen_button_left_ == nullptr);

    const auto left_button_pos = Vec2(on_screen_button_left_->getContentSize().width / 2,
                                      on_screen_button_left_->getContentSize().height / 2);
    on_screen_button_left_->setPosition(left_button_pos);

    // right button
    on_screen_button_right_ = add_on_screen_button("02_joystick_right_01.png", "02_joystick_right_02.png");
    UTILS_BREAK_IF(on_screen_button_right_ == nullptr);

    const auto right_button_pos = left_button_pos + Vec2(
      on_screen_button_left_->getContentSize().width + (on_screen_button_left_->getContentSize().width / 2), 0.f);
    on_screen_button_right_->setPosition(right_button_pos);

    // up button
    on_screen_button_up_ = add_on_screen_button("02_joystick_up_01.png", "02_joystick_up_02.png");
    UTILS_BREAK_IF(on_screen_button_up_ == nullptr);

    const auto size = Director::getInstance()->getOpenGLView()->getVisibleSize();
    const auto up_button_pos = Vec2(size.width - (on_screen_button_up_->getContentSize().width / 2),
                                    on_screen_button_up_->getContentSize().height / 2);
    on_screen_button_up_->setPosition(up_button_pos);

    ret = true;
  }
  while (false);

  return ret;
}

bool virtual_joy_stick::init()
{
  auto ret = false;

  do
  {
    //////////////////////////////
    // 1. super init first
    UTILS_BREAK_IF(!base_class::init());

    // add on screen buttons
    UTILS_BREAK_IF(!add_on_screen_buttons());

    // create touch listener
    UTILS_BREAK_IF(!create_touch_listener());

    // create keyboard listener
    UTILS_BREAK_IF(!create_keyboard_listener());

    ret = true;
  }
  while (false);

  return ret;
}

void virtual_joy_stick::reset_on_screen_buttons()
{
  for (const auto button : on_screen_buttons_)
  {
    button->pushed(false);
  }
}

Vec2 virtual_joy_stick::get_location_in_node_space(Touch* touch)
{
  const auto point_one = Director::getInstance()->convertToUI(touch->getLocationInView());
  const auto ui = dynamic_cast<game_ui*>(getParent());
  const auto location = point_one + ui->getPosition();
  return location;
}

bool virtual_joy_stick::on_touches_began(const std::vector<Touch*>& touches, Event* unused_event)
{
  reset_on_screen_buttons();

  for (const auto touch : touches)
  {
    const auto location = get_location_in_node_space(touch);

    for (const auto button : on_screen_buttons_)
    {
      if (button->is_touched_by_location(location))
      {
        button->pushed(true);
        return true;
      }
    }
  }

  return false;
}

void virtual_joy_stick::on_touches_moved(const std::vector<Touch*>& touches, Event* unused_event)
{
  on_touches_began(touches, unused_event);
}

void virtual_joy_stick::on_touches_ended(const std::vector<Touch*>& touches, Event* unused_event)
{
  reset_on_screen_buttons();
}

void virtual_joy_stick::on_touches_cancel(const std::vector<Touch*>& touches, Event* unused_event)
{
  on_touches_ended(touches, unused_event);
}

bool virtual_joy_stick::create_touch_listener()
{
  auto ret = false;

  do
  {
    // Register Touch Event
    const auto listener = EventListenerTouchAllAtOnce::create();
    UTILS_BREAK_IF(listener == nullptr);

    listener->onTouchesBegan = CC_CALLBACK_2(virtual_joy_stick::on_touches_began, this);
    listener->onTouchesEnded = CC_CALLBACK_2(virtual_joy_stick::on_touches_ended, this);
    listener->onTouchesMoved = CC_CALLBACK_2(virtual_joy_stick::on_touches_moved, this);
    listener->onTouchesCancelled = CC_CALLBACK_2(virtual_joy_stick::on_touches_cancel, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    ret = true;
  }
  while (false);

  return ret;
}

bool virtual_joy_stick::create_keyboard_listener()
{
  auto result = false;

  do
  {
    auto listener = EventListenerKeyboard::create();
    UTILS_BREAK_IF(listener == nullptr);

    listener->onKeyPressed = CC_CALLBACK_2(virtual_joy_stick::on_key_pressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(virtual_joy_stick::on_key_released, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    result = true;
  }
  while (false);

  return result;
}

void virtual_joy_stick::on_key_pressed(const EventKeyboard::KeyCode key_code, Event*)
{
  switch (key_code)
  {
  case EventKeyboard::KeyCode::KEY_UP_ARROW:
  case EventKeyboard::KeyCode::KEY_W:
    key_up_ = true;
    break;
  case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case EventKeyboard::KeyCode::KEY_A:
    key_left_ = true;;
    break;
  case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case EventKeyboard::KeyCode::KEY_D:
    key_right_ = true;
    break;
  case EventKeyboard::KeyCode::KEY_SPACE:
    button_a_keyboard_ = true;
    break;
  case EventKeyboard::KeyCode::KEY_LEFT_CTRL:
  case EventKeyboard::KeyCode::KEY_RIGHT_CTRL:
    button_b_keyboard_ = true;
    break;
  default:
    break;
  }
}

void virtual_joy_stick::on_key_released(const EventKeyboard::KeyCode key_code, Event*)
{
  switch (key_code)
  {
  case EventKeyboard::KeyCode::KEY_UP_ARROW:
  case EventKeyboard::KeyCode::KEY_W:
    key_up_ = false;
    break;
  case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case EventKeyboard::KeyCode::KEY_A:
    key_left_ = false;;
    break;
  case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case EventKeyboard::KeyCode::KEY_D:
    key_right_ = false;
    break;
  case EventKeyboard::KeyCode::KEY_SPACE:
    button_a_keyboard_ = false;
    break;
  case EventKeyboard::KeyCode::KEY_LEFT_CTRL:
  case EventKeyboard::KeyCode::KEY_RIGHT_CTRL:
    button_b_keyboard_ = false;
    break;
  default:
    break;
  }
}

bool virtual_joy_stick::get_left() const
{
  return key_left_ || on_screen_button_left_->is_pushed();
}

bool virtual_joy_stick::get_right() const
{
  return key_right_ || on_screen_button_right_->is_pushed();
}

bool virtual_joy_stick::get_up() const
{
  return key_up_ || on_screen_button_up_->is_pushed();
}

bool virtual_joy_stick::button_a_down() const
{
  return button_a_keyboard_;
}

bool virtual_joy_stick::button_b_down() const
{
  return button_b_keyboard_;
}
