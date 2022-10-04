#ifndef SVG_FILE_RESOURCE_H
    #define SVG_FILE_RESOURCE_H

    #include <Godot.hpp>
    #include <Resource.hpp>
    #include <memory>
    #include <lunasvg.h>

    namespace godot
    {
        class SVGFile:public Resource
        {
            std::unique_ptr<lunasvg::Document> _svg_doc;

        public:
            SVGFile();
            ~SVGFile();

            String data;
            String data_hash;
            String path;

            void _init();
        };
    }

#endif