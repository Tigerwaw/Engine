#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>

static void ReplaceSubstrings(std::string& aFileString, std::string& aStringToReplace, std::string& aReplacementString)
{
    size_t pos = 0;
    while (pos != std::string::npos)
    {
        pos = aFileString.find(aStringToReplace);
        if (pos == std::string::npos) break;

        aFileString.replace(pos, aStringToReplace.length(), aReplacementString);
    }
}

static void CreateFromTemplate(std::filesystem::path& aSource, std::filesystem::path& aDestination, std::string& aProjectName, std::string& aDestFilename)
{
    // Stream file to stringstream
    std::ifstream src(aSource);
    std::stringstream inBuffer;
    inBuffer << src.rdbuf();
    src.close();

    std::string fileContent = inBuffer.str();
    std::string replaceString = "APPNAME";
    ReplaceSubstrings(fileContent, replaceString, aProjectName);

    // Stream content to new file in directory
    std::stringstream outBuffer(fileContent);
    std::ofstream dest(aDestination / aDestFilename);
    dest << outBuffer.rdbuf();
    dest.close();
}

static void UpdateBasePremake(std::string& aProjectName)
{
    std::filesystem::path premakeTemplatePath("Premake/premake5.lua");

    // Stream file to stringstream
    std::ifstream src(premakeTemplatePath);
    std::stringstream inBuffer;
    inBuffer << src.rdbuf();
    src.close();

    std::string fileContent = inBuffer.str();
    std::string addition = "\ninclude(dirs.application .. \"" + aProjectName + "\")";
    fileContent.append(addition);

    // Stream content to new file in directory
    std::stringstream outBuffer(fileContent);
    std::ofstream dest(premakeTemplatePath);
    dest << outBuffer.rdbuf();
    dest.close();
}

int main()
{
    // Get project name
    std::string projectName;
    std::cout << "Enter a project name: ";
    std::cin >> projectName;

    // Create Project Folder
    std::filesystem::path projectPath("Source/Application/" + projectName);
    std::filesystem::create_directory(projectPath);

    // Create Bin Folder
    std::filesystem::path binPath("Bin/" + projectName);
    std::filesystem::create_directory(binPath);

    std::filesystem::path premakeTemplatePath("Premake/premake_template.lua");
    std::string premakeName = "premake5.lua";
    CreateFromTemplate(premakeTemplatePath, projectPath, projectName, premakeName);

    std::filesystem::path mainTemplatePath("Premake/app_template.cpp");
    std::string mainName = projectName + ".cpp";
    CreateFromTemplate(mainTemplatePath, projectPath, projectName, mainName);    

    UpdateBasePremake(projectName);

    std::cout << "Successfully created new project at " << projectPath << std::endl;
    system("pause");
}