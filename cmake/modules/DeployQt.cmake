function(deploy_qt target)
    if(NOT WIN32)
        return()
    endif()

    get_target_property(QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")
    
    if(NOT WINDEPLOYQT_EXECUTABLE)
        message(WARNING "windeployqt не найден, развертывание Qt отключено для цели ${target}")
        return()
    endif()

    message(STATUS "Настройка развертывания Qt для цели: ${target}")

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E env "PATH=${QT_BIN_DIR}" 
                "${WINDEPLOYQT_EXECUTABLE}"
                --verbose 1
                --no-translations
                --no-compiler-runtime
                --dir \"$<TARGET_FILE_DIR:${target}>\"
                \"$<TARGET_FILE:${target}>\"
        COMMENT "Запуск windeployqt для развертывания Qt..."
    )
endfunction(deploy_qt)