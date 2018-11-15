add_executable(ch.catch
    condition_test.cxx
    dictionary_test.cxx
    finder_test.cxx
    locale_utils_test.cxx
    string_utils_test.cxx
    structures_test.cxx
    catch_main.cxx)
target_link_libraries(ch.catch nuspell Catch2 Boost::locale)

add_executable(nuspell_legacy_tests legacy_test.cxx)
target_link_libraries(nuspell_legacy_tests nuspell Boost::locale)

list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/external/Catch2/contrib")
include(Catch)
catch_discover_tests(ch.catch)

find_program(BASH bash)
file(GLOB v1tests
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/v1cmdline
    "v1cmdline/*.dic"
    "v1cmdline/*.sug")
if (BASH)
    foreach(t ${v1tests})
        add_test(NAME ${t}
            COMMAND ${BASH} ${CMAKE_CURRENT_SOURCE_DIR}/test.sh
                    ${CMAKE_CURRENT_SOURCE_DIR}/v1cmdline/${t})
        set_tests_properties(${t} PROPERTIES
            ENVIRONMENT NUSPELL=$<TARGET_FILE:nuspell-bin>)
    endforeach()
else()
    # alternative test runner wriiten in C++, mostly for Windows
    foreach(t ${v1tests})
        add_test(NAME ${t}
            COMMAND nuspell_legacy_tests
                    ${CMAKE_CURRENT_SOURCE_DIR}/v1cmdline/${t})
    endforeach()
endif()

set_tests_properties(
allcaps.dic
allcaps2.dic
allcaps_utf.dic
base_utf.dic
checkcompoundrep.dic
forceucase.dic
hu.dic
nepali.dic
1463589.sug
1463589_utf.sug
allcaps.sug
allcaps2.sug
allcaps_utf.sug
base.sug
base_utf.sug
breakdefault.sug
checksharps.sug
checksharpsutf.sug
forceucase.sug
i35725.sug
i54633.sug
i58202.sug
keepcase.sug
nosuggest.sug
oconv.sug
opentaal_keepcase.sug
phone.sug
sug.sug
sugutf.sug
utf8_nonbmp.sug

PROPERTIES WILL_FAIL TRUE)