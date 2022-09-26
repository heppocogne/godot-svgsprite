#ifndef SVGSPRITE_H
    #define SVGSPRITE

    #include <Godot.hpp>
    #include <Ref.hpp>
    #include <Node2D.hpp>
    #include <ImageTexture.hpp>

    #include <memory>
    #include <lunasvg.h>

    namespace godot
    {
        class SVGSprite :public Node2D
        {
            GODOT_CLASS(SVGSprite,Node2D)

            std::unique_ptr<lunasvg::Document> _svg_doc;
            // rotation and scaling
            Transform2D _transform;
            PoolByteArray _bitmap_byte_array;
            Ref<ImageTexture> _ref_texture;

            bool _cache_dirty;

        public:
            String svg_file;
            bool centered;
            Vector2 offset;

            static void _register_methods();

            SVGSprite();
            ~SVGSprite();

            void _init();
            void _ready();
            void _draw();
            void _notification(int what);

            void set_svg_file(String p_svg_file);
            String get_svg_file()const{return svg_file;}

            void set_centered(bool p_centered);
            bool get_centered()const{return centered;}
            void set_offset(Vector2 p_offset);
            Vector2 get_offset()const{return offset;}
        };
    }

#endif