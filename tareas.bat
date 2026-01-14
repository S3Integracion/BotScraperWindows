@echo off
:: ===============================================
:: BLOQUE DE AUTO-ELEVACION A ADMINISTRADOR
:: ===============================================
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo.
    echo Elevando privilegios de administrador...
    powershell -Command "Start-Process '%~f0' -Verb runAs"
    exit /b
)

:: ===============================================
:: ELIMINACIÓN DE TAREAS EXISTENTES
:: ===============================================
echo Eliminando tareas programadas existentes si existen...

:: Eliminar tarea Bot_Lunes.exe si existe
schtasks /query /tn "Bot_Lunes.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Lunes.exe
    schtasks /delete /tn "Bot_Lunes.exe" /f
)

:: Eliminar tarea Bot_Martes.exe si existe
schtasks /query /tn "Bot_Martes.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Martes.exe
    schtasks /delete /tn "Bot_Martes.exe" /f
)

:: Eliminar tarea Bot_Miercoles.exe si existe
schtasks /query /tn "Bot_Miercoles.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Miercoles.exe
    schtasks /delete /tn "Bot_Miercoles.exe" /f
)

:: Eliminar tarea Bot_Jueves.exe si existe
schtasks /query /tn "Bot_Jueves.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Jueves.exe
    schtasks /delete /tn "Bot_Jueves.exe" /f
)

:: ===============================================
:: CREACION DE TAREAS PROGRAMADAS
:: ===============================================
echo Creando tareas programadas con el usuario actual: %USERNAME%...

:: Lunes - Bot_Lunes.exe (Activo)
schtasks /create /sc weekly /d MON /st 00:15 /tn "Bot_Lunes.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Lunes.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: Martes - Bot_Martes.exe (Activo)
schtasks /create /sc weekly /d TUE /st 00:15 /tn "Bot_Martes.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Martes.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: Miércoles - Bot_Miercoles.exe (Activo)
schtasks /create /sc weekly /d WED /st 00:15 /tn "Bot_Miercoles.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Miercoles.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: Jueves - Bot_Jueves.exe (Activo)
schtasks /create /sc weekly /d THU /st 00:15 /tn "Bot_Jueves.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Jueves.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

echo.
echo ===============================================
echo Tareas creadas correctamente con el usuario actual: %USERNAME%
echo ===============================================
pause
