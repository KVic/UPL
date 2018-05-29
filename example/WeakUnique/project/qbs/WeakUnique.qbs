/*
 * Copyright (c) 2018 Viktor Kireev
 * Distributed under the MIT License
 */

import qbs

CppApplication {
    Depends { name: "Upl" }

    consoleApplication: true

    cpp.cxxLanguageVersion: "c++17"

    Properties {
        condition: qbs.toolchain.contains("clang")
        cpp.cxxLanguageVersion: "c++1z"
        cpp.cxxStandardLibrary: "libc++"
    }

    property path rootDir: sourceDirectory + "/../.."
    property path sourceDir: rootDir + "/src"
    property path sourceSubDirs: sourceDir + "/**/"

    cpp.includePaths: sourceDir

    Group {
        name: "Sources"
        prefix: sourceSubDirs
        files: [ "*.h", "*.hpp", "*.c", "*.cpp" ]
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
    }
}
