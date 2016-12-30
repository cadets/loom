set(FILES
	DebugInfo
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
)

foreach(base IN LISTS FILES)
	list(APPEND HEADERS ${base}.hh)
	list(APPEND SOURCES ${base}.cc)
endforeach(base)

list(APPEND SOURCES OptPass.cc)

file(COPY ${HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/include/loom)
install(FILES ${HEADERS} DESTINATION "include/loom")

add_llvm_loadable_module(LLVMLoom ${SOURCES})
