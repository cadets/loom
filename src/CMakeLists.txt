set(FILES
	DebugInfo
    DTraceLogger
	Instrumentation
	Instrumenter
	InstrStrategy
	IRUtils
	KTraceLogger
	Logger
	NVSerializer
	Policy
	PolicyFile
	Serializer
	Strings
	Transform
)

foreach(base IN LISTS FILES)
	list(APPEND HEADERS ${base}.hh)
	list(APPEND SOURCES ${base}.cc)
endforeach(base)

list(APPEND SOURCES OptPass.cc)
list(APPEND HEADERS Metadata.hh)

file(COPY ${HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include/loom)
install(FILES ${HEADERS} COMPONENT "development" DESTINATION "include/loom")

add_llvm_library(LLVMLoom MODULE BUILDTREE_ONLY ${SOURCES})
install(TARGETS LLVMLoom COMPONENT "runtime" DESTINATION "lib")
