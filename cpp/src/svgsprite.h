#ifndef SVGSPRITE_H
#define SVGSPRITE_H

#include <Godot.hpp>
#include <Ref.hpp>
#include <Node2D.hpp>
#include <ImageTexture.hpp>

#include <memory>
#include <lunasvg.h>

namespace godot
{
    class SVGSprite : public Node2D
    {
        GODOT_CLASS(SVGSprite, Node2D)

        lunasvg::Document *_svg_doc;
        // rotation and scaling
        Transform2D _transform;
        PoolByteArray _bitmap_byte_array;
        Ref<ImageTexture> _ref_texture;
#ifndef EDITOR_FEATURE_DISABLED
        Ref<Texture> _ref_prerasterized;
#endif
        bool _cache_dirty;
        constexpr static const char *_rawsvg_root = "res://_rawsvg";

    public:
        String svg_file;
        bool centered;
        Vector2 offset;
        int texture_flags;
        bool flip_h;
        bool flip_v;

        static void _register_methods();
        static String get_rawsvg_path(String path);
        static String get_rawsvgz_path(String path);

        SVGSprite();
        ~SVGSprite();

        void _init();
        void _ready();
        void _draw();
        void _notification(int what);

        void set_svg_file(String p_svg_file);
        String get_svg_file() const { return svg_file; }

        void set_centered(bool p_centered);
        bool get_centered() const { return centered; }
        void set_offset(Vector2 p_offset);
        Vector2 get_offset() const { return offset; }
        void set_flip_h(bool p_flip_h);
        bool get_flip_h() const { return flip_h; }
        void set_flip_v(bool p_flip_v);
        bool get_flip_v() const { return flip_v; }
        Ref<Texture> get_current_texture() const { return _ref_texture; }
        void set_texture_flags(int p_texture_flags);
        int get_texture_flags(void) const { return texture_flags; }

        Vector2 get_size() const;
    };
}

#endif