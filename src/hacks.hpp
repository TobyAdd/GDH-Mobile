#pragma once
#include <map>
#include <string>
#include <matjson.hpp>

static const auto folderPath = geode::Mod::get()->getSaveDir();
static const auto folderMacroPath = folderPath / "Macros";
static const auto fileDataPath = folderPath / "config.json";

class Config {
public:
    static Config& get() {
        static Config instance;
        return instance;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void load(const std::filesystem::path& filepath) {
        std::ifstream inFile(filepath);
        if (inFile.is_open()) {
            std::string file_contents((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            jsonData = matjson::parse(file_contents);
            inFile.close();
        } else {
            MessageBoxA(0, "error load", 0, 0);
        }
    }

    void save(const std::filesystem::path& filepath) const {
        std::ofstream outFile(filepath);
        if (outFile.is_open()) {
            outFile << jsonData.dump(4);
            outFile.close();
        } else {
            MessageBoxA(0, "error save", 0, 0);
        }
    }

    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        if (jsonData.contains(key)) {
            return jsonData[key].as<T>();
        }
        return defaultValue;
    }

    template<typename T>
    void set(const std::string& key, const T& value) {
        jsonData[key] = value; 
    }

private:
    Config() = default;
    matjson::Value jsonData;
};