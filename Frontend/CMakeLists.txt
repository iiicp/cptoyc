add_definitions("-std=c++17")
add_definitions("-Wall")

include_directories(${cptoyc_SOURCE_DIR})


add_library(
        Frontend

        STATIC

        InitHeaderSearch.cpp
        TextDiagnosticBuffer.cpp
        PrintPreprocessedOutput.cpp
)