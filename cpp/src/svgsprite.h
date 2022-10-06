#ifndef SVGSPRITE_H
    #define SVGSPRITE

    #include <Godot.hpp>
    #include <Ref.hpp>
    #include <Node2D.hpp>
    #include <ImageTexture.hpp>
    
    #include "svg_file_resource.h"

    #include <memory>
    #include <lunasvg.h>

    namespace godot
    {
        class SVGSprite :public Node2D
        {
            GODOT_CLASS(SVGSprite,Node2D)

            // rotation and scaling
            Transform2D _transform;
            PoolByteArray _bitmap_byte_array;
            Ref<ImageTexture> _ref_texture;

            bool _cache_dirty;
        public:
            Ref<SVGFile> ref_svg_file;
            bool centered;
            Vector2 offset;
            int texture_flags;

            static void _register_methods();

            SVGSprite();
            ~SVGSprite(){}

            void _init();
            void _ready();
            void _draw();
            void _notification(int what);

            void set_ref_svg_file(Ref<SVGFile> p_svg_file);
            Ref<SVGFile> get_ref_svg_file()const{return ref_svg_file;}

            void set_centered(bool p_centered);
            bool get_centered()const{return centered;}
            void set_offset(Vector2 p_offset);
            Vector2 get_offset()const{return offset;}
            void set_texture_flags(int p_texture_flags);
            int get_texture_flags(void)const{return texture_flags;}
        };
    }

#endif