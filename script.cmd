@echo off

setlocal enabledelayedexpansion

set temporary_file="temporary.tmp"
set ntp_server="pool.ntp.org"

set log_file_path=%1
set base_path=%2
set process_to_kill=%3
set archive_path=%4

if "%log_file_path%"=="" (
    echo "Usage: <log_file_name> [base_path] [process_to_kill]"
    exit /b
)

w32tm /config /manualpeerlist:"pool.ntp.org" /syncfromflags:manual /update > NUL
w32tm /resync > NUL

call :add_log "Time is synced to the %ntp_server% NTP server:"

call :add_log "Process:"
tasklist >> %log_file_path%

if defined process_to_kill (
    taskkill /im %process_to_kill% > NUL 2>&1

    if !ERRORLEVEL! == 0 (
        call :add_log "Process %process_to_kill% was successfully killed!"
    ) else (
        if !ERRORLEVEL! == 128 (
            call :add_log "Process %process_to_kill% was not found!"
        )
    )
)

if defined base_path (
    if exist %base_path% (
        for /f %%A in ('dir /b /a-d "%folder_path%" ^| find /c /v ""') do set "file_count_before=%%A"

        del *.tmp > NUL 2>&1
        del temp* > NUL 2>&1

        for /f %%A in ('dir /b /a-d "%folder_path%" ^| find /c /v ""') do set "file_count_after=%%A"

        set /a files_deleted=!file_count_before!-!file_count_after!
        
        call :add_log "Temporary files deleted: !files_deleted!"

        if exist %archive_path% (
            set datetime="%date%_%time%.zip"
            set valid_archive_name=!datetime::=-!

            7z a !valid_archive_name! "%base_path%/*.*" > NUL

            move !valid_archive_name! %archive_path% > NUL

            call :add_log "Created an archive at "!archive_path!!valid_archive_name!""
        )
    )
)

exit /b

:: functions

:add_log

if not exist "%log_file_path%" (
    echo [%date% %time%] Log file "%log_file_path%" was created! >> "%log_file_path%"
)

if not "%~1" == "" (
    echo [%date% %time%] %~1 >> "%log_file_path%"
)

if not "%~2" == "" (
    echo %~2 >> "%log_file_path%"
)

goto :EOF
