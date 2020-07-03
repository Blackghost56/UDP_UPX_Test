set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Zee")
set(CPACK_PACKAGE_CONTACT "Michael Tsvetkov <blackghost0702@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Program for working with the UPX interface")
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

if(UNIX)

    if(CMAKE_SYSTEM_NAME MATCHES Linux)

	set(DEPLOY_DIR ${CMAKE_CURRENT_LIST_DIR}/../deploy/)
	FILE(WRITE ${DEPLOY_DIR}/${PROJECT_NAME}.desktop
                "[Desktop Entry]\n"
                "Name=${PROJECT_NAME}\n"
                "Exec=/usr/bin/${PROJECT_NAME}\n"
                "Icon=/usr/share/pixmaps/${PROJECT_NAME}.png\n"
                "Type=Application"
        )

        set(PERMISSIONS_755 OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
        set(PERMISSIONS_775 OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE GROUP_WRITE WORLD_READ WORLD_EXECUTE)
        set(PERMISSIONS_666 OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE)

	set(INSTALL_BINDIR /opt/${PROJECT_NAME} CACHE PATH "Installation directory for executables")

        install(FILES "${OUT_DIR}/${PROJECT_NAME}" DESTINATION "${INSTALL_BINDIR}" PERMISSIONS ${PERMISSIONS_755})
        install(FILES "${DEPLOY_DIR}/${PROJECT_NAME}.desktop" DESTINATION "/usr/share/applications/" PERMISSIONS ${PERMISSIONS_775})
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/res/${PROJECT_NAME}.png" DESTINATION "/usr/share/pixmaps/" RENAME "${PROJECT_NAME}.png" PERMISSIONS ${PERMISSIONS_666})

	# Ссылка в user/bin на исполняемый файл 
	install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${INSTALL_BINDIR}/${PROJECT_NAME} symlink)")
    	install(FILES "${CMAKE_BINARY_DIR}/symlink" DESTINATION "usr/bin/" RENAME "${PROJECT_NAME}")

        include(LinuxInfo)
        if (${LINUX_DISTRO_ID} STREQUAL "debian")

            if(HAS_ARCH)
                if(ARCH STREQUAL "x86_64")
                    set(DEBIAN_ARCH "amd64")
                else()
                    set(DEBIAN_ARCH ${ARCH})
                endif()
            endif()
            list(APPEND CPACK_GENERATOR "DEB")

            set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
            set(CPACK_DEBIAN_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${LINUX_DISTRO_FULL}_${DEBIAN_ARCH}.deb)
	    #В старых версиях CMake CPACK_DEBIAN_FILE_NAME отсутсвует. Определяем CPACK_PACKAGE_FILE_NAME
            if(${CMAKE_VERSION} VERSION_LESS "3.6.3")
                set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${LINUX_DISTRO_FULL}_${DEBIAN_ARCH})
            endif()
            set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
            
	    #CMake автоматически находит зависимости, вручную имеет смысл добавлять только те пакеты которые автоматически не добавляются
	    if(QT5_TARGET)
	       #set(CPACK_DEBIAN_PACKAGE_DEPENDS "menu, libcap2-bin") 
	    else()
               #set(CPACK_DEBIAN_PACKAGE_DEPENDS "menu, libcap2-bin, opencv-libs") 
	    endif()
        endif()

        if (${LINUX_DISTRO_ID} STREQUAL "ubuntu")

            if(HAS_ARCH)
                if(ARCH STREQUAL "x86_64")
                    set(DEBIAN_ARCH "amd64")
                else()
                    set(DEBIAN_ARCH ${ARCH})
                endif()
            endif()
            list(APPEND CPACK_GENERATOR "DEB")

            set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
            set(CPACK_DEBIAN_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${LINUX_DISTRO_FULL}_${DEBIAN_ARCH}.deb)
	    #В старых версиях CMake CPACK_DEBIAN_FILE_NAME отсутсвует. Определяем CPACK_PACKAGE_FILE_NAME
            if(${CMAKE_VERSION} VERSION_LESS "3.6.3")
                set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${LINUX_DISTRO_FULL}_${DEBIAN_ARCH})
            endif()
            set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
            
	    #CMake автоматически находит зависимости, вручную имеет смысл добавлять только те пакеты которые автоматически не добавляются
	    if(QT5_TARGET)
	        #set(CPACK_DEBIAN_PACKAGE_DEPENDS "menu, libcap2-bin") 
	    else()
            	#set(CPACK_DEBIAN_PACKAGE_DEPENDS "menu, libcap2-bin, opencv-libs") 
	    endif()
        endif()
	
        if (${LINUX_DISTRO_ID} STREQUAL "centos")

            list(APPEND CPACK_GENERATOR "RPM")
            set(CPACK_RPM_PACKAGE_RELEASE "1")

            set(CPACK_RPM_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}.el${LINUX_DISTRO_VERSION}.${ARCH}.rpm)
            #В старых версиях CMake CPACK_RPM_FILE_NAME отсутсвует. Определяем CPACK_PACKAGE_FILE_NAME
            if(${CMAKE_VERSION} VERSION_LESS "3.6.3")
                set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}.el${LINUX_DISTRO_VERSION}.${ARCH})
            endif()
	    
	    #CMake автоматически находит зависимости, вручную имеет смысл добавлять только те пакеты которые автоматически не добавляются
	    if(QT5_TARGET)
	      #set(CPACK_RPM_PACKAGE_REQUIRES "menu")
	    else()
	      #set(CPACK_RPM_PACKAGE_REQUIRES "menu")
	    endif()

        endif()
    endif()
endif()


message(STATUS "CPack generators: ${CPACK_GENERATOR}")

include(CPack)
