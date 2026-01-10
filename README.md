<h1 align="center" style="text-shadow: 2px 2px 5px red;">2D Minecraft with Custom Game Engine</h2>

<div align="center">
    <img src="https://hc-cdn.hel1.your-objectstorage.com/s/v3/69194621ebc5e58d_image.png" width="auto" height="auto" alt="Demo pic of the game">
    <img src="https://hc-cdn.hel1.your-objectstorage.com/s/v3/f2ea563eb7075359_image.png" width="auto" height="auto" alt="Demo pic of the game">
    <img src="https://hc-cdn.hel1.your-objectstorage.com/s/v3/52ce88733043c0f2_image.png" width="auto" height="auto" alt="Demo pic of the game">
    <img src="https://hc-cdn.hel1.your-objectstorage.com/s/v3/5fe3dc6c87a67321_image.png" width="auto" height="auto" alt="Demo pic of the game">
</div>

<h3 align="center"><a href="https://cyao.dev/mc">Try out web version now!</a></h3>

<h2 align="center">Controls</h2>

- WASD for movement
- E to open Inventory
- ESC to close
- Hold left click to break block
- Right click to interact/place block
- Everything else like Minecraft

<h2 align="center">Features</h2>

- **Cross-Platorm** : **MacOS**, **Linux**, **Windows**, **Web**, **Android** and **IOS** all supported!
- **Performant** : Uses C++, the game runs smoothly even on browsers. (**750**+ FPS on a **11 year old** laptop!)
- **Multilingual** : **Unicode** and Locale support build in.
- **Modern** : Uses **Modern** C++23 features and follows best practices. SDL3 used!
- **Extendable** : The Game Engine uses the ECS arcitecture. Extend easily by adding stuff to `src/registers.cpp`!
- **Debuggable** : Custom asserts and **Debug menu** helps you identify any bugs!
- **Custom Saves** : Easily save **any** data on any platform you want in a json file, the game engine will manage it for you!
- **Crafting UI**: UI for any type of menu you want!
- **Audio** : Easily integrate audio into the game
- **Minimal dependencies**: Only SDL3 Needed!

(I swear this isn't written by AI)

Minecraft Features:

- Walking
- Breaking
- Inventory
- Placing
- Crafting
- Smelting
- Break levels
- Loot table
- Audio
- Animations

<h2 align="center">Inner workings</h2>

This game engine uses the Entity-Component-System architecture, implemented using `Scenes` and `sparse_sets`.

Entities are just UUIDs.

Components are just plain old structs.

The systems manage all the logic and changes the components.

<h2 align="center">Building</h2>

All builds can be packaged by compiling the `package` target (Using cmake CPack). If you are using `clang`, you need libc++

Dependencies:

- [SDL3](https://www.libsdl.org/)
- [Git LFS](https://git-lfs.com/)

Please install [git lfs](https://git-lfs.com/) before cloning the repo or the assets won't download correctly.

### Flatpack
Flatpack is the easiest way to compile on linux
```bash 
$ git clone --depth 1 https://github.com/cheyao/2d-minecraft.git
$ cd 2d-minecraft 
$ flatpak-builder --user --install-deps-from=flathub --force-clean --repo=repo --install build com.cyao.flat-minecraft.yml
$ flatpak run com.cyao.flat-minecraft
```

### MacOS and Linux

If your package manager has SDL3 install it there:
```
$ brew install sdl3   # MacOS
$ sudo pacman -S SDL3 # Arch Linux
```

Or compile SDL3:
```bash
$ git clone --depth 1 https://github.com/libsdl-org/SDL.git
$ cd SDL 
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
$ sudo cmake --install .
```

Compile project:

```bash
$ git clone --depth 1 https://github.com/cheyao/2d-minecraft.git
$ cd 2d-minecraft 
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

I reccomend cross compiling with the arch mingw package

The cmake directory includes a getdll64 script to help package all the dlls.

### Android

Dependencies, java 17 is a must:
```bash
$ brew install --cask android-platform-tools android-ndk temurin@17
$ brew install cmake
# For arch
$ sudo pacman -S android-tools android-sdk-cmdline-tools-latest android-sdk-build-tools android-sdk-platform-tools android-platform android-ndk
```

Add these to your `.zshrc` (Change it to the appropriate folders on linux):
```bash
$ export ANDROID_NDK_HOME="/usr/local/share/android-ndk"
$ export JAVA_HOME=`/usr/libexec/java_home -v 17` # Yes, you __must__ use java 17, blame android
$ export ANDROID_HOME="/usr/local/share/android-commandlinetools/"

# For arch linux
$ export ANDROID_HOME="/opt/android-sdk"
$ export ANDROID_NDK_HOME="/opt/android-ndk"

# You might need to accept the licenses with
$ sdkmanager --licenses
```

Configure your gradlew's credentials (I'm not gonna let you use mine duh)
```
nvim .gradle/gradle.properties

RELEASE_STORE_FILE=[KEYSTORE PATH RELATIVE FROM ./android]
RELEASE_STORE_PASSWORD=[KEYSTORE PASSWORD]
RELEASE_KEY_ALIAS=[KEY NAME]
RELEASE_KEY_PASSWORD=[KEY PASSWORD]
```

Now build the project:
```bash
$ git clone --depth 1 https://github.com/cheyao/2d-minecraft.git
$ cd 2d-minecraft
$ git submodule update --init --recursive
$ mkdir build && cd build 
$ cmake -DCMAKE_BUILD_TYPE=Release \
        -DANDROID=ON \
        ..
$ cmake --build .
```
Now there is the apk in the folder

Or alternatively you can cd into the android folder, and run `./gradlew` manually. But you must run the cmake project ot correctly generate a few files.

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
$ git clone --depth 1 https://github.com/cheyao/2d-minecraft.git
$ cd 2d-minecraft
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

The linux save directory is `~/.local/share/cyao/2d-minecraft/`

Add your own blocks to `~/.local/share/cyao/opengl/`

# :3

See the Minecraft mod version of this at https://modrinth.com/mod/2d-minecraft

NOT AN OFFICIAL MINECRAFT SPINOFF. NOT APPROVED BY OR ASSOCIATED WITH MOJANG OR MICROSOFT
