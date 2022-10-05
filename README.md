# godot-svgsprite
GDNative addon for dynamic svg rendering
![](screenshot.png)

## How to use
### Build
1. Clone this repository
   ```
   git clone https://github.com/heppocogne/godot-svgsprite --recursive
   cd godot-svgsprite
   ```
2. Build godot-cpp library  
   For more information, please check the [official documentation](https://docs.godotengine.org/en/stable/tutorials/scripting/gdnative/gdnative_cpp_example.html).
   ```
   cd cpp/godot-cpp
   scons platform=<platform> generate_bindings=yes -j4
   ```
   (wait a minute)
    
   ```
   cd ../../
   ```
3. Build godot-svgsprite library
   ```
   mkdir bin
   scons platform=<platform> target_name=libgodot-svgsprite
   ```
### Enjoy the demo project!
### Install addon
1. Copy "addons" and "bin" folders to your project folder  
   ![image](https://user-images.githubusercontent.com/83043568/192418076-98a9eaab-ac55-4b71-85c3-f62afe3929e5.png)
2. Project > ProjectSetting > Plugins: Activate SVGSprite plugin  
   ![image](https://user-images.githubusercontent.com/83043568/192416729-c99612d1-476c-4471-b414-46b34a64544d.png)