#include <windows.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

bool LoadCustomFont(const std::string &fontPath) {
    // Load the font from the specified file path
    int numFontsAdded = AddFontResourceA(fontPath.c_str());
    if (numFontsAdded > 0) {
        return true;
    } else {
        std::cerr << "Failed to load font from " << fontPath << std::endl;
        return false;
    }
}

void UnloadCustomFont(const std::string &fontPath) {
    // Remove the font from the system font table
    RemoveFontResourceA(fontPath.c_str());
}

bool LoadFonts(const std::vector<std::string> &fonts, const std::string parentPath) {
    int index = 0;
    for (; index < fonts.size(); ++index) {
        if (!LoadCustomFont(parentPath + fonts[index])) {
            for (--index; index >= 0; --index) {
                UnloadCustomFont(parentPath + fonts[index]);
            }
            return false;
        }
    }
    return true;
}

void UnloadFonts(const std::vector<std::string> &fonts, const std::string parentPath) {
    for (auto font : fonts) {
        UnloadCustomFont(parentPath + font);
    }
}

bool SetEnvVariable(const char *name, const char *value) {
    if (SetEnvironmentVariableA(name, value)) {
        return true;
    } else {
        std::cerr << "Failed to set environment variable " << name << std::endl;
        return false;
    }
}

bool RunExecutable(const std::string &executablePath, const std::string &arguments) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Construct the full command line with arguments
    std::string fullCommand = executablePath + " " + arguments;

    // Start the child process.
    if (!CreateProcessA(NULL,  // No module name (use command line)
                        const_cast<char *>(fullCommand.c_str()),  // Command line
                        NULL,                                     // Process handle not inheritable
                        NULL,                                     // Thread handle not inheritable
                        FALSE,                                    // Set handle inheritance to FALSE
                        0,                                        // No creation flags
                        NULL,                                     // Use parent's environment block
                        NULL,                                     // Use parent's starting directory
                        &si,  // Pointer to STARTUPINFO structure
                        &pi)  // Pointer to PROCESS_INFORMATION structure
    ) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        return false;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

std::string GetExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

inline void HideConsole() { ::ShowWindow(::GetConsoleWindow(), SW_HIDE); }

int main(int argc, char *argv[]) {
    HideConsole();
    std::string exePath = GetExecutablePath();

    // Construct paths relative to the executable path
    std::string fontsDir = exePath + "\\fonts\\";
    std::string neovidePath = exePath + "\\neovide.exe";
    std::string xdgConfigHome = exePath + "\\config";

    std::vector<std::pair<std::string, std::string>> envs = {
        {"XDG_CONFIG_HOME", exePath + "\\config"},
        {"XDG_DATA_HOME", exePath + "\\data"},
        {"XDG_STATE_HOME", exePath + "\\data"},
        {"XDG_CACHE_HOME", exePath + "\\cache"}};

    std::vector<std::string> fonts = {
        "FiraCodeNerdFontMono-Medium.ttf", "FiraCodeNerdFontMono-Bold.ttf",
        "FiraCodeNerdFontMono-Light.ttf", "FiraCodeNerdFontMono-SemiBold.ttf",
        "FiraCodeNerdFontMono-Regular.ttf"};
    std::vector<std::string> paths = {"\\git\\bin", "\\nvim\\bin"};

    // Load the custom font
    if (!LoadFonts(fonts, fontsDir)) {
        return 1;
    }

    // Set the PATH environment variable
    char currentPath[32767];
    if (GetEnvironmentVariableA("PATH", currentPath, sizeof(currentPath)) > 0) {
        std::string newPath = std::string(currentPath);
        for (auto path : paths) {
            newPath += (";" + exePath + path);
        }

        if (!SetEnvVariable("PATH", newPath.c_str())) {
            std::cerr << "Could not set PATH";
            UnloadFonts(fonts, fontsDir);
            return 1;
        }
    } else {
        std::cerr << "Failed to get current PATH";
        UnloadFonts(fonts, fontsDir);
        return 1;
    }

    // Set environment variables
    for (auto i : envs) {
        if (!SetEnvVariable(i.first.c_str(), i.second.c_str())) {
            std::cerr << "Could not set " << i.first << " variable";
            UnloadFonts(fonts, fontsDir);
            return 1;
        }
    }

    // Pass all passed arguments to Neovide
    std::string neovideArguments = "";
    for (int i = 1; i < argc; ++i) {  // Start from 1 to skip the first argument (program name)
        neovideArguments += " " + std::string(argv[i]);
    }

    // Run the Neovide executable
    if (!RunExecutable(neovidePath, neovideArguments)) {
        UnloadFonts(fonts, fontsDir);
        return 1;
    }

    // Unload the custom fonts
    UnloadFonts(fonts, fontsDir);

    return 0;
}
