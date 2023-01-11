#include "svgsprite.h"
#include "rawsvg_loader.h"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o)
{
    godot::Godot::gdnative_init(o);
    godot::RawSvgLoader::on_library_init();
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o)
{
    godot::RawSvgLoader::on_library_terminate();
    godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle)
{
    godot::Godot::nativescript_init(handle);

    // register classes here (use godot::register_class<ClassName>() for ordinal classes)
    godot::register_tool_class<godot::SVGSprite>();
}
