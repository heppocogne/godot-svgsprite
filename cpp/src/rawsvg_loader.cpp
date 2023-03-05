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
    if (docs_cache.count(path) == 0)
    {
        Ref<FileAccess> ref_f;
        if (path.ends_with(".rawsvg"))
            ref_f = FileAccess::open(path, FileAccess::READ);
        else if (path.ends_with(".rawsvgz"))
            ref_f = FileAccess::open_compressed(path, FileAccess::READ, FileAccess::COMPRESSION_DEFLATE);

        if (ref_f != nullptr)
        {
            const uint8_t *buf = ref_f->get_as_text().to_utf8_buffer().ptr();
            docs_cache[path] = lunasvg::Document::loadFromData(reinterpret_cast<const char *>(buf));
            if (!docs_cache[path])
            {
                UtilityFunctions::push_error("invalid svg file:" + path, __func__, __FILE__, __LINE__);
                docs_cache[path] = nullptr;
            }
        }
    }

    return docs_cache[path];
}