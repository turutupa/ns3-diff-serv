check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/diff-serv-examples-test-suite.cc
        )
endif()

set(source_files
  model/diff-serv.cc
  model/traffic-class.cc
  model/filter.cc
  model/spq.cc
  model/drr.cc
  model/filter-elements/dest-ip-mask.cc
  model/filter-elements/dest-port.cc
  model/filter-elements/protocol.cc
  model/filter-elements/source-ip-mask.cc
  model/filter-elements/source-port.cc
  helper/diff-serv-helper.cc
)

set(header_files
  model/diff-serv.h
  model/traffic-class.h
  model/filter.h
  model/spq.h
  model/drr.h
  model/filter-elements/filter-element.h
  model/filter-elements/dest-ip-mask.h
  model/filter-elements/dest-port.h
  model/filter-elements/protocol.h
  model/filter-elements/source-ip-mask.h
  model/filter-elements/source-port.h
  helper/diff-serv-helper.h
)

set(required_libraries
  ${libcore}
  ${libinternet}
  ${libapplications}
  ${libpoint-to-point}
)

set(test_sources
  test/diff-serv-test-suite.cc
)

build_lib(
    LIBNAME diff-serv
    SOURCE_FILES ${source_files}
    HEADER_FILES ${header_files}
    LIBRARIES_TO_LINK ${required_libraries}
    TEST_SOURCES ${test_sources}
)

