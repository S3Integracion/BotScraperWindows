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

:: Eliminar tarea AutoScrap1.exe si existe
schtasks /query /tn "AutoScrap1.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: AutoScrap1.exe
    schtasks /delete /tn "AutoScrap1.exe" /f
)

:: Eliminar tarea AutoScrap2.exe si existe
schtasks /query /tn "AutoScrap2.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: AutoScrap2.exe
    schtasks /delete /tn "AutoScrap2.exe" /f
)

:: Eliminar tarea AutoScrap3.exe si existe
schtasks /query /tn "AutoScrap3.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: AutoScrap3.exe
    schtasks /delete /tn "AutoScrap3.exe" /f
)

:: Eliminar tarea AutoScrap4.exe si existe
schtasks /query /tn "AutoScrap4.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: AutoScrap4.exe
    schtasks /delete /tn "AutoScrap4.exe" /f
)

:: ===============================================
:: CREACION DE TAREAS PROGRAMADAS
:: ===============================================
echo Creando tareas programadas con el usuario actual: %USERNAME%...

:: Lunes - Bot_Lunes.exe (Activo)
schtasks /create /sc weekly /d MON /st 00:15 /tn "Bot_Lunes.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Lunes.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: Martes - Bot_Martes.exe (Activo)
schtasks /create /sc weekly /d TUE /st 00:15 /tn "Bot_Martes.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Martes.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: Miércoles - Bot_Miercoles.exe (Deshabilitado inicialmente)
schtasks /create /sc weekly /d WED /st 00:15 /tn "Bot_Miercoles.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Miercoles.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

echo.
echo ===============================================
echo Tareas creadas correctamente con el usuario actual: %USERNAME%
echo ===============================================
pause
