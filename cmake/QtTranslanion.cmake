if(NOT TARGET translate)
    add_custom_target(translate)
endif()

macro(QT_GET_FILENAME_COMPONENT _out _in _action)
    if(${_action} STREQUAL "EXT")
        get_filename_component(_filename ${_in} NAME)
        string(FIND ${_filename} "." pos REVERSE)
        if(pos)
            string(SUBSTRING ${_filename} ${pos} -1 ${_out})
        endif()
    elseif(${_action} STREQUAL "NAME_WE")
        get_filename_component(_filename ${_in} NAME)
        string(FIND ${_filename} "." pos REVERSE)
        if(pos)
            string(SUBSTRING ${_filename} 0 ${pos} ${_out})
        else()
            set(${_out} ${_filename})
        endif()
    else()
        message(FATAL_ERROR "QT_GET_REAL_FILENAME_COMPONENT: unsupported action ${_action}")
    endif()
endmacro()

macro(QT_MAKE_TRANSLATION _qms)
    set(_out_list)
    set(_ts_files ${ARGN})
    foreach (_ts ${_ts_files})
        qt_get_filename_component(_filename ${_ts} NAME_WE)
        set(_output "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${_filename}.qm")
        add_custom_command(OUTPUT ${_output}
            COMMAND
                ${QT_LRELEASE_EXECUTABLE}
                -nounfinished
                -compress
                -silent
                ${_ts}
                -qm ${_output}
            DEPENDS ${_ts}
            #WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Compiling ${_output}"
        )
        set(_out_list ${_out_list} ${_output})
    endforeach()
    set(${_qms} ${_out_list})
endmacro()

macro(QT_CREATE_TRANSLATION _tgt_name)
    set(_tgt ${_tgt_name}_translation)

    set(_codectr)
    if(CODECFORTR)
        set(_codectr -codecfortr ${CODECFORTR})
    endif()

    set(_input_tsfiles)
    set(_input_sources)
    set(_input_uis)
    set(_input_qmls)
    set(_lupdate_files ${ARGN})
    foreach (_file ${_lupdate_files})
        get_filename_component(_ext ${_file} EXT)
        get_filename_component(_abs_FILE ${_file} ABSOLUTE)
        if(_ext MATCHES ".ts")
            list(APPEND _input_tsfiles ${_abs_FILE})
         elseif(_ext MATCHES ".ui")
             list(APPEND _input_uis ${_abs_FILE})
         elseif(_ext MATCHES ".h" OR _ext MATCHES ".cpp")
             list(APPEND _input_sources ${_abs_FILE})
         elseif(_ext MATCHES ".qml")
             list(APPEND _input_qmls ${_abs_FILE})
        endif()
    endforeach()

    if(NOT _input_sources AND NOT _input_uis AND NOT _input_qmls)
        # nothing to translate
        return()
    endif()

    set(_includes)

    if(NOT IGNORE_INCLUDES_FOR_TR)
        get_directory_property(_inc_DIRS INCLUDE_DIRECTORIES)
        foreach(_include ${_inc_DIRS})
            get_filename_component(_abs_include "${_include}" ABSOLUTE)
            set(_includes ${_includes} -I "${_abs_include}")
        endforeach()
    endif()

    set(_commands ${_commands}
        COMMAND
            ${QT_LUPDATE_EXECUTABLE}
            -noobsolete
            -extensions ui,c,c++,cc,cpp,cxx,ch,h,h++,hh,hpp,hxx,qml
            ${_includes}
            ${_codectr}
            ${_input_sources}
            ${_input_uis}
            ${_input_qmls}
            -ts ${_input_tsfiles}
    )

    add_custom_target(${_tgt}
        ${_commands}
        DEPENDS
            ${_input_sources}
            ${_input_uis}
            ${_input_qmls}
        VERBATIM
    )

    add_dependencies(translate ${_tgt})

endmacro()
