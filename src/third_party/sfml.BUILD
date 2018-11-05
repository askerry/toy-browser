cc_library(name = "sfml",
           srcs = glob([ "include/SFML/*/*.hpp", "lib/*.dylib" ]),
           hdrs = glob(["include/SFML/*.hpp"]),
           textual_hdrs = glob(["include/SFML/*/*.inl"]),
           includes = ["include"], visibility = ["//visibility:public"],
           linkstatic = 1, )