cmake_minimum_required(VERSION 3.20)
project(CPUSchedulingSimulator VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Qt6 auto-tools ───────────────────────────────────────────────────────────
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ── Find Qt6 ─────────────────────────────────────────────────────────────────
# If Qt6 is not found in PATH, set Qt6_DIR manually, e.g.:
#   cmake -DQt6_DIR=/path/to/Qt/6.x.x/gcc_64/lib/cmake/Qt6 ..
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    main.cpp
    mainwindow.cpp
    scheduler.cpp
    ganttwidget.cpp
)

set(HEADERS
    process.h
    scheduler.h
    mainwindow.h
    ganttwidget.h
)

# ── Executable ────────────────────────────────────────────────────────────────
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)

# ── Compiler warnings ─────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Installation ──────────────────────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ── Qt deployment (Windows: windeployqt) ──────────────────────────────────────
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT windeployqt HINTS "${QT_BIN_DIR}")
    if(WINDEPLOYQT)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT}"
            "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Running windeployqt..."
        )
    endif()
endif()
