cmake_minimum_required(VERSION 3.10)

project(interact_box VERSION 1.2.2)

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

# Format: "Flavor;RequirePThread9x;RequireMsvcrt40;RequireWrappers;SignBinaries;RequiredArch"
set(win_props_win98  "98;TRUE;TRUE;TRUE;FALSE;32")
set(win_props_winme  "98;TRUE;TRUE;TRUE;FALSE;32")
set(win_props_win2k  "98;TRUE;TRUE;FALSE;FALSE;32")
set(win_props_winxp  "XP;TRUE;TRUE;FALSE;FALSE;32")
set(win_props_vista  "Vista;FALSE;FALSE;FALSE;FALSE;64")
set(win_props_win7   "Vista;FALSE;FALSE;FALSE;FALSE;64")
set(win_props_win8   "Vista;FALSE;FALSE;FALSE;FALSE;64")
set(win_props_win10  "Win10;FALSE;FALSE;FALSE;TRUE;64")
set(win_props_win11  "Win10;FALSE;FALSE;FALSE;TRUE;64")

set(TARGET_WINDOWS_VERSION "win98" CACHE STRING "The version of Windows to target (win98, winme, win2k, winxp, vista, win7, win8, win10, win11)")

set_property(CACHE TARGET_WINDOWS_VERSION PROPERTY STRINGS
	win98 winme win2k winxp vista win7 win8 win10 win11
)

if(NOT DEFINED TARGET_WINDOWS_VERSION)
	set(TARGET_WINDOWS_VERSION "win98"
		CACHE STRING "Target Windows version"
	)
endif()

if(WIN32)
	list(GET win_props_${TARGET_WINDOWS_VERSION} 0 OS_FLAVOR)
	list(GET win_props_${TARGET_WINDOWS_VERSION} 1 REQ_PTHREAD9X)
	list(GET win_props_${TARGET_WINDOWS_VERSION} 2 REQ_MSVCRT40)
	list(GET win_props_${TARGET_WINDOWS_VERSION} 3 REQ_WRAPPERS)
	list(GET win_props_${TARGET_WINDOWS_VERSION} 4 SIGN_BINARIES)
	list(GET win_props_${TARGET_WINDOWS_VERSION} 5 REQUIRED_ARCH)

	if(NOT DEFINED win_props_${TARGET_WINDOWS_VERSION})
		message(FATAL_ERROR "Unknown TARGET_WINDOWS_VERSION: ${TARGET_WINDOWS_VERSION}")
	endif()

	string(TOLOWER "${OS_FLAVOR}" flavor_lower)
	string(TOUPPER "${OS_FLAVOR}" flavor_upper)
	set(INSTALLER_ICON "..\\..\\icon\\win${flavor_lower}.ico") # Handled Vista special case fix below
	if(OS_FLAVOR STREQUAL "Vista" OR OS_FLAVOR STREQUAL "Win10")
		set(INSTALLER_ICON "..\\..\\icon\\${flavor_lower}.ico")
		set(INCLUDE_X64 "!include \"x64.nsh\"\n")
		set(RUN_PAGE_DEFINE "!define MUI_FINISHPAGE_RUN_FUNCTION \"CreateScheduledTaskOnFinish\"")
		if(OS_FLAVOR STREQUAL "Vista")
			set(TASKS_COMMAND [[
				ExecWait '"schtasks.exe" /create /tn "Interact Box" /tr "\"$INSTDIR\interact_box.exe\"" /sc onlogon /RL HIGHEST /f'
			]])
		else()
			set(TASKS_COMMAND [[
				${DisableX64FSRedirection}
				ExecWait '"$WINDIR\sysnative\schtasks.exe" /create /tn "Interact Box" /tr "\"$INSTDIR\interact_box.exe\"" /sc onlogon /RL HIGHEST /f'
				${EnableX64FSRedirection}
			]])
		endif()
		
	endif()
	set(NSIS_ARG "IS_${flavor_upper}_BUILD")
	set(HEADER_IMAGE "..\\..\\images\\${PRODUCT_FLAVOR} header image.bmp")
else()
	set(OS_FLAVOR "Linux")
	set(REQUIRED_ARCH "64")
endif()

set(PRODUCT_FLAVOR "Interact Box ${OS_FLAVOR}")

add_subdirectory(${CMAKE_SOURCE_DIR}/interact_box_libs)
add_subdirectory(${CMAKE_SOURCE_DIR}/interact_box_extras)

set(SERVER_FILES
	src/server/web_server.cpp
	src/server/http/http_response.cpp
	src/server/http/http_request.cpp
	src/server/http/http_route.cpp
	src/server/routes/route_handlers.cpp
	src/server/routes/route_handler_auxiliaries.cpp
	src/server/socket_backend/socket_backend_windows.cpp
	src/server/socket_backend/socket_backend_linux.cpp
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

set(ADDITIONAL_SOURCE_FILES
	src/errors/error_handler.cpp
	include/http_status_codes.h
)

if (WIN32)
	list(APPEND UTIL_FILES
		src/utils/registry_util.cpp
		src/utils/instance_status_check_util.cpp
	)
	list(APPEND ADDITIONAL_SOURCE_FILES
		src/threads/tray_thread.cpp
		src/resources.rc
	)
endif()

if(REQUIRED_ARCH EQUAL 32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(FATAL_ERROR "Configuration Failed: ${TARGET_WINDOWS_VERSION} requires a 32-bit toolchain, but a 64-bit compiler was detected.")
elseif(REQUIRED_ARCH EQUAL 64 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
	if(WIN32)
		message(FATAL_ERROR "Configuration Failed: ${TARGET_WINDOWS_VERSION} requires a 64-bit toolchain, but a 32-bit compiler was detected.")
	else()
		message(FATAL_ERROR "Configuration Failed: Linux edition requires a 64-bit toolchain, but a 32-bit compiler was detected.")
	endif()
endif()

add_executable(
	${PROJECT_NAME} src/main.cpp
	${SERVER_FILES}
	${UTIL_FILES}
	${ADDITIONAL_SOURCE_FILES}
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

if (WIN32 AND NOT REQ_WRAPPERS)
	list(APPEND MAIN_DEPENDENCIES tts_process)
endif()

add_dependencies(
	${PROJECT_NAME}
	${MAIN_DEPENDENCIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/interact_box_libs/include
	${CMAKE_SOURCE_DIR}/interact_box_extras/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE platform_config errors string_helper file_helper json_helper process_helper)
if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wininet version gdi32 dbghelp)
	if (NOT REQ_PTHREAD)
		target_link_libraries(${PROJECT_NAME} PRIVATE wbemuuid)
	endif()
endif()

link_base_libs(${PROJECT_NAME} ${TARGET_WINDOWS_VERSION} TRUE)

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

target_sources(${PROJECT_NAME} PRIVATE ${OUT_RC})

target_compile_options(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:Debug>:-g -gdwarf-4 -O0 -fno-omit-frame-pointer>
	-fpermissive
)

target_link_options(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:Debug>:-g>
)

configure_package_config_file(
	cmake/InteractBoxConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/InteractBoxConfig.cmake
	INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

if(EXISTS ${CMAKE_SOURCE_DIR}/malware_with_date_specific_deployment.json)
	file(COPY ${CMAKE_SOURCE_DIR}/malware_with_date_specific_deployment.json DESTINATION ${CMAKE_BINARY_DIR}/installer)
else()
	file(WRITE ${CMAKE_BINARY_DIR}/installer/malware_with_date_specific_deployment.json "{}")
endif()


set(SOUNDS_DIR "${CMAKE_SOURCE_DIR}/sounds")

set(SOUND_PACKS "")

file(GLOB CHILDREN RELATIVE "${SOUNDS_DIR}" "${SOUNDS_DIR}/*")

foreach(child ${CHILDREN})
	if (IS_DIRECTORY "${SOUNDS_DIR}/${child}")
		list(APPEND SOUND_PACKS "${child}")
	endif()
endforeach()

set(SOUND_INSTALL_COMMANDS "")
set(CERT_INSTALL_COMMANDS "")

foreach(pack ${SOUND_PACKS})
	string(APPEND SOUND_INSTALL_COMMANDS
		"    SetOutPath \"$INSTDIR\\sounds\\${pack}\"\n"
		"    File /r \"..\\..\\sounds\\${pack}\\*.*\"\n\n"
	)
endforeach()

if(SIGN_BINARIES)
	if(EXISTS ${CMAKE_SOURCE_DIR}/keys)
		set(KEYS_DIR ${CMAKE_SOURCE_DIR}/keys)
	else()
		set(KEYS_DIR /work/keys)
	endif()

	file(COPY ${KEYS_DIR}/interactbox_cert.pem DESTINATION ${CMAKE_BINARY_DIR}/installer)
	file(COPY ${KEYS_DIR}/interactbox_cert.pfx DESTINATION ${CMAKE_BINARY_DIR}/installer)

	set(CERT_INSTALL_COMMANDS "    File \"interactbox_cert.pem\"")

	set(FINAL_CERT_FILE_PATH ${CMAKE_BINARY_DIR}/installer/interactbox_cert.pfx)
endif()

if(OS_FLAVOR STREQUAL "98" OR OS_FLAVOR STREQUAL "XP")
	set(CONFIG_OUT_PATH "\$WINDIR")
else()
	set(CONFIG_OUT_PATH "\$APPDATA\\Interact Box")
endif()

configure_file(
	"${CMAKE_SOURCE_DIR}/installer/install.nsi.in"
	"${CMAKE_BINARY_DIR}/installer/install.nsi"
	@ONLY
)

set(BOOT_KEYS "")
set(BOOT_BOOL_KEYS "")
set(XP_BOOL_KEYS "")

if(WIN32 AND OS_FLAVOR STREQUAL "98")
	set(BOOT_KEYS "\"bootImagesDir\": \"boot_images\",\n	\"shutdownImagesDir\": \"shutdown_images\",")
	set(BOOT_BOOL_KEYS "\"useBootAndShutdownImages\": true,\n	\"useSystemBox\": true,")
elseif(WIN32)
	set(XP_BOOL_KEYS "\"useTts\": true,")
endif()

if(WIN32 AND OS_FLAVOR STREQUAL "Win10")
	set(DEFAULT_PORT "8080")
else()
	set(DEFAULT_PORT "80")
endif()

configure_file(
	"${CMAKE_SOURCE_DIR}/interact_box_config.json.in"
	"${CMAKE_BINARY_DIR}/installer/interact_box_config.json"
	@ONLY
)

add_custom_command(TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND pandoc -V lang=en-US -f markdown -s ${CMAKE_SOURCE_DIR}/README.md -o ${CMAKE_BINARY_DIR}/installer/README.rtf
	COMMAND pandoc -V lang=en-US -f markdown -s ${CMAKE_SOURCE_DIR}/LICENSE.md -o ${CMAKE_BINARY_DIR}/installer/LICENSE.rtf
	COMMENT "Generating license and readme"	
)

if (LINUX)
	file(COPY ${CMAKE_SOURCE_DIR}/installer/install.sh DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY ${CMAKE_SOURCE_DIR}/icon/${PROJECT_NAME}.png DESTINATION ${CMAKE_BINARY_DIR}/icon)
	file(COPY ${CMAKE_SOURCE_DIR}/interact_box_extras/src/interact_box_settings/icon/config_icon-linux.ico DESTINATION ${CMAKE_BINARY_DIR}/icon)
	file(COPY ${CMAKE_SOURCE_DIR}/interact_box_extras/src/malware_date_settings/icon/malware_date_settings.ico DESTINATION ${CMAKE_BINARY_DIR}/icon)
	file(COPY ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME}.desktop DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY ${CMAKE_SOURCE_DIR}/interact_box_extras/src/interact_box_settings/interact_box_settings.desktop DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY ${CMAKE_SOURCE_DIR}/interact_box_extras/src/malware_date_settings/malware_date_settings.desktop DESTINATION ${CMAKE_BINARY_DIR})
endif()

if(WIN32)
	if(EXISTS ${CMAKE_SOURCE_DIR}/pre_compiled/system_message_process/system_box.exe AND OS_FLAVOR STREQUAL "98")
		add_custom_command(TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND makensis 
			ARGS -D${NSIS_ARG}=1 -DUSE_PRE_COMPILED_EXES=1 ${CMAKE_BINARY_DIR}/installer/install.nsi
			COMMENT "Generating installer for ${PRODUCT_FLAVOR}"
		)
	else()
		if(SIGN_BINARIES)
			add_custom_command(TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND osslsigncode sign -pkcs12 ${FINAL_CERT_FILE_PATH} -pass "${CERT_PASS}" -n \"Interact Box\" -in ${CMAKE_BINARY_DIR}/interact_box.exe -out ${CMAKE_BINARY_DIR}/interact_box_signed.exe
				COMMAND rm ${CMAKE_BINARY_DIR}/interact_box.exe
				COMMAND mv ${CMAKE_BINARY_DIR}/interact_box_signed.exe ${CMAKE_BINARY_DIR}/interact_box.exe
				COMMENT "Signing Interact Box"
			)
			
		endif()
		add_custom_command(TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND makensis 
			ARGS -D${NSIS_ARG}=1 ${CMAKE_BINARY_DIR}/installer/install.nsi
			COMMENT "Generating installer for ${PRODUCT_FLAVOR}"
		)
		if(SIGN_BINARIES)
			add_custom_command(TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND osslsigncode sign -pkcs12 ${FINAL_CERT_FILE_PATH} -pass "${CERT_PASS}" -n \"Interact Box\" -in ${CMAKE_BINARY_DIR}/installer/setup.exe -out ${CMAKE_BINARY_DIR}/installer/setup_signed.exe
				COMMAND rm ${CMAKE_BINARY_DIR}/installer/setup.exe
				COMMAND mv ${CMAKE_BINARY_DIR}/installer/setup_signed.exe ${CMAKE_BINARY_DIR}/installer/setup.exe
				COMMENT "Signing Installer"
			)
		endif()
	endif()
else()
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND tar
		ARGS -C ${CMAKE_BINARY_DIR} --exclude=*CMake* --exclude=*cmake* -czf ${PROJECT_NAME}.tar.gz ${PROJECT_NAME} install.sh interact_box_extras/src/**/* icon/* *.desktop installer/interact_box_config.json installer/malware_with_date_specific_deployment.json
		COMMENT "Generating tarball"
	)
endif()

if (CREATE_ISO)
	if (WIN32)
		set(FILE_TO_BURN ${CMAKE_BINARY_DIR}/installer/setup.exe)
	else()
		set(FILE_TO_BURN ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.tar.gz)
	endif()
	string(REPLACE " " "_" ISO_NAME ${PRODUCT_FLAVOR})
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND rm ${CMAKE_BINARY_DIR}/${ISO_NAME}.iso || true
		COMMAND mkisofs 
		ARGS -o ${CMAKE_BINARY_DIR}/${ISO_NAME}.iso -J -R -V "${PRODUCT_FLAVOR}" ${FILE_TO_BURN}
		COMMENT "Generating ISO for ${PRODUCT_FLAVOR}"
	)
endif()