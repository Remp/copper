#pragma once

#include "Copper/Resource/context.h"
#include "Copper/Resource/Resource.h"
#include "Copper/Core/io/IWriteStream.h"

namespace Copper
{
/*!
 * \brief MetaTableSectionWriter:
 * writes metatable to bundle file from \param context
 */
class MetaTableSectionWriter
{
public:
    explicit MetaTableSectionWriter(IWriteStream& stream, const std::filesystem::path& workingDir);

    bool Write(const std::vector<ParsedFileData>& files, const std::map<std::string, FileLayout>& filesLayout);

    static uint32_t CalculateMetadataSize(const std::vector<ParsedFileData>& parsedFilesData);

private:
    IWriteStream& m_Stream;
    std::filesystem::path m_WorkingDir;
};

/*!
 * \brief MetaTableSectionReader:
 * reads metatable from bundle file into \param context
 */
class MetaTableSectionReader
{
public:
    explicit MetaTableSectionReader(std::ifstream& stream, const std::filesystem::path& bundlePath);

    std::unordered_map<uint64_t, Ref<Resource>> Read();
    inline bool HasError() const { return m_Error; }

private:
    std::ifstream& m_Stream;
    std::filesystem::path m_BundlePath;
    bool m_Error{ false };
};
} // namespace Copper
