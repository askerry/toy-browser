cc_binary(
        name="browser",
        srcs=[
                "main.cc", "util.h", "dom.h", "dom.cc", "parse/html.h", "parse/html.cc", "parse/parser.h", "parse/parser.cc",
                "parse/css.h", "parse/css.cc", "style.h", "style.cc", "layout.h", "layout.cc",
                "render/paint.h", "render/paint.cc", "render/text.h", "render/text.cc", "render/image.h", "render/image.cc",
                "color.h", "color.cc", "render/shape.h", "render/shape.cc", "constants.h",
        ],
        deps = [
              "@sfml//:sfml",
              "@com_google_absl//absl/strings",
              "@com_google_absl//absl/strings:str_format",
              "@com_github_gflags_gflags//:gflags",
        ],
)