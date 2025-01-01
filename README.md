<h1 align="center" style="text-shadow: 2px 2px 5px red;">Cyao Game Engine</h1>
<h2 align="center">Features</h2>

- **Cross-Platorm** 💻📱: **MacOS**, **Linux**, **Windows**, **Web**, **Android** and **IOS** all supported!
- **Performant** 🏃: Uses C++, the game runs smoothly even on browsers. (**600**+ FPS on a **11 year old** laptop!)
- **Multilingual** 🌐: **Unicode** and Locale support build in. (No more �!)
- **Modern** ✨: Uses **Modern** C++20 features and follows best practices. SDL3 used!
- **Extendable** 🧩: The Game Engine uses the **Bleeding-edge** ECS arcitecture.
- **Debuggable** 🔍: Custom asserts and **Debug menu** helps you identify any bugs 🐛!
- **Custom Saves** 📝: Easily save **any** data on any platform you want in a json file, the game engine will manage it for you!
- **2D and 3D** : Make **2D** and **3D** Together!

<h3 align="center"><a href="https://cyao.hackclub.app">Try out web version now!</a></h3>

<div align="center">
    <img src="https://cloud-l0r4ps4t3-hack-club-bot.vercel.app/0image.png" width="auto" height="auto" alt="Demo pic of the game">
</div>

<h2 align="center">Inner workings</h2>

This game engine uses the Entity-Component-System architecture, implemented using `Scenes` and `sparse_sets`.

Entities are just UUIDs.

Components are just plain old structs.

The systems manage all the logic and changes the components.

<h2 align="center">Building</h2>

Dependencies:

- SDL3
- Freetype 2

### MacOS and Linux

Install `freetype` with your package manager

Compile SDL3:
```bash
$ git clone --depth 1 https://github.com/libsdl-org/SDL.git
$ cd SDL 
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
$ sudo cmake --install .
```

Install dependencies:

```bash
# Install freetype 2
# MacOS
$ brew install freetype
# Linux
$ sudo pacman -S freetype2
```

Compile project:

```bash
$ git clone --depth 1 https://github.com/cheyao/opengl.git
$ cd opengl 
$ mkdir build && cd build 
$ cmake ..
$ cmake --build .

For Linux:
$ ./OpenGL

For MacOS:
$ mv OpenGL.app /Applications/
$ open /Applications/OpenGL.app # Or just openg the app
```
### Windows

Same, use cmake to build project

### Android

MacOS Dependencies:
```bash
$ brew install --cask android-platform-tools android-ndk temurin@17
$ brew install cmake
```

Add these to your `.zshrc` (Change it to the appropriate folders on linux):
```bash
$ export ANDROID_NDK_HOME="/usr/local/share/android-ndk"
$ export JAVA_HOME=`/usr/libexec/java_home -v 17` # Yes, you __must__ use java 17, blame android
$ export ANDROID_HOME="/usr/local/share/android-commandlinetools/"
```

Configure your gradlew's credentials (I'm not gonna let you use mine duh)
```
File: ~/.gradle/gradle.properties

RELEASE_STORE_FILE=[KEYSTORE PATH RELATIVE FROM ./android]
RELEASE_STORE_PASSWORD=[KEYSTORE PASSWORD]
RELEASE_KEY_ALIAS=[KEY NAME]
RELEASE_KEY_PASSWORD=[KEY PASSWORD]
```

Now build the project:
```bash
$ git clone --depth 1 https://github.com/cheyao/opengl.git
$ cd opengl
$ git submodule update --init --recursive
$ mkdir build && cd build 
$ cmake -DCMAKE_BUILD_TYPE=Release \
        -DANDROID=ON \
        ..
$ cmake --build .
```
Now there is the apk in the folder

## Web 
Install [emsdk](https://emscripten.org/) to `~/emsdk`

On arch just run `sudo pacman -S emscripten`

Build SDL3:

```bash
$ source ~/emsdk/emsdk_env.sh # Only on MacOS
$ git clone --depth 1 https://github.com/libsdl-org/SDL.git
$ cd SDL 
$ mkdir build && cd build
$ emcmake cmake ..
$ cmake --build .
$ cmake --install .
```

Build the project:
```bash
$ git clone --depth 1 https://github.com/cheyao/opengl.git
$ cd opengl
$ mkdir build && cd build 
$ emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
$ emcmake cmake --build .
$ python3 -m http.server
```

If you get the error `emcc: error: unable to create cache directory "{cachdir}"` run the build command with sudo

Currently the web build fetches the resources on demand

## Distribuing
There are scripts in the cmake folder:
```
cmake
├── distribute-linux.sh
├── getdll32.sh
├── getdll64.sh
├── index.html.in
├── info.plist.in
├── mac-patch-dylib.sh
└── sign-apk.sh
```

- `./cmake/distribute-linux.sh` For patching the executable then copying the dlibs into `libs` folder, should be enough to patch for linux release
- `./cmake/mac-patch-dylib.sh` Same but for mac, only works for bundled macos app
- `./cmake/index.html.in` Is the Emscripten idex
- `./cmake/sign-apk.sh` Is to sign the android apk

## Assets
The assets should be bundled automatically with android and macos, on other platforms you must distribute with the assets (that are copied into the project folder)

```
assets
├── fonts
├── models
├── shaders
└── textures
    └── ui
```

- `./assets/fonts/` Fonts
- `./assets/models/` 3D models
- `./assets/shaders/` shaders
- `./assets/textures/` Textures, the models will look here for the corresponding textures
- `./assets/textures/ui` Textures for the UI
- `./assets/strings.csv` Source loc csv file (See [here](https://en.wikipedia.org/wiki/IETF_language_tag) for language codes)
- `./assets/strings.json` Output loc file (to be generated) 

## Notes

The linux save directory is `~/.local/share/cyao/opengl/`

## TODO

- [x] In-game metrics & ms count
    - [x] ImGUI Menu
- [ ] Live edit shaders in-game
- [ ] Fix MacOS bundle process https://github.com/libsdl-org/SDL/blob/main/docs/README-macos.md
- [x] Game pause + resume things
- [x] Debug menu with ImGUI and item names
- [x] Better component and actors
- [x] UI Scaling
- [x] Multitouch
- [x] Collision resolver
- [x] Scaling
- [x] Save state
- [x] Android build
    - [x] Android controls
- [x] Debug menu 
    - [x] Collision area editor
    - [x] Signal list
    - [x] Velocity arrows
    - [x] Component and entity list
        - [ ] Real time component and entity addition
- [ ] UI
    - [ ] Embed text in buttons
    - [ ] MacOS: Link new freetype version
- [ ] Pause menu etc
    - [x] Text
    - [ ] Settings
- [x] Localization
    - [x] Fallback fonts.
- [x] Emscripten build
    - [ ] Progress bar
    - [ ] Text with contrast
- [x] TTF
    - [ ] Button auto scale + text
    - [ ] Generate texturemap
    - [ ] Signed distance field fonts https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
- [ ] Windows build
- [ ] Level map
    - [ ] Level generator
    - [ ] Thing
    - [ ] Binary json (MessagePack)
    - [x] Move scenes to `src/scenes`
- [ ] Grid map
- [ ] Add functional hot reload
- [ ] Entity name saved
- [x] Combine localemanager and textmanager
- [ ] Shadows
- [ ] Multithreading
    - [ ] Preload

- [ ] Read books (By priority)
    - [ ] [Effective Modern C++]()
    - [ ] [Game maths](https://gamemath.com/book/intro.html)
    - [ ] [Game engine architecture](https://www.amazon.com/dp/1568814135)
    - [ ] [Game programming gems](https://www.satori.org/game-programming-gems/)
    - [ ] [Real time rendering](https://www.amazon.com/dp/1568814240)
    - [ ] [Real time collision detection](https://www.amazon.com/dp/1558607323)

# :3

Almost no AI used, only AI changes are from the pull requests submitted by these prs https://github.com/cheyao/opengl/pulls?q=is%3Apr+is%3Aclosed. I accepted them since some beta tester is testing the new o3 model on my project and I was really curious of what if can acheive. But I still modified most of the code generated.

Plus I'm smarter then AI! My implementation of sparse set caching shaved out 700000ns of runtime while the ai version shaved only 400000ns!

NOT AN OFFICIAL MINECRAFT SPINOFF. NOT APPROVED BY OR ASSOCIATED WITH MOJANG OR MICROSOFT
