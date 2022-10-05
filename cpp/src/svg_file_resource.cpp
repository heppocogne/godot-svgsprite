#include "svg_file_resource.h"

#include <File.hpp>
#include <Ref.hpp>
#include <OS.hpp>

using namespace godot;


void SVGFile::_register_methods()
{
    register_method("_init",&SVGFile::_init);
    register_property<SVGFile, String>("path", &SVGFile::_set_path, &SVGFile::get_path, "", 
            GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_FILE, "*.svg");
    //register_method("_set_path",&SVGFile::_set_path);
    register_method("get_path",&SVGFile::get_path);
    //register_method("_set_data",&SVGFile::_set_data);
    register_method("get_data",&SVGFile::get_data);
}


void SVGFile::_init()
{
    path="";
    //compressed=true;
    data="";
}


void SVGFile::_set_path(String p_path)
{
    path=p_path;

    // svg files cannot be opened in standalone mode
    if(godot::OS::get_singleton()->has_feature("standalone"))
        return;

    Ref<File> ref_f=File::_new();
    if(ref_f->file_exists(p_path) && ref_f->open(p_path,File::READ)==godot::Error::OK)
    {
        data=ref_f->get_as_text();
        PoolByteArray pba=ref_f->get_buffer(ref_f->get_len());
        svg_doc=lunasvg::Document::loadFromData(reinterpret_cast<const char*>(pba.read().ptr()));
        ref_f->close();
    }else
    {
        Godot::print_error(String("cannot open file (error code=")+Variant((int)ref_f->get_error())+String("):")+path,__func__,__FILE__,__LINE__);
        if(ref_f.is_valid())
            ref_f->close();
        data="";
        svg_doc=nullptr;
    }
    
    if(!svg_doc)   // invalid image
        Godot::print_error("invalid svg file:"+path,__func__,__FILE__,__LINE__);
}


void SVGFile::_set_data(String p_data)
{
    data=p_data;
}


String SVGFile::get_data()
{
    return data;
}
