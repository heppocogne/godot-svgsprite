#include "svgsprite.h"

#define TYPED_METHOD_BIND
#define NOMINMAX

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/transform2d.hpp>
#include <godot_cpp/classes/project_settings.hpp>

#ifndef EDITOR_FEATURE_DISABLED
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#endif

#include <algorithm>
#include <cstring>
#include "rawsvg_loader.h"

using namespace godot;

void SVGSprite::_bind_methods()
{
    // ClassDB::bind_method(D_METHOD("_init"), &SVGSprite::_init);
    // ClassDB::bind_method(D_METHOD("_ready"), &SVGSprite::_ready);
    // ClassDB::bind_method(D_METHOD("_draw"), &SVGSprite::_draw);
    ClassDB::bind_method(D_METHOD("_notification"), &SVGSprite::_notification);

    ClassDB::bind_method(D_METHOD("set_svg_file", "svg_file"), &SVGSprite::set_svg_file);
    ClassDB::bind_method(D_METHOD("get_svg_file"), &SVGSprite::get_svg_file);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "svg_file", godot::PROPERTY_HINT_FILE, "*.svg"), "set_svg_file", "get_svg_file");

    ClassDB::bind_method(D_METHOD("set_centered", "centered"), &SVGSprite::set_centered);
    ClassDB::bind_method(D_METHOD("get_centered"), &SVGSprite::get_centered);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "centered"), "set_centered", "get_centered");

    ClassDB::bind_method(D_METHOD("set_offset", "offset"), &SVGSprite::set_offset);
    ClassDB::bind_method(D_METHOD("get_offset"), &SVGSprite::get_offset);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset"), "set_offset", "get_offset");

    ClassDB::bind_method(D_METHOD("set_flip_h", "flip_h"), &SVGSprite::set_flip_h);
    ClassDB::bind_method(D_METHOD("get_flip_h"), &SVGSprite::get_flip_h);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_h"), "set_flip_h", "get_flip_h");

    ClassDB::bind_method(D_METHOD("set_flip_v", "flip_v"), &SVGSprite::set_flip_v);
    ClassDB::bind_method(D_METHOD("get_flip_v"), &SVGSprite::get_flip_v);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_v"), "set_flip_v", "get_flip_v");

    ClassDB::bind_method(D_METHOD("get_size"), &SVGSprite::get_size);

    ClassDB::bind_method(D_METHOD("get_current_texture"), &SVGSprite::get_current_texture);
}

SVGSprite::SVGSprite() : _cache_dirty(true),
                         _svg_doc(nullptr),
                         _ref_texture(memnew(ImageTexture)),
#ifndef EDITOR_FEATURE_DISABLED
                         _ref_prerasterized(nullptr),
#endif
                         svg_file(""),
                         centered(true),
                         offset(Vector2()),
                         flip_h(false),
                         flip_v(false)
{
}

SVGSprite::~SVGSprite()
{
}

void SVGSprite::_ready()
{
    set_notify_transform(true);
}

void SVGSprite::_draw()
{
    if (!_svg_doc) // invalid image
        return;

#ifndef EDITOR_FEATURE_DISABLED
    // use pre-rasterized texture
    if (Engine::get_singleton()->is_editor_hint())
    {
        if (_ref_prerasterized.is_valid())
        {
            Rect2 rect(-offset, get_size());
            if (centered)
                rect.position -= Vector2(_svg_doc->width(), _svg_doc->height()) / 2;
            if (flip_h)
                rect.size.x *= -1;
            if (flip_v)
                rect.size.y *= -1;
            draw_texture_rect(_ref_prerasterized, rect, false);
        }
        return;
    }
#endif

    Transform2D tf2d = get_global_transform();
    const auto go = tf2d.get_origin();
    tf2d.set_origin(Vector2(0.0, 0.0));

    auto center = Vector2(_svg_doc->width(), _svg_doc->height()) / 2.0;
    auto lb = Vector2(0.0, _svg_doc->height());
    lb = tf2d.xform(lb);
    auto rt = Vector2(_svg_doc->width(), 0.0);
    rt = tf2d.xform(rt);
    auto rb = Vector2(_svg_doc->width(), _svg_doc->height());
    rb = tf2d.xform(rb);

    auto w = std::max(abs(lb.x - rt.x), abs(rb.x));
    auto h = std::max(abs(lb.y - rt.y), abs(rb.y));

    if (_cache_dirty)
    {
        const int w_int = ceil(w);
        const int h_int = ceil(h);

        _bitmap_byte_array.resize(4 * w_int * h_int);
        unsigned char *const bitmap_write_ptr = _bitmap_byte_array.ptrw();
        memset(bitmap_write_ptr, 0, 4 * w_int * h_int);

        auto gs = tf2d.get_scale();
        if (flip_h)
            gs.x *= -1;
        if (flip_v)
            gs.y *= -1;

        lunasvg::Bitmap bitmap(bitmap_write_ptr, w_int, h_int, w_int * 4);
        auto mat = lunasvg::Matrix::scaled(gs.x, gs.y) * lunasvg::Matrix::translated((w - gs.x * _svg_doc->width()) / 2, (h - gs.y * _svg_doc->height()) / 2);
        mat *= lunasvg::Matrix::rotated(tf2d.get_rotation() / Math_PI * 180, w / 2, h / 2);
        _svg_doc->render(bitmap, mat);
        // Bitmap:argb -> Image:rgba
        bitmap.convertToRGBA();

        Ref<Image> ref_image = memnew(Image);
        ref_image = Image::create_from_data(w_int, h_int, false, Image::FORMAT_RGBA8, _bitmap_byte_array);
        if (_ref_texture->get_size() == ref_image->get_size())
            _ref_texture->update(ref_image);
        else
            _ref_texture = ImageTexture::create_from_image(ref_image);

        _cache_dirty = false;
    }

    auto offset_raw = offset;
    if (centered)
    {
        offset_raw += Vector2(_svg_doc->width(), _svg_doc->height()) / 2;
    }
    auto lt_in_texture = tf2d.xform(-get_size() / 2 + offset_raw) + Vector2(w, h) / 2;
    draw_set_transform_matrix(tf2d.affine_inverse());
    draw_texture(_ref_texture, -lt_in_texture);
}

void SVGSprite::_notification(int what)
{
#ifndef EDITOR_FEATURE_DISABLED
    if (Engine::get_singleton()->is_editor_hint())
        return;
#endif

    if (what == NOTIFICATION_TRANSFORM_CHANGED)
    {
        auto gtf = get_global_transform();
        if (_transform[0] == gtf[0] && _transform[1] == gtf[1])
            return;

        _cache_dirty = true;
        queue_redraw();
    }
}

void SVGSprite::set_svg_file(String p_svg_file)
{
    svg_file = p_svg_file;
#ifndef EDITOR_FEATURE_DISABLED

    if (svg_file == "")
        _ref_prerasterized = Ref<ImageTexture>(memnew(ImageTexture));
    else
        _ref_prerasterized = ResourceLoader::get_singleton()->load(svg_file);
#endif

    if ((bool)ProjectSettings::get_singleton()->get_setting("svgsprite/compress", false) == false)
        _svg_doc = RawSvgLoader::get_singleton()->load(get_rawsvg_path(p_svg_file)).get();
    else
        _svg_doc = RawSvgLoader::get_singleton()->load(get_rawsvgz_path(p_svg_file)).get();
    if (!_svg_doc) // external file
        _svg_doc = RawSvgLoader::get_singleton()->load(p_svg_file).get();
    _cache_dirty = true;

    queue_redraw();
}

void SVGSprite::set_centered(bool p_centered)
{
    centered = p_centered;
    queue_redraw();
}

void SVGSprite::set_offset(Vector2 p_offset)
{
    offset = p_offset;
    queue_redraw();
}

Vector2 SVGSprite::get_size() const
{
    if (_svg_doc)
        return Vector2(_svg_doc->width(), _svg_doc->height());
    else
        return Vector2();
}

void SVGSprite::set_flip_h(bool p_flip_h)
{
    flip_h = p_flip_h;
    queue_redraw();
}

void SVGSprite::set_flip_v(bool p_flip_v)
{
    flip_v = p_flip_v;
    queue_redraw();
}

String SVGSprite::get_rawsvg_path(String path)
{
    return String(_rawsvg_root) + "/" + path.get_file() + "-" + path.get_basename().sha256_text() + ".rawsvg";
}

String SVGSprite::get_rawsvgz_path(String path)
{
    return String(_rawsvg_root) + "/" + path.get_file() + "-" + path.get_basename().sha256_text() + ".rawsvgz";
}
