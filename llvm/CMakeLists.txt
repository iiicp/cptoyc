add_definitions("-std=c++17")
add_definitions("-Wall")

add_library(
        LLVMADT

        STATIC

        Allocator.cpp
        APFloat.cpp
        APInt.cpp
        APSInt.cpp
        FoldingSet.cpp
        FormattedStream.cpp
        MemoryBuffer.cpp
        Process.cpp
        raw_ostream.cpp
        SmallPtrSet.cpp
        StringMap.cpp
        StringExtras.cpp
)