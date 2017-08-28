message(STATUS "Loading tests-gengine app...")

if (NOT CONFIGURED_ONCE)
	set(APP_TESTS_GENGINE_SOURCE_DIR ${APP_TESTS_GENGINE_DIR}/src)
endif()

add_executable(aurorafw_app_tests_gengine ${APP_TESTS_GENGINE_SOURCE_DIR}/test_gengine.cpp)

target_link_libraries(aurorafw_app_tests_gengine aurorafw-core aurorafw-gengine-core)

set_target_properties(aurorafw_app_tests_gengine PROPERTIES OUTPUT_NAME test_gengine.out)
