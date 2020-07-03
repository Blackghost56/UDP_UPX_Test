# По мотивам
# https://github.com/couchbase/tlm/blob/master/cmake/Modules/PlatformIntrospection.cmake


# Returns a lowercased version of a given lsb_release field.
macro (_LSB_RELEASE field retval)

    execute_process(COMMAND lsb_release "--${field}"
        OUTPUT_VARIABLE _output ERROR_VARIABLE _output RESULT_VARIABLE _result)

    if (_result)
        message (FATAL_ERROR "Cannot determine Linux revision! Output from "
        "lsb_release --${field}: ${_output}")
    endif (_result)

    string (REGEX REPLACE "^[^:]*:" "" _output "${_output}")
    string (TOLOWER "${_output}" _output)
    string (STRIP "${_output}" ${retval})

endmacro (_LSB_RELEASE)


# Returns a lowercased version of a given /etc/os-release field.
macro (_OS_RELEASE field retval)

    file (STRINGS /etc/os-release vars)
    set (${_value} "${field}-NOTFOUND")

    foreach (var ${vars})

        if (var MATCHES "^${field}=(.*)")

            set (_value "${CMAKE_MATCH_1}")

            # Value may be quoted in single- or double-quotes; strip them
            if (_value MATCHES "^['\"](.*)['\"]\$")
                set (_value "${CMAKE_MATCH_1}")
            endif ()

            break ()

        endif ()
    endforeach ()

    set (${retval} "${_value}")

endmacro (_OS_RELEASE)


if (EXISTS "/etc/os-release")
    _OS_RELEASE (ID LINUX_DISTRO_ID)
    _OS_RELEASE (VERSION_ID LINUX_DISTRO_VERSION)
endif()

if (NOT (LINUX_DISTRO_ID AND LINUX_DISTRO_VERSION) )

    find_program(LSB_RELEASE lsb_release)

    if (LSB_RELEASE)
        _LSB_RELEASE (id LINUX_DISTRO_ID)
        _LSB_RELEASE (release LINUX_DISTRO_VERSION)
    else (LSB_RELEASE)
        message (WARNING "Can't determine Linux platform without /etc/os-release or lsb_release")
        set (_id "unknown")
        set (_ver "")
    endif (LSB_RELEASE)

endif ()

if (${LINUX_DISTRO_ID} STREQUAL "debian" OR ${LINUX_DISTRO_ID} STREQUAL "centos")
    # Just use the major version from the CentOS/Debian/RHEL identifier;
    # we don't need different builds for different minor versions.
    string (REGEX MATCH "[0-9]+" LINUX_DISTRO_VERSION "${LINUX_DISTRO_VERSION}")
endif ()

set (LINUX_DISTRO_FULL "${LINUX_DISTRO_ID}${LINUX_DISTRO_VERSION}")



find_program(ARCH_PROGRAM arch)

if(ARCH_PROGRAM)

    set(HAS_ARCH TRUE)

    execute_process(COMMAND ${ARCH_PROGRAM} OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
