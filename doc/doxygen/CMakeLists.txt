find_package(Doxygen)

if(DOXYGEN_FOUND)
	set(DOX_BOOTSTRAP ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-bootstrapped)
	set(DOX_THEME ${CMAKE_CURRENT_SOURCE_DIR})

	configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		DEPENDS Doxyfile.in
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)
