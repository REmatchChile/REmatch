cmake_minimum_required(VERSION 3.10)
project(JavaREmatch)

find_package(Java REQUIRED)

message(STATUS "javac path: ${Java_JAVAC_EXECUTABLE}")
message(STATUS "java path:  ${Java_JAVA_EXECUTABLE}")


option(BUILD_JAR_LIBRARY "Build the rematch.jar Java library" OFF)





set(OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(MAIN_CLASS ${CMAKE_SOURCE_DIR}/rematch/Main.java)
set(JAVACPP_JAR ${CMAKE_SOURCE_DIR}/javacpp/libs/javacpp.jar)

# Recolecta todos los archivos Java
file(GLOB_RECURSE JAVACPP_SOURCES
    "${CMAKE_SOURCE_DIR}/javacpp/*.java"
)

file(GLOB JAVA_SOURCES
    "${CMAKE_SOURCE_DIR}/rematch/*.java"
)




# 1. Incluye las rutas de encabezados necesarias para encontrar archivos .hpp
include_directories(/usr/local/include/REmatch)

# 2. Definir nombre JNI
set(JNI_WRAPPER_LIBRARY jniREmatch)

# 3. Archivo de salida de compilación Java, usado para saber si se debe compilar javacpp
set(JAVA_CLASSES_STAMP "${CMAKE_BINARY_DIR}/compiled.stamp")

# 3.5 Compila los .java solo si se necesita para el .so
add_custom_command(
    OUTPUT ${JAVA_CLASSES_STAMP}
    COMMAND javac 
            -cp ${JAVACPP_JAR}
            -d ${CMAKE_BINARY_DIR}/classes     
            ${JAVACPP_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E touch ${JAVA_CLASSES_STAMP}
    DEPENDS ${JAVACPP_SOURCES}
    COMMENT "Compilando archivos .java a .class"
)

# 4. Archivo esperado del JNI
set(JNI_OUTPUT_FILE "${CMAKE_BINARY_DIR}/jni/libjniREmatch.so")

# 5. Generar JNI .so solo si aun no existe
add_custom_command(
    OUTPUT ${JNI_OUTPUT_FILE}
    COMMAND java 
            -jar ${JAVACPP_JAR}
            -cp "${CMAKE_BINARY_DIR}/classes"
            -d "${CMAKE_BINARY_DIR}/jni"
            -o ${JNI_WRAPPER_LIBRARY}
            javacpp.REmatch
            javacpp.Flags
            javacpp.Query
            javacpp.Match
            javacpp.Span
            javacpp.StringSpanMap
            javacpp.MatchGeneratorIterator
            javacpp.MatchGenerator
            javacpp.StringVector
            javacpp.MatchVector
            javacpp.SpanVector
            javacpp.MultiQuery
            javacpp.MultiMatch
            javacpp.MultiMatchVector
            javacpp.MultiMatchGeneratorIterator
            javacpp.MultiMatchGenerator
            javacpp.JNILoader
            javacpp.NativeLoader
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/classes"
    COMMENT "Generando un solo JNI wrapper con javacpp"
    DEPENDS ${JAVA_CLASSES_STAMP}
)

# 6. Crea un target para el JNI .so
add_custom_target(GenerateJNIWrapper ALL
    DEPENDS ${JNI_OUTPUT_FILE}
)





# 7. Compilar JavaREmatch
add_custom_command(
    OUTPUT ${OUT_DIR}
    COMMAND ${Java_JAVAC_EXECUTABLE}
            -cp ${CMAKE_SOURCE_DIR}:${JAVACPP_JAR}
            -d ${OUT_DIR}
            ${JAVA_SOURCES}
    DEPENDS ${JAVA_SOURCES}
    COMMENT "Compiling Java files..."
)

add_custom_target(CompileJava2 ALL 
    DEPENDS ${OUT_DIR}
)

# 8. Ejecutar Main
add_custom_target(RunJavaClass2 ALL
    COMMAND ${Java_JAVA_EXECUTABLE}
            -cp "${OUT_DIR}:${JAVACPP_JAR}"
            -Djava.library.path=${CMAKE_SOURCE_DIR}/build/jni  # donde buscar la librería libjniREmatch.so 
            ${MAIN_CLASS}
    DEPENDS CompileJava2
    COMMENT "Running Main.java with JNI"
)




if(BUILD_JAR_LIBRARY)

    message(STATUS ">> Building rematch.jar library")

    set(JAR_OUTPUT_FILE ${CMAKE_BINARY_DIR}/rematch.jar)
    set(JAR_BUILD_DIR ${CMAKE_BINARY_DIR}/jar-build)
    set(MANIFEST_FILE ${CMAKE_BINARY_DIR}/generated_manifest.mf)

    file(WRITE ${MANIFEST_FILE}
        "Manifest-Version: 1.0\n"
        "Main-Class: rematch.Main\n"
    )

    add_custom_command(
        OUTPUT ${JAR_OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${JAR_BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${OUT_DIR} ${JAR_BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${JNI_OUTPUT_FILE} ${JAR_BUILD_DIR}/libjniREmatch.so
        COMMAND ${CMAKE_COMMAND} -E make_directory ${JAR_BUILD_DIR}/META-INF
        COMMAND ${CMAKE_COMMAND} -E copy ${MANIFEST_FILE} ${JAR_BUILD_DIR}/META-INF/MANIFEST.MF
        COMMAND jar cfm ${JAR_OUTPUT_FILE} ${JAR_BUILD_DIR}/META-INF/MANIFEST.MF -C ${JAR_BUILD_DIR} .
        COMMENT "Empaquetando rematch.jar con clases + .so"
        DEPENDS CompileJava2
    )

    add_custom_target(BuildJarLibrary ALL
        DEPENDS ${JAR_OUTPUT_FILE}
    )

    # Borra jar-build (no se vuelve a utilizar)
    add_custom_command(
        TARGET BuildJarLibrary
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${JAR_BUILD_DIR}
        COMMENT "Limpiando carpeta temporal jar-build"
    )

endif()
