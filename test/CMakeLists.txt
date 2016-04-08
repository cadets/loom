set(LLVM_LIT ${LLVM_BINARY_DIR}/llvm-lit)

set(SHOW_TEST_STATS true CACHE BOOL "Show statistics after running tests")
if (SHOW_TEST_STATS)
  message(STATUS "Will show statistics after each test run")
  set(LIT_OPTIONS "-sv")
else ()
  message(STATUS "Will run successful tests quietly")
  set(LIT_OPTIONS "-qv")
endif ()


add_custom_target(check
	COMMAND
		${LLVM_LIT} ${LIT_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}
		--param=build_dir=${CMAKE_BINARY_DIR}
		--param=source_dir=${CMAKE_SOURCE_DIR}
		--param=output_dir=${CMAKE_CURRENT_BINARY_DIR}
		--xunit-xml-output=loom.xml

	BYPRODUCTS loom.xml
	COMMENT "Running unit tests"
)

add_dependencies(check LLVMLoom)
