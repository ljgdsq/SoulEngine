#include "ContentBrowser.h"
#include <vector>
#include <string>
#include <filesystem>
#include "imgui.h"
using std::vector;
using std::string;
using std::pair;

using namespace std;

namespace SoulEditor
{
    pair<vector<string>, vector<string>> ListFilesAndDirs(const char* path)
    {
        filesystem::path root(path);
        vector<string> files;
        vector<string> directories;

        for (const auto& entry : filesystem::directory_iterator(root))
        {
            if (entry.is_directory())
            {
                directories.push_back(entry.path().string());
            }
            else if (entry.is_regular_file())
            {
                files.push_back(entry.path().string());
            }
        }

        return {directories, files};
    }

    void ContentBrowser::OnInitialize()
    {
        GuiWindow::OnInitialize();
    }

    void ContentBrowser::OnUpdate(float deltaTime)
    {
        GuiWindow::OnUpdate(deltaTime);
    }

    void ContentBrowser::OnRender()
    {
        GuiWindow::OnRender();
        ImGui::Begin(m_name.c_str());
        ImGui::Text("Content");
        RenderContent();
        ImGui::End();
    }

    void ContentBrowser::RenderContent()
    {
        auto root = std::filesystem::path(rootPath);
        for (const auto& c : currentPath)
        {
            root /= c;
        }

        std::filesystem::path path(root);
        auto [dirs, files] = ListFilesAndDirs(path.string().c_str());
        // Display directories
        for (const auto& dir : dirs)
        {
            std::filesystem::path dirPath(dir);
            bool isOpen = ImGui::TreeNodeEx(dirPath.filename().string().c_str(),
                                            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth);
            if (isOpen)
            {
                if (ImGui::IsItemClicked())
                {
                    currentPath.push_back(dir);
                    ImGui::SetNextItemOpen(true); // Ensure the next item is open
                }
                ImGui::TreePop();
            }
        }

        ImGui::Separator();
        // Display files
        for (const auto& file : files)
        {
            std::filesystem::path filePath(file);
            std::string filename = filePath.filename().string();
            std::string extension = filePath.extension().string();

            ImGui::TextUnformatted(filename.c_str());
        }

        if (!currentPath.empty())
        {
            ImGui::Separator();
            if (ImGui::Button("Back"))
            {
                currentPath.pop_back();
            }
        }
    }

    void ContentBrowser::OnShutdown()
    {
        GuiWindow::OnShutdown();
    }

    bool ContentBrowser::CanClose() const
    {
        return true;
    }
}
