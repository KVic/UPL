/*
 * Copyright (c) 2018 Viktor Kireev
 * Distributed under the MIT License
 */

import qbs

Project {
    property path ExamplePath: "../../example/"

    references: [
         ExamplePath + "WeakUnique/project/qbs/WeakUnique.qbs"
    ]
}
