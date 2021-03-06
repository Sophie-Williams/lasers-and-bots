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

#include "game_scene.h"
#include "../utils/base/sprite/game_object.h"
#include "../utils/physics/physics_shape_cache.h"
#include "../utils/audio/audio_helper.h"
#include "../game/robot_object.h"
#include "../game/laser_object.h"
#include "../game/switch_object.h"
#include "../game/door_object.h"
#include "../game/barrel_object.h"
#include "../game/box_object.h"
#include "../game/saw_object.h"
#include "../game/robot_fragment.h"
#include "../game/harm_object.h"
#include "../ui/game_ui.h"
#include "../ui/virtual_joy_stick.h"
#include "../laser_and_bots_app.h"

game_scene::game_scene() :
  robot_(nullptr),
  game_ui_(nullptr),
  last_robot_position_(Vec2::ZERO),
  last_camera_position_(Vec2::ZERO),
  min_camera_pos_(Vec2::ZERO),
  max_camera_pos_(Vec2::ZERO),
  paused_(false),
  doing_final_anim_(false),
  doing_delay_start_(false), closing_(false),
  total_time_(0.f),
  time_limit_(0),
  level_name_(""),
  barrel_count_(0)
{
}

Scene* game_scene::scene(basic_app* application, const bool debug_grid, const bool debug_physics)
{
  auto scene = new game_scene();

  if (scene)
  {
    if (scene->init(application, debug_grid, debug_physics))
    {
      scene->autorelease();
    }
    else
    {
      delete scene;
      scene = nullptr;
    }
  }

  return scene;
}

game_scene* game_scene::create(basic_app* application, const bool debug_grid, const bool debug_physics)
{
  game_scene* ret = nullptr;

  do
  {
    auto object = new game_scene();
    UTILS_BREAK_IF(object == nullptr);

    if (object->init(application, debug_physics, debug_grid))
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

  return ret;
}

bool game_scene::init(basic_app* application, const bool debug_grid, const bool debug_physics)
{
  auto ret = false;

  do
  {
    // load the map
    UTILS_BREAK_IF(!base_class::init(application, "maps/map.tmx", gravity, debug_physics));

    calculate_camera_bounds();

    cache_objects_textures();

    UTILS_BREAK_IF(!create_game_ui());

    UTILS_BREAK_IF(!add_objects_to_game());

    UTILS_BREAK_IF(!cache_robot_explosion());

    get_physics_shape_cache()->remove_all_shapes();

    if (debug_grid)
    {
      UTILS_BREAK_IF(!create_debug_grid("fonts/tahoma.ttf"));
    }

    UTILS_BREAK_IF(!create_physics_contacts_callback());

    set_map_bounds_contacts_settings();

    pre_load_sounds();

    get_map_settings();

    // we start with delay
    doing_delay_start_ = true;

    // start game loop
    scheduleUpdate();

    ret = true;
  }
  while (false);

  return ret;
}

bool game_scene::create_game_ui()
{
  auto ret = false;

  do
  {
    game_ui_ = game_ui::create(get_audio_helper());
    UTILS_BREAK_IF(game_ui_ == nullptr);

    game_ui_->setAnchorPoint(Vec2(0.f, 0.f));

    addChild(game_ui_);

    ret = true;
  }
  while (false);

  return ret;
}

void game_scene::calculate_camera_bounds()
{
  min_camera_pos_ = Vec2(screen_size_.width / 2, screen_size_.height / 2);
  max_camera_pos_ = Vec2(total_size_.width - min_camera_pos_.x, total_size_.height - min_camera_pos_.y);
}

bool game_scene::create_physics_contacts_callback()
{
  auto ret = false;

  do
  {
    auto contact_listener = EventListenerPhysicsContact::create();
    UTILS_BREAK_IF(contact_listener == nullptr);

    contact_listener->onContactBegin = CC_CALLBACK_1(game_scene::on_contact_begin, this);
    contact_listener->onContactSeparate = CC_CALLBACK_1(game_scene::on_contact_separate, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contact_listener, this);

    ret = true;
  }
  while (false);

  return ret;
}

void game_scene::set_map_bounds_contacts_settings() const
{
  const auto map = get_tiled_map();
  const auto edge = map->getPhysicsBody();
  edge->setCategoryBitmask(static_cast<unsigned short>(categories::world));
  edge->setContactTestBitmask(static_cast<unsigned short>(categories::feet));
}

void game_scene::pre_load_sounds() const
{
  get_audio_helper()->pre_load_music("sounds/music.mp3");
  get_audio_helper()->pre_load_effect("sounds/fail.mp3");
  get_audio_helper()->pre_load_effect("sounds/victory.mp3");
  get_audio_helper()->pre_load_effect("sounds/countdown.mp3");
  get_audio_helper()->pre_load_effect("sounds/explosion.mp3");
}

void game_scene::get_map_settings()
{
  const auto map = get_tiled_map();

  time_limit_ = static_cast<unsigned int>(map->getProperty("time_limit").asInt());
  level_name_ = map->getProperty("name").asString();
}

void game_scene::cache_objects_textures()
{
  SpriteFrameCache::getInstance()->addSpriteFramesWithFile("objects/objects.plist");
}

Node* game_scene::provide_physics_node(const int gid)
{
  const auto map = get_tiled_map();
  const auto gid_properties = map->getPropertiesForGID(gid);
  const auto shape = get_shape_from_tile_gid(gid);
  if (gid_properties.getType() == Value::Type::MAP)
  {
    const auto& value_map = gid_properties.asValueMap();
    if (value_map.count("damage") == 1)
    {
      const auto damage = value_map.at("damage").asInt();
      return harm_object::create(get_physics_shape_cache(), shape, "dummy", damage);
    }
  }

  return physics_game_object::create(get_physics_shape_cache(), shape, "dummy");
}

void game_scene::update_game_time(const float delta)
{
  total_time_ += delta;
  game_ui_->update_time(total_time_, time_limit_);
}

bool game_scene::update_robot_shield_and_check_if_depleted() const
{
  const auto shield_percentage = robot_->get_shield_percentage();
  game_ui_->set_shield_percentage(shield_percentage);

  return shield_percentage != 0.0f;
}

void game_scene::check_robot_movement(const float delta)
{
  const auto new_position = robot_->getPosition();
  if (last_robot_position_ != new_position)
  {
    camera_follow_robot(new_position, delta);

    last_robot_position_ = new_position;
  }
}

void game_scene::update(float delta)
{
  game_ui_->update(delta);

  if (do_we_need_game_updates())
  {
    update_game_time(delta);

    if (update_robot_shield_and_check_if_depleted())
    {
      robot_->update(delta);

      check_robot_movement(delta);
    }
    else
    {
      explode_robot();
    }
  }
}

bool game_scene::add_laser(const ValueMap& values, Node* layer)
{
  auto ret = false;

  do
  {
    const auto name = values.at("name").asString();
    const auto rotation = values.at("rotation").asFloat();
    const auto position = get_object_center_position(values);
    const auto damage = values.at("damage").asInt();
    const auto speed = values.at("speed").asFloat();

    auto laser = laser_object::create(get_audio_helper(), rotation, speed, damage);
    UTILS_BREAK_IF(laser == nullptr);

    laser->setPosition(position);

    layer->addChild(laser);

    game_objects_[name] = laser;

    ret = true;
  }
  while (false);


  return ret;
}

bool game_scene::add_robot(const ValueMap& values, Node* layer)
{
  auto ret = false;
  do
  {
    const auto shield = values.at("shield").asInt();
    robot_ = robot_object::create(get_physics_shape_cache(), get_audio_helper(), game_ui_->get_virtual_joy_stick(),
                                  shield);
    UTILS_BREAK_IF(robot_ == nullptr);

    auto position = get_object_center_position(values);
    position.y -= (robot_->getContentSize().height - block_size_.height) / 2;

    robot_->setPosition(position);

    layer->addChild(robot_);

    ret = true;
  }
  while (false);

  return ret;
}

bool game_scene::add_switch(const ValueMap& values, Node* layer)
{
  auto ret = false;

  do
  {
    const auto name = values.at("name").asString();
    const auto target = values.at("target").asString();

    auto switch_game_object = switch_object::create(get_physics_shape_cache(), target);
    UTILS_BREAK_IF(switch_game_object == nullptr);

    switch_game_object->setAnchorPoint(Vec2(0.5f, 0.f));

    auto position = get_object_position(values);
    position.y += (values.at("height").asFloat() / 2);

    switch_game_object->setPosition(position);

    layer->addChild(switch_game_object);

    game_objects_[name] = switch_game_object;

    ret = true;
  }
  while (false);

  return ret;
}

bool game_scene::add_door(const ValueMap& values, Node* layer)
{
  auto ret = false;

  do
  {
    const auto name = values.at("name").asString();
    auto door_game_object = door_object::create(get_physics_shape_cache(), get_audio_helper());

    UTILS_BREAK_IF(door_game_object == nullptr);

    door_game_object->setAnchorPoint(Vec2(0.5f, 0.f));

    auto position = get_object_position(values);
    position.y += (values.at("height").asFloat() / 2);

    door_game_object->setPosition(position);

    layer->addChild(door_game_object);

    game_objects_[name] = door_game_object;

    ret = true;
  }
  while (false);

  return ret;
}

bool game_scene::add_barrel(const ValueMap& values, Node* layer)
{
  auto ret = false;

  do
  {
    barrel_count_++;

    const auto name = values.at("name").asString();
    const auto image = values.at("image").asString();
    const auto shape = values.at("shape").asString();
    const auto rotation = values.at("rotation").asFloat();

    auto barrel = barrel_object::create(get_physics_shape_cache(), barrel_count_, image, shape);
    UTILS_BREAK_IF(barrel == nullptr);

    const auto position = get_object_center_position(values);

    barrel->setPosition(position);
    barrel->setRotation(rotation);

    layer->addChild(barrel);

    game_objects_[name] = barrel;

    ret = true;
  }
  while (false);

  return ret;
}

bool game_scene::add_saw(const ValueMap& values, Node* layer)
{
  auto ret = false;

  do
  {
    const auto name = values.at("name").asString();
    const auto image = values.at("image").asString();
    const auto shape = values.at("shape").asString();
    const auto damage = values.at("damage").asInt();
    const auto rotation_time = values.at("rotation_time").asFloat();
    const auto movement = values.at("movement").asFloat();
    const auto movement_time = values.at("movement_time").asFloat();
    const auto stop_time = values.at("stop_time").asFloat();

    auto saw = saw_object::create(get_physics_shape_cache(), image, shape, damage, rotation_time, movement,
                                  movement_time, stop_time);
    UTILS_BREAK_IF(saw == nullptr);

    const auto position = get_object_center_position(values);

    saw->setPosition(position);
    saw->setAnchorPoint(Vec2(0.5f, 0.5f));

    layer->addChild(saw);

    game_objects_[name] = saw;

    ret = true;
  }
  while (false);

  return ret;
}

bool game_scene::add_box(const ValueMap& values, Node* layer)
{
  auto ret = false;

  do
  {
    const auto name = values.at("name").asString();
    const auto image = values.at("image").asString();
    const auto shape = values.at("shape").asString();
    const auto rotation = values.at("rotation").asFloat();

    auto box = box_object::create(get_physics_shape_cache(), image, shape);
    UTILS_BREAK_IF(box == nullptr);

    const auto position = get_object_center_position(values);

    box->setPosition(position);
    box->setRotation(rotation);

    layer->addChild(box);

    game_objects_[name] = box;

    ret = true;
  }
  while (false);

  return ret;
}

bool game_scene::add_object(const vector<Value>::value_type& object)
{
  auto ret = false;
  do
  {
    const auto map = get_tiled_map();
    UTILS_BREAK_IF(map == nullptr);

    const auto layer_walk = map->getLayer("walk");
    UTILS_BREAK_IF(layer_walk == nullptr);

    const auto layer_walk_back = map->getLayer("walk_back");
    UTILS_BREAK_IF(layer_walk_back == nullptr);

    const auto& values = object.asValueMap();
    const auto type = values.at("type").asString();

    if (type == "laser")
    {
      UTILS_BREAK_IF(!add_laser(values, layer_walk));
    }
    else if (type == "robot")
    {
      UTILS_BREAK_IF(!add_robot(values, layer_walk));
    }
    else if (type == "saw")
    {
      UTILS_BREAK_IF(!add_saw(values, layer_walk_back));
    }
    else if (type == "barrel")
    {
      UTILS_BREAK_IF(!add_barrel(values, layer_walk_back));
    }
    else if (type == "switch")
    {
      UTILS_BREAK_IF(!add_switch(values, layer_walk));
    }
    else if (type == "door")
    {
      UTILS_BREAK_IF(!add_door(values, layer_walk_back));
    }
    else if (type == "box")
    {
      UTILS_BREAK_IF(!add_box(values, layer_walk));
    }

    ret = true;
  }
  while (false);
  return ret;
}

bool game_scene::add_objects_to_game()
{
  auto result = false;

  do
  {
    const auto map = get_tiled_map();
    const auto objects = map->getObjectGroup("objects");

    for (const auto& object : objects->getObjects())
    {
      UTILS_BREAK_IF(!add_object(object));
    }

    result = true;
  }
  while (false);

  return result;
}

unsigned short int game_scene::calculate_stars() const
{
  unsigned short int stars = 1;

  if (total_time_ <= time_limit_)
  {
    stars = 2;
    if (robot_->get_shield_percentage() == 100.f)
    {
      stars = 3;
    }
  }

  return stars;
}

void game_scene::move_fragments_to_robot()
{
  const auto pos = robot_->getPosition();
  const auto position = Vec2(pos.x, pos.y + robot_->getContentSize().height / 2.5);

  robot_fragments_[5]->setPosition(position);
  robot_fragments_[0]->setPosition(Vec2(
    position.x,
    position.y + robot_fragments_[5]->getContentSize().height / 2 +
    (robot_fragments_[0]->getContentSize().height / 2)
  ));
  robot_fragments_[1]->setPosition(Vec2(
    position.x - (robot_fragments_[5]->getContentSize().width / 2),
    robot_fragments_[5]->getPosition().y
  ));
  robot_fragments_[2]->setPosition(Vec2(
    position.x + (robot_fragments_[5]->getContentSize().width / 2),
    robot_fragments_[5]->getPosition().y
  ));
  robot_fragments_[3]->setPosition(Vec2(
    position.x - (robot_fragments_[5]->getContentSize().width / 2),
    robot_fragments_[5]->getPosition().y -
    (robot_fragments_[5]->getContentSize().height / 2)
  ));
  robot_fragments_[4]->setPosition(Vec2(
    position.x + (robot_fragments_[5]->getContentSize().width / 2),
    robot_fragments_[5]->getPosition().y -
    (robot_fragments_[5]->getContentSize().height / 2)
  ));
}

bool game_scene::cache_robot_explosion()
{
  auto ret = false;
  do
  {
    for (auto fragment_number = 1; fragment_number <= 6; ++fragment_number)
    {
      const auto robot_fragment = robot_fragment::create(get_physics_shape_cache(), fragment_number);
      UTILS_BREAK_IF(robot_fragment == nullptr);

      robot_fragments_.push_back(robot_fragment);
      get_tiled_map()->getLayer("walk")->addChild(robot_fragment);
    }

    ret = true;
  }
  while (false);

  return ret;
}

void game_scene::explode_robot()
{
  get_audio_helper()->play_effect("sounds/explosion.mp3");

  game_ui_->disable_buttons(true);
  doing_final_anim_ = true;

  move_fragments_to_robot();

  for (auto robot_fragment : robot_fragments_)
  {
    robot_fragment->explode(robot_->getPhysicsBody()->getVelocity());
  }

  robot_->removeFromParent();
  robot_ = nullptr;

  auto const delay = DelayTime::create(5.0f);
  auto const game_over_call = CallFunc::create(CC_CALLBACK_0(game_scene::game_over, this, false));
  auto const delay_call = Sequence::create(delay, game_over_call, nullptr);

  runAction(delay_call);
}

void game_scene::game_over(const bool win)
{
  do
  {
    pause();
    game_ui_->disable_buttons(true);

    if (win)
    {
      get_audio_helper()->play_effect("sounds/victory.mp3");
      game_ui_->display_message("Level Completed", level_name_,
                                CC_CALLBACK_0(game_scene::reload, this), calculate_stars());
    }
    else
    {
      get_audio_helper()->play_effect("sounds/fail.mp3");
      game_ui_->display_message("Game Over", "\n\n\n\n\nOops, we are going to\nneed a new robot.",
                                CC_CALLBACK_0(game_scene::reload, this));
    }
  }
  while (false);
}

void game_scene::delay_start()
{
  pause();
  game_ui_->disable_buttons(true);

  const auto start = CallFunc::create(CC_CALLBACK_0(game_scene::start, this));
  const auto delay = DelayTime::create(4.6f);

  const auto delay_start_sequence = Sequence::create(delay, start, nullptr);

  const auto count_3 = CallFuncN::create(CC_CALLBACK_1(game_scene::set_countdown_number_in_ui, this, 3));
  const auto count_2 = CallFuncN::create(CC_CALLBACK_1(game_scene::set_countdown_number_in_ui, this, 2));
  const auto count_1 = CallFuncN::create(CC_CALLBACK_1(game_scene::set_countdown_number_in_ui, this, 1));
  const auto count_0 = CallFuncN::create(CC_CALLBACK_1(game_scene::set_countdown_number_in_ui, this, 0));
  const auto count_go = CallFuncN::create(CC_CALLBACK_1(game_scene::set_countdown_number_in_ui, this, -1));

  const auto count_sequence = Sequence::create(count_3, DelayTime::create(1.f),
                                               count_2, DelayTime::create(1.f),
                                               count_1, DelayTime::create(1.f),
                                               count_0, DelayTime::create(1.f),
                                               count_go,
                                               nullptr);

  get_audio_helper()->play_effect("sounds/countdown.mp3");

  runAction(delay_start_sequence);
  runAction(count_sequence);
}

void game_scene::set_countdown_number_in_ui(Ref* sender, const int value) const
{
  game_ui_->update_countdown(value);
}

void game_scene::start()
{
  get_audio_helper()->play_music("sounds/music.mp3", 0.30f);

  resume();
  game_ui_->disable_buttons(false);
}

void game_scene::close()
{
  closing_ = true;
  pause();
  application_->close();
}

void game_scene::pause()
{
  paused_ = true;
  base_class::pause();

  if (closing_)
  {
    return;
  }

  game_ui_->change_pause_button();

  getPhysicsWorld()->setAutoStep(false);

  if (robot_ != nullptr)
  {
    robot_->pause();
  }

  for (const auto& game_object : game_objects_)
  {
    game_object.second->pause();
  }

  get_audio_helper()->pause_music();

  game_ui_->get_virtual_joy_stick()->pause();

  if (doing_final_anim_)
  {
    for (auto robot_fragment : robot_fragments_)
    {
      robot_fragment->pause();
    }
  }
}

void game_scene::resume()
{
  base_class::resume();

  getPhysicsWorld()->setAutoStep(true);

  if (robot_ != nullptr)
  {
    robot_->resume();
  }

  for (const auto& game_object : game_objects_)
  {
    game_object.second->resume();
  }

  get_audio_helper()->resume_music();

  game_ui_->get_virtual_joy_stick()->resume();

  paused_ = false;
}

void game_scene::toggle_pause()
{
  if (paused_)
  {
    resume();
  }
  else
  {
    pause();
  }
}

void game_scene::reload()
{
  pause();

  game_ui_->disable_buttons(true);

  auto app = dynamic_cast<laser_and_bots_app*>(application_);
  app->to_game();
}

void game_scene::onEnter()
{
  base_class::onEnter();

  if (doing_delay_start_)
  {
    delay_start();

    doing_delay_start_ = false;
  }
}

void game_scene::update_ui_position(const Vec2& final_pos) const
{
  const auto ui_pos = Vec2(final_pos.x - (screen_size_.width / 2), final_pos.y - (screen_size_.height / 2));
  game_ui_->setPosition(ui_pos);
}

void game_scene::camera_follow_robot(const Vec2& robot_position, const float delta)
{
  // move the camera to the clamped position
  const auto final_pos = robot_position.getClampPoint(min_camera_pos_, max_camera_pos_);
  if (final_pos != last_camera_position_)
  {
    getDefaultCamera()->setPosition(final_pos);
    getDefaultCamera()->update(delta);

    update_ui_position(final_pos);

    last_camera_position_ = final_pos;
  }
}

void game_scene::switch_activate_door(door_object* door)
{
  if (door->is_off())
  {
    door->on();
  }
}

void game_scene::switch_activate_switch(switch_object* switch_object)
{
  if (switch_object->is_off())
  {
    switch_object->on();

    const auto target = switch_object->get_target();
    if (game_objects_.count(target) == 1)
    {
      switch_activate_target(game_objects_.at(target));
    }
  }
}

void game_scene::switch_activate_target(game_object* target)
{
  const auto type = target->get_type();
  if (type == "switch")
  {
    switch_activate_switch(dynamic_cast<switch_object*>(target));
  }
  else if (type == "door")
  {
    switch_activate_door(dynamic_cast<door_object*>(target));
  }
}

bool game_scene::is_switch_targeting_a_switch(switch_object* switch_object)
{
  const auto target = switch_object->get_target();
  if (game_objects_.count(target) == 1)
  {
    return game_objects_.at(target)->get_type() == "switch";
  }
  return false;
}

void game_scene::robot_touch_switch(switch_object* switch_object)
{
  if (switch_object->is_off())
  {
    if (is_switch_targeting_a_switch(switch_object))
    {
      switch_object->on();

      switch_activate_target(game_objects_.at(switch_object->get_target()));
    }
  }
}

void game_scene::robot_touch_door(door_object* door_game_object)
{
  if (door_game_object->is_on())
  {
    if (door_game_object->is_closed())
    {
      door_game_object->open();
    }
    else
    {
      game_over(true);
    }
  }
}

void game_scene::robot_touch_harm_object_start(harm_object* const harm_object) const
{
  robot_->start_periodic_damage(harm_object->get_damage());
}

void game_scene::robot_touch_harm_object_end(harm_object* harm_object) const
{
  robot_->stop_periodic_damage(harm_object->get_damage());
}

void game_scene::robot_touch_object_start(const PhysicsContact& contact)
{
  const auto door = get_object_from_contact<door_object>(contact, categories::door);
  if (door != nullptr)
  {
    robot_touch_door(door);
  }
  else
  {
    const auto switch_game_object = get_object_from_contact<switch_object>(contact, categories::switches);
    if (switch_game_object != nullptr)
    {
      robot_touch_switch(switch_game_object);
    }
    else
    {
      const auto harm_game_object = get_object_from_contact<harm_object>(contact, categories::harm);
      if (harm_game_object != nullptr)
      {
        robot_touch_harm_object_start(harm_game_object);
      }
    }
  }
}

void game_scene::robot_touch_object_end(const PhysicsContact& contact) const
{
  const auto harm_game_object = get_object_from_contact<harm_object>(contact, categories::harm);
  if (harm_game_object != nullptr)
  {
    robot_touch_harm_object_end(harm_game_object);
  }
}

void game_scene::feet_touch_object_start(const PhysicsContact& contact) const
{
  const auto block_game_object = get_object_from_contact<game_object>(contact, categories::walk_on);
  if (block_game_object != nullptr)
  {
    robot_->feet_touch_walk_object_start();
  }
  else
  {
    const auto map = get_object_from_contact<experimental::TMXTiledMap>(contact, categories::world);
    if (map != nullptr)
    {
      robot_->feet_touch_walk_object_start();
    }
  }
}

void game_scene::feet_touch_object_end(const PhysicsContact& contact) const
{
  const auto block_game_object = get_object_from_contact<game_object>(contact, categories::walk_on);
  if (block_game_object != nullptr)
  {
    robot_->feet_touch_walk_object_end();
  }
  else
  {
    const auto map = get_object_from_contact<experimental::TMXTiledMap>(contact, categories::world);
    if (map != nullptr)
    {
      robot_->feet_touch_walk_object_end();
    }
  }
}


bool game_scene::on_contact_begin(PhysicsContact& contact)
{
  if (do_we_need_game_updates())
  {
    const auto robot = get_object_from_contact<robot_object>(contact, categories::robot);
    if (robot != nullptr)
    {
      robot_touch_object_start(contact);
    }
    else
    {
      const auto feet_node = get_object_from_contact<robot_object>(contact, categories::feet);
      if (feet_node != nullptr)
      {
        feet_touch_object_start(contact);
      }
    }
  }

  return true;
}

void game_scene::on_contact_separate(PhysicsContact& contact) const
{
  if (do_we_need_game_updates())
  {
    const auto robot = get_object_from_contact<robot_object>(contact, categories::robot);
    if (robot != nullptr)
    {
      robot_touch_object_end(contact);
    }
    else
    {
      const auto feet_node = get_object_from_contact<robot_object>(contact, categories::feet);
      if (feet_node != nullptr)
      {
        feet_touch_object_end(contact);
      }
    }
  }
}


template <class Type>
Type* game_scene::get_object_from_contact(const PhysicsContact& contact, const categories category)
{
  Type* object = nullptr;
  const auto short_category = static_cast<unsigned short>(category);
  if (contact.getShapeA()->getCategoryBitmask() & short_category)
  {
    object = dynamic_cast<Type*>(contact.getShapeA()->getBody()->getNode());
    CCASSERT(object != nullptr, "contact dynamic_cast fail");
  }
  else if (contact.getShapeB()->getCategoryBitmask() & short_category)
  {
    object = dynamic_cast<Type*>(contact.getShapeB()->getBody()->getNode());
    CCASSERT(object != nullptr, "contact dynamic_cast fail");
  }

  return object;
}
