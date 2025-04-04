@echo off

setlocal enabledelayedexpansion

set email_file_path="email.txt"
set ntp_server="pool.ntp.org"

set log_file_path=%1
set base_path=%2
set process_to_kill=%3
set archive_path=%4

if "%log_file_path%"=="" (
    echo "Usage: <log_file_name> [base_path] [process_to_kill] [archive_folder]"
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
            call :add_log "Process %process_to_kill% was not found^^^!"
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

for /f %%a in ('powershell -nologo -noprofile -command "[datetime]::Today.AddDays(-1).ToString('MM.dd.yyyy')"') do set YESTERDAY=%%a

set found=false

for %%F in (!archive_path!%YESTERDAY%*) do (
    set found=true
)

if "!found!"=="false" (
    call :send_email "No archives found for %YESTERDAY%^^^!"

    call :add_log "An email was send to the admin notifying him that there are no archive backups for the previous day: %YESTERDAY%"
)

set file_count_before=0
set file_count_after=0

for /f %%A in ('dir /b /a-d "%archive_path%" ^| find /c /v ""') do set "file_count_before=%%A"

forfiles /p "%archive_path%" /m *.zip /d 30 /c "cmd /c del /q @path" > NUL 2>&1

if errorlevel 1 (
    call :add_log No old archives found.
) else (
    set /a files_deleted=!file_count_before!-!file_count_after!
    call :add_log "Old archives deleted: !files_deleted!."
)

for /f %%A in ('dir /b /a-d "%archive_path%" ^| find /c /v ""') do set "file_count_after=%%A"

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

:send_email

if not exist "%email_file_path%" (
    break>%email_file_path%
)

echo %~1 >> %email_file_path%
echo Sent: %date%%time% >> %email_file_path%
echo. >> %email_file_path%

goto :EOF
