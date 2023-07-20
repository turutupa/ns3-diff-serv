
build_lib_example(
    NAME drr-validation-example
    SOURCE_FILES drr-validation-example.cc
    LIBRARIES_TO_LINK ${libdiff-serv}
)

build_lib_example(
    NAME spq-validation-example
    SOURCE_FILES spq-validation-example.cc
    LIBRARIES_TO_LINK ${libdiff-serv}
)


