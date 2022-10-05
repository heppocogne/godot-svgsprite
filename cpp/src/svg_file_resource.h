#ifndef SVG_FILE_RESOURCE_H
    #define SVG_FILE_RESOURCE_H

    #include <Godot.hpp>
    #include <Resource.hpp>
    #include <Marshalls.hpp>

    #include <memory>
    #include <lunasvg.h>

    namespace godot
    {
        class SVGFile:public Resource
        {
            GODOT_CLASS(SVGFile,Resource)
            //Ref<Marshalls> marshalls;

            //void _store_compressed_data(String p_data);
            //String _decompress_data();

        public:
            String path;
            //bool compressed;
            String data;
            std::unique_ptr<lunasvg::Document> svg_doc;

            static void _register_methods();
            
            SVGFile():svg_doc(nullptr){}
            ~SVGFile(){}

            void _init();

            void _set_path(String p_path);
            String get_path()const{return path;}
            void _set_data(String p_data);
            String get_data();
        };
    }

#endif