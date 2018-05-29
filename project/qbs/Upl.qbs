/*
 * Copyright (c) 2018 Viktor Kireev
 * Distributed under the MIT License
 */

import qbs

StaticLibrary {
    Depends { name: "cpp" }

    cpp.cxxLanguageVersion: "c++17"

    Properties {
        condition: qbs.toolchain.contains("clang")
        cpp.cxxLanguageVersion: "c++1z"
        cpp.cxxStandardLibrary: "libc++"
    }

    property path rootDir: sourceDirectory + "/../.."
    property path includeDir: rootDir + "/include"
    property path includeSubDirs: includeDir + "/**/"

    cpp.includePaths: includeDir

    Export {
        Depends { name: "cpp" }

        cpp.includePaths: "../../include"
    }

    Group {
        name: "Headers:"
        prefix: includeSubDirs
        files: ["*.h", "*.hpp"]
    }

    Group {
        fileTagsFilter: product.type
        qbs.install: true
    }
}
