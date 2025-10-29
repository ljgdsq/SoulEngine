#pragma once
#include <string>
#include <filesystem>
namespace SoulEngine
{
    class FileUtil
    {
    private:
        // 禁止实例化
        FileUtil() = default;

    public:
        /// @brief 提取文件名
        /// @param filePath 文件路径
        /// @return 文件名
        static std::string ExtractFileName(const std::string &filePath)
        {
            size_t lastSlash = filePath.find_last_of("/\\");
            size_t lastDot = filePath.find_last_of(".");

            if (lastSlash == std::string::npos)
            {
                lastSlash = 0;
            }
            else
            {
                lastSlash++;
            }

            if (lastDot == std::string::npos || lastDot < lastSlash)
            {
                return filePath.substr(lastSlash);
            }

            return filePath.substr(lastSlash, lastDot - lastSlash);
        }

        static bool HasExtension(const std::string &filePath, const std::string &extension)
        {
            if (filePath.empty() || extension.empty())
            {
                return false;
            }
            // 转换为小写以进行不区分大小写的比较
            std::string lowerFilePath = filePath;
            std::string lowerExtension = extension;
            std::transform(lowerFilePath.begin(), lowerFilePath.end(), lowerFilePath.begin(), ::tolower);
            std::transform(lowerExtension.begin(), lowerExtension.end(), lowerExtension.begin(), ::tolower);

            return lowerFilePath.size() >= lowerExtension.size() &&
                   lowerFilePath.compare(lowerFilePath.size() - lowerExtension.size(), lowerExtension.size(), lowerExtension) == 0;
        }

        static std::string GetFileExtension(const std::string &filePath)
        {
            size_t lastDot = filePath.find_last_of(".");
            if (lastDot == std::string::npos || lastDot == filePath.size() - 1)
            {
                return ""; // 没有扩展名
            }
            return filePath.substr(lastDot + 1); // 返回扩展名，不包括点
        }

        /// @brief 提取路径，父路径和当前文件夹的名称
        /// @param filePath 文件路径  
        /// @return 父路径和当前文件夹名称的pair
        static std::pair<std::string, std::string> ExtractDirectory(const std::string &filePath)
        {
            if (filePath.empty())
            {
                return std::make_pair("", "");
            }

            std::filesystem::path path(filePath);
    
            // 如果路径以分隔符结尾，移除它
            if (path.has_filename() == false && path.has_parent_path())
            {
                path = path.parent_path();
            }

            std::string parentPath = path.has_parent_path() ? path.parent_path().string() : "";
            std::string folderName = path.has_filename() ? path.filename().string() : "";

            return std::make_pair(parentPath, folderName);
        }
        
    };
}
