#include <filesystem>
#include <format>

namespace path {
inline std::filesystem::path& app_dir() {
    static std::filesystem::path _;
    return _;
}

inline std::string icon_path(const char* icon_name) {
    return std::format("{}/resources/icons/{}", app_dir().string(), icon_name);
}
} // end namespace path
