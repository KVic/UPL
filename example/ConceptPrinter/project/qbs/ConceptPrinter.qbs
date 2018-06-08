/*
 * Copyright (c) 2018 Viktor Kireev
 * Distributed under the MIT License
 */

import qbs
import qbs.Environment

CppApplication {
    Depends { name: "Upl" }

    consoleApplication: true

    cpp.cxxLanguageVersion: "c++17"

    Properties {
        condition: qbs.toolchain.contains("clang")
        cpp.cxxLanguageVersion: "c++1z"
        cpp.cxxStandardLibrary: "libc++"
    }

    cpp.cxxFlags: {
        var flags = [];
        if (Environment.getEnv("UPL_USE_CONCEPTS") != undefined)
            flags.push("-fconcepts");
        return flags;
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
