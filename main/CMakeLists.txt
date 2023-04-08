idf_component_register(SRCS "main_rt_model.cpp"
                    INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++2b)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
target_compile_options(${COMPONENT_LIB} PRIVATE "-frtti")
target_compile_options(${COMPONENT_LIB} PRIVATE "-fexceptions")
target_compile_options(${COMPONENT_LIB} PRIVATE "-DRT_ESP32")
# target_compile_options(${COMPONENT_LIB} PRIVATE "-DNO_LOGGING")

# copied what follows from the CMakeFile from the ds18b20 example
set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")

# register_component()
