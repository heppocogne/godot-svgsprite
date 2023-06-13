#include "rawsvg_loader.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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
    if (path == "")
        return std::unique_ptr<lunasvg::Document>(nullptr);

    if (docs_cache.count(path) == 0)
    {
        Ref<FileAccess> ref_f;
        if (path.ends_with(".rawsvg") || path.ends_with(".svg"))
            ref_f = FileAccess::open(path, FileAccess::READ);
        else if (path.ends_with(".rawsvgz"))
            ref_f = FileAccess::open_compressed(path, FileAccess::READ, FileAccess::COMPRESSION_DEFLATE);

        if (ref_f != nullptr)
        {
            PackedByteArray buf = ref_f->get_as_text().to_utf8_buffer();
            char *chars = new char[buf.size()];
            for (int i = 0; i < buf.size(); i++)
                chars[i] = buf[i];
            docs_cache[path] = lunasvg::Document::loadFromData(chars);
            if (!docs_cache[path])
            {
                UtilityFunctions::push_error("invalid rawsvg file:" + path, __func__, __FILE__, __LINE__);
                docs_cache[path] = nullptr;
            }
        }
    }

    return docs_cache[path];
}