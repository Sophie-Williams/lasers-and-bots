# Lasers and Bots

A simple game with lasers and bots

[![License: Apache2](https://img.shields.io/badge/license-Apache%202-blue.svg)](/LICENSE)

## info

This is an experimental game

# requirements

- C++ compiler for your platform (clang, gcc, etc.)
- C++ IDE (XCode, Visual Studio Express, etc.)
- python

## building

Init submodules

```bash
$ git submodule update --init
```

Update cocos2d-x files

```bash
$ cd cocos2d
$ git submodule update --init
$ python download-deps.py
```

Now you could open the project folder for the platform you like to use:

- proj.ios_mac
- proj.win32
- proj.android

## resources

- Game art : [Game Art 2D](https://www.gameart2d.com/)
- Map Editor : [Tiled](https://www.mapeditor.org/)
- Game engine : [Cocox2d-x](https://cocos2d-x.org/)
- JSON Parse : [Rapid JSON](http://rapidjson.org/)
