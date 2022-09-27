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
1. Copy "addons" folder to your project folder  
   ![image](https://user-images.githubusercontent.com/83043568/192416833-806910aa-27a8-4183-b42f-4ce761620023.png)
2. Project > ProjectSetting > Plugins: Activate SVGSprite plugin  
   ![image](https://user-images.githubusercontent.com/83043568/192416729-c99612d1-476c-4471-b414-46b34a64544d.png)
