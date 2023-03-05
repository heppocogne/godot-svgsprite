#ifndef RAWSVG_LOADER_H
#define RAWSVG_LOADER_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/string.hpp>

#include <map>
#include <memory>
#include <lunasvg.h>

namespace godot
{
    class RawSvgLoader
    {
        static RawSvgLoader *singleton;
        RawSvgLoader() {}
        std::map<String, std::unique_ptr<lunasvg::Document>> docs_cache;

    public:
        static void on_library_init();
        static void on_library_terminate();
        static RawSvgLoader *get_singleton();
        ~RawSvgLoader();

        std::unique_ptr<lunasvg::Document> &load(const String &path);
    };
};

#endif