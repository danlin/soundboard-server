cmake_minimum_required(VERSION 3.15)
project("Ultraschall Soundboard" VERSION 4.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY JUCE_ENABLE_MODULE_SOURCE_GROUPS ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)
endif()

add_subdirectory(./JUCE JUCE)

juce_add_modules(ALIAS_NAMESPACE ultraschall modules/ultraschall_cli)
juce_add_modules(ALIAS_NAMESPACE ultraschall modules/ultraschall_audio)
juce_add_modules(ALIAS_NAMESPACE ultraschall modules/ultraschall_store)

add_subdirectory(src/commandline Commandline)
add_subdirectory(src/plugin Plugin)



