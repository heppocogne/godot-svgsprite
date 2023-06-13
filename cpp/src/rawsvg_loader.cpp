#include "rawsvg_loader.h"
#include <File.hpp>

using namespace godot;

RawSvgLoader *RawSvgLoader::singleton = nullptr;

void RawSvgLoader::on_library_init()
{
    if (singleton != nullptr)
        delete singleton;
    singleton = new RawSvgLoader();
}

void RawSvgLoader::on_library_terminate()
{
    delete singleton;
    singleton = nullptr;
}

RawSvgLoader *RawSvgLoader::get_singleton()
{
    return singleton;
}

RawSvgLoader::~RawSvgLoader()
{
    for (auto &pair : docs_cache)
        docs_cache[pair.first] = nullptr;
}

std::unique_ptr<lunasvg::Document> &RawSvgLoader::load(const String &path)
{
    if (docs_cache.count(path) == 0)
    {
        Ref<File> ref_f = File::_new();
        Error err;
        if (path.ends_with(".rawsvg") || path.ends_with(".svg"))
            err = ref_f->open(path, File::READ);
        else if (path.ends_with(".rawsvgz"))
            err = ref_f->open_compressed(path, File::READ, File::COMPRESSION_DEFLATE);

        if (err == godot::Error::OK)
        {
            char *buf = ref_f->get_as_text().alloc_c_string();
            docs_cache[path] = lunasvg::Document::loadFromData(const_cast<const char *>(buf));
            godot::api->godot_free(buf);
            if (!docs_cache[path])
            {
                Godot::print_error("invalid svg file:" + path, __func__, __FILE__, __LINE__);
                docs_cache[path] = nullptr;
            }
        }
    }

    return docs_cache[path];
}