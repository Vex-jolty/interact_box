cmake_minimum_required(VERSION 3.10)

project(InteractBox VERSION 1.2.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILDING_FULL_PACKAGE TRUE)

option(CREATE_ISO "Whether to create an ISO containing the installer" OFF)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if (NOT MINGW)
		message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} is not a currently supported compiler")
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
else()
	message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} is not a currently supported operating system")
endif()

set(TARGET_WINDOWS_VERSION "win98" CACHE STRING "The version of Windows to target (win98, winme, win2k, winxp, vista, win7, win8, win10, win11)")

set_property(CACHE TARGET_WINDOWS_VERSION PROPERTY STRINGS
	win98 winme win2k winxp vista win7 win8 win10 win11
)

set(VERSIONS_THAT_REQUIRE_PTHREAD9X win98 winme win2k winxp)
set(VERSIONS_THAT_REQUIRE_MSVCRT40 win98 winme win2k winxp)
set(VERSIONS_THAT_REQUIRE_WRAPPERS win98 winme)

set(VERSIONS_THAT_USE_IB98 win98 winme win2k)
set(VERSIONS_THAT_USE_IBXP winxp)

if(NOT DEFINED TARGET_WINDOWS_VERSION)
	set(TARGET_WINDOWS_VERSION "win98"
		CACHE STRING "Target Windows version"
	)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/interact_box_libs)
add_subdirectory(${CMAKE_SOURCE_DIR}/interact_box_extras)

set(SERVER_FILES
	src/server/web_server.cpp
	src/server/http/http_response.cpp
	src/server/http/http_request.cpp
	src/server/http/http_route.cpp
	src/server/routes/route_handlers.cpp
	src/server/routes/route_handler_auxiliaries.cpp
)

set(UTIL_FILES
	src/utils/crash_util.cpp
	src/utils/message_box_util.cpp
	src/utils/file_util.cpp
	src/utils/time_util.cpp
	src/utils/reboot_util.cpp
	src/utils/logging_util.cpp
	src/utils/cmd_util.cpp
	src/utils/resolution_util.cpp
	src/utils/shell_util.cpp
	src/utils/config_util.cpp
)

if (WIN32)
	list(APPEND UTIL_FILES src/utils/registry_util.cpp)
endif()

add_executable(
	interact_box src/main.cpp
	${SERVER_FILES}
	${UTIL_FILES}
	src/errors/error_handler.cpp
	include/http_status_codes.h
	src/threads/tray_thread.cpp
	src/resources.rc
)

set(MAIN_DEPENDENCIES
	errors
	json_helper
	string_helper
	file_helper
	process_helper
	index_helper
	interact_box_settings
	trivia_game
	message_box_process
)

if (IS_WINDOWS_BUILD AND NOT TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_WRAPPERS)
	list(APPEND MAIN_DEPENDENCIES tts_process)
endif()

add_dependencies(
	interact_box
	${MAIN_DEPENDENCIES}
)

target_include_directories(interact_box PRIVATE
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/interact_box_libs/include
	${CMAKE_SOURCE_DIR}/interact_box_extras/include
)

target_link_libraries(interact_box PRIVATE platform_config errors string_helper file_helper json_helper process_helper)
if (IS_WINDOWS_BUILD)
	target_link_libraries(interact_box PRIVATE ws2_32 wininet version gdi32 dbghelp)
	if (NOT TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_PTHREAD9X)
		target_link_libraries(interact_box PRIVATE wbemuuid)
	endif()
endif()

link_base_libs(interact_box ${TARGET_WINDOWS_VERSION} TRUE)

string(REPLACE
	"." "," VERSION_STRING ${CMAKE_PROJECT_VERSION}
)

set(OUT_RC
	${CMAKE_CURRENT_BINARY_DIR}/src/interact_box_version.rc
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/src/version.rc.in
	${OUT_RC}
	@ONLY
)

target_sources(interact_box PRIVATE ${OUT_RC})

target_compile_options(interact_box PRIVATE
	$<$<CONFIG:Debug>:-g -gdwarf-4 -O0 -fno-omit-frame-pointer>
	-fpermissive
)

target_link_options(interact_box PRIVATE
	$<$<CONFIG:Debug>:-g>
)

configure_package_config_file(
	cmake/InteractBoxConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/InteractBoxConfig.cmake
	INSTALL_DESTINATION lib/cmake/InteractBox
)

if(EXISTS ${CMAKE_SOURCE_DIR}/malware_with_date_specific_deployment.json)
	file(COPY ${CMAKE_SOURCE_DIR}/malware_with_date_specific_deployment.json DESTINATION ${CMAKE_BINARY_DIR}/installer)
else()
	file(WRITE ${CMAKE_BINARY_DIR}/installer/malware_with_date_specific_deployment.json "{}")
endif()

if(IS_WINDOWS_BUILD AND TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_USE_IB98)
	set(PRODUCT_FLAVOR "Interact Box 98")
	set(HEADER_IMAGE "..\\..\\images\\Interact Box 98 header image.bmp")
	set(INSTALLER_ICON "..\\..\\icon\\win98.ico")
	set(NSIS_ARG "IS_WIN98_BUILD")
elseif(TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_USE_IBXP)
	set(PRODUCT_FLAVOR "Interact Box XP")
	set(HEADER_IMAGE "..\\..\\images\\Interact Box XP header image.bmp")
	set(INSTALLER_ICON "..\\..\\icon\\winxp.ico")
	set(NSIS_ARG "IS_WINXP_BUILD")
else()
	set(PRODUCT_FLAVOR "Interact Box Vista")
	set(HEADER_IMAGE "..\\..\\images\\Interact Box Vista header image.bmp")
	set(INSTALLER_ICON "..\\..\\icon\\vista.ico")
	set(NSIS_ARG "IS_VISTA_BUILD")
endif()

set(SOUNDS_DIR "${CMAKE_SOURCE_DIR}/sounds")

message(STATUS "This is the sounds dir: ${SOUNDS_DIR}")

set(SOUND_PACKS "")

file(GLOB CHILDREN RELATIVE "${SOUNDS_DIR}" "${SOUNDS_DIR}/*")

foreach(child ${CHILDREN})
	if (IS_DIRECTORY "${SOUNDS_DIR}/${child}")
		list(APPEND SOUND_PACKS "${child}")
	endif()
endforeach()

set(SOUND_INSTALL_COMMANDS "")

foreach(pack ${SOUND_PACKS})
	string(APPEND SOUND_INSTALL_COMMANDS
		"    SetOutPath \"$INSTDIR\\sounds\\${pack}\"\n"
		"    File /r \"..\\..\\sounds\\${pack}\\*.*\"\n\n"
	)
endforeach()

configure_file(
	"${CMAKE_SOURCE_DIR}/installer/install.nsi.in"
	"${CMAKE_BINARY_DIR}/installer/install.nsi"
	@ONLY
)

set(BOOT_KEYS "")
set(BOOT_BOOL_KEYS "")
set(XP_BOOL_KEYS "")

if(TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_USE_IB98)
	set(BOOT_KEYS "\"bootImagesDir\": \"boot_images\",\n	\"shutdownImagesDir\": \"shutdown_images\",")
	set(BOOT_BOOL_KEYS "\"useBootAndShutdownImages\": true,\n	\"useSystemBox\": true,")
else()
	set(XP_BOOL_KEYS "\"useTts\": true,")
endif()

configure_file(
	"${CMAKE_SOURCE_DIR}/interact_box_config.json.in"
	"${CMAKE_BINARY_DIR}/installer/interact_box_config.json"
	@ONLY
)

add_custom_command(TARGET interact_box
	POST_BUILD
	COMMAND pandoc -V lang=en-US -f markdown -s ${CMAKE_SOURCE_DIR}/README.md -o ${CMAKE_BINARY_DIR}/installer/README.rtf
	COMMAND pandoc -V lang=en-US -f markdown -s ${CMAKE_SOURCE_DIR}/LICENSE.md -o ${CMAKE_BINARY_DIR}/installer/LICENSE.rtf
	COMMENT "Generating license and readme"	
)

if(EXISTS ${CMAKE_SOURCE_DIR}/pre_compiled/system_message_process/system_box.exe AND NSIS_ARG STREQUAL "IS_WIN98_BUILD")
		add_custom_command(TARGET interact_box
		POST_BUILD
		COMMAND makensis 
		ARGS -D${NSIS_ARG}=1 -DUSE_PRE_COMPILED_EXES=1 ${CMAKE_BINARY_DIR}/installer/install.nsi
		COMMENT "Generating installer for ${PRODUCT_FLAVOR}"
	)
else()
	add_custom_command(TARGET interact_box
		POST_BUILD
		COMMAND makensis 
		ARGS -D${NSIS_ARG}=1 ${CMAKE_BINARY_DIR}/installer/install.nsi
		COMMENT "Generating installer for ${PRODUCT_FLAVOR}"
	)
endif()

if (CREATE_ISO)
	string(REPLACE " " "_" ISO_NAME ${PRODUCT_FLAVOR})
	add_custom_command(
		TARGET interact_box
		POST_BUILD
		COMMAND rm ${CMAKE_BINARY_DIR}/${ISO_NAME}.iso || true
		COMMAND mkisofs 
		ARGS -o ${CMAKE_BINARY_DIR}/${ISO_NAME}.iso -J -R -V "${PRODUCT_FLAVOR}" ${CMAKE_BINARY_DIR}/installer/setup.exe
		COMMENT "Generating ISO for ${PRODUCT_FLAVOR}"
	)
endif()