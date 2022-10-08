#include "svgsprite.h"

#include <File.hpp>
#include <Image.hpp>
#include <Vector2.hpp>
#include <Transform2D.hpp>
//#include <ProjectSettings.hpp>
#include <Engine.hpp>
#include <ResourceLoader.hpp>

#include <algorithm>
#include <cstring>

using namespace godot;

void SVGSprite::_register_methods()
{
    register_method("_init", &SVGSprite::_init);
    register_method("_ready", &SVGSprite::_ready);
    register_method("_draw", &SVGSprite::_draw);
    register_method("_notification", &SVGSprite::_notification);
    register_property<SVGSprite,String>("svg_file", &SVGSprite::set_svg_file, &SVGSprite::get_svg_file, "", 
            GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_FILE, "*.svg");
    register_property<SVGSprite, bool>("centered", &SVGSprite::set_centered, &SVGSprite::get_centered, true);
    register_method("set_centered",&SVGSprite::set_centered);
    register_method("get_centered",&SVGSprite::get_centered);
    register_property<SVGSprite, Vector2>("offset", &SVGSprite::set_offset, &SVGSprite::get_offset, Vector2::ZERO);
    register_method("set_offset",&SVGSprite::set_offset);
    register_method("get_offset",&SVGSprite::get_offset);
    register_property<SVGSprite, int>("texture_flags", &SVGSprite::set_texture_flags, &SVGSprite::get_texture_flags, 7, 
            GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_FLAGS, "Mipmaps,Repeat,Filter,Anisotropic Filter,Convert to Linear,Mirrored Repeat,Video Surface");
    register_method("set_texture_flags",&SVGSprite::set_texture_flags);
    register_method("get_texture_flags",&SVGSprite::get_texture_flags);
}


SVGSprite::SVGSprite():
    _cache_dirty(true),
#ifndef EDITOR_FEATURE_DISABLED
    _ref_texture(ImageTexture::_new()),
    _ref_prerasterized(nullptr)
#else
    _ref_texture(ImageTexture::_new())
#endif
{

}


SVGSprite::~SVGSprite()
{

}


void SVGSprite::_init()
{
    // initialize properties here
    svg_file="";
    centered=true;
    offset=Vector2::ZERO;
    texture_flags=7;
}


void SVGSprite::_ready()
{
    set_notify_transform(true);
}


void SVGSprite::_draw()
{
#ifndef EDITOR_FEATURE_DISABLED
    // use pre-rasterized texture
    if(Engine::get_singleton()->is_editor_hint())
    {
        if(_ref_prerasterized.is_valid())
        {
            if(centered)
                draw_texture(_ref_prerasterized,-(offset+Vector2(_svg_doc->width(), _svg_doc->height())/2));
            else
                draw_texture(_ref_prerasterized,-offset);
        }
        return;
    }
#endif

    if(!_svg_doc)   // invalid image
        return;

    Transform2D tf2d=get_global_transform();
    const auto go=tf2d.get_origin();
    tf2d.set_origin(Vector2(0.0,0.0));

    const auto gs=tf2d.get_scale();

    auto center=Vector2(_svg_doc->width(),_svg_doc->height())/2.0;
    auto lb=Vector2(0.0,_svg_doc->height());
    lb=tf2d.xform(lb);
    auto rt=Vector2(_svg_doc->width(),0.0);
    rt=tf2d.xform(rt);
    auto rb=Vector2(_svg_doc->width(),_svg_doc->height());
    rb=tf2d.xform(rb);

    auto w=std::max(abs(lb.x-rt.x), abs(rb.x));
    auto h=std::max(abs(lb.y-rt.y), abs(rb.y));

    if(_cache_dirty){
        const int w_int=ceil(w);
        const int h_int=ceil(h);

        _bitmap_byte_array.resize(4*w_int*h_int);
        unsigned char* const bitmap_write_ptr=_bitmap_byte_array.write().ptr();
        memset(bitmap_write_ptr,0,4*w_int*h_int);
        
        lunasvg::Bitmap bitmap(bitmap_write_ptr, w_int, h_int, w_int*4);
        _svg_doc->render(bitmap, lunasvg::Matrix::scaled(gs.x, gs.y)
                                *lunasvg::Matrix::translated((w-gs.x*_svg_doc->width())/2, (h-gs.y*_svg_doc->height())/2)
                                *lunasvg::Matrix::rotated(tf2d.get_rotation()/Math_PI*180, w/2, h/2));
        // Bitmap:argb -> Image:rgba
        bitmap.convertToRGBA();

        Ref<Image> ref_image=Image::_new();
        ref_image->create_from_data(w_int, h_int, false, Image::FORMAT_RGBA8, _bitmap_byte_array);
        if(_ref_texture->get_size()==ref_image->get_size())
            _ref_texture->set_data(ref_image);
        else
        {
            const auto flags=_ref_texture->get_flags();
            _ref_texture->create_from_image(ref_image);
            _ref_texture->set_flags(flags);
        }
        
        _cache_dirty=false;
    }

    auto offset_raw=offset;
    if(centered){
        offset_raw+=Vector2(_svg_doc->width(), _svg_doc->height())/2;
    }
    auto lt_in_texture=tf2d.xform(-Vector2(_svg_doc->width(), _svg_doc->height())/2 + offset_raw)+Vector2(w,h)/2;
    draw_set_transform_matrix(tf2d.affine_inverse());
    draw_texture(_ref_texture, -lt_in_texture);
}


void SVGSprite::_notification(int what)
{
#ifndef EDITOR_FEATURE_DISABLED
    if(Engine::get_singleton()->is_editor_hint())
        return;
#endif

    if(what==NOTIFICATION_TRANSFORM_CHANGED){
        auto gtf=get_global_transform();
        if(_transform[0]==gtf[0] && _transform[1]==gtf[1])
            return;
        
        _cache_dirty=true;
        update();
    }
}


void SVGSprite::set_svg_file(String p_svg_file)
{
    svg_file=p_svg_file;

#ifndef EDITOR_FEATURE_DISABLED
    _ref_prerasterized=ResourceLoader::get_singleton()->load(svg_file);
#endif

    Ref<File> ref_f=File::_new();
    const String rawsvg_file=get_rawsvg_path(svg_file);
    godot::Error err=godot::Error::ERR_UNCONFIGURED;
    if(ref_f->file_exists(svg_file))
    {
        godot::Error err=ref_f->open(rawsvg_file,File::READ);
        if(err==godot::Error::OK)
        {
            PoolByteArray pva=ref_f->get_buffer(ref_f->get_len());
            _svg_doc=lunasvg::Document::loadFromData(reinterpret_cast<const char*>(pva.read().ptr()));
            if(!_svg_doc)
                Godot::print_error("invalid svg file:"+svg_file,__func__,__FILE__,__LINE__);
            else
            {
                _cache_dirty=true;
                update();
                return;
            }
        }else
        {
            _svg_doc=nullptr;
            Godot::print_error(String("cannot open file (error code=")+Variant((int)err)+String("):")+svg_file,__func__,__FILE__,__LINE__);
        }
    }
    if(ref_f->file_exists(rawsvg_file))
    {
        godot::Error err=ref_f->open(rawsvg_file,File::READ);
        if(err==godot::Error::OK)
        {
            PoolByteArray pva=ref_f->get_buffer(ref_f->get_len());
            _svg_doc=lunasvg::Document::loadFromData(reinterpret_cast<const char*>(pva.read().ptr()));
            if(!_svg_doc)
                Godot::print_error("invalid svg file:"+rawsvg_file,__func__,__FILE__,__LINE__);
        }else
        {
            _svg_doc=nullptr;
            Godot::print_error(String("cannot open file (error code=")+Variant((int)err)+String("):")+rawsvg_file,__func__,__FILE__,__LINE__);
        }
    }else
        Godot::print_error(String("neither ")+svg_file+String(" nor ")+rawsvg_file+String(" exists"),__func__,__FILE__,__LINE__);
    _cache_dirty=true;
    update();
}


void SVGSprite::set_centered(bool p_centered)
{
    centered=p_centered;
    update();
}


void SVGSprite::set_offset(Vector2 p_offset)
{
    offset=p_offset;
    update();
}


void SVGSprite::set_texture_flags(int p_texture_flags)
{
    texture_flags=p_texture_flags;
    _ref_texture->set_flags(texture_flags);
    update();
}


String SVGSprite::get_rawsvg_path(String path)
{
    return String(_rawsvg_root).plus_file(path.get_file()+"-"+path.get_basename().sha256_text()+".rawsvg");
}
