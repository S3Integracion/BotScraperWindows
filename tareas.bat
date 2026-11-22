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
:: (incluye las tareas antiguas con nombres de días)
:: ===============================================
echo Eliminando tareas programadas existentes si existen...

schtasks /query /tn "Bot_Lunes.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Lunes.exe
    schtasks /delete /tn "Bot_Lunes.exe" /f
)

schtasks /query /tn "Bot_Martes.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Martes.exe
    schtasks /delete /tn "Bot_Martes.exe" /f
)

schtasks /query /tn "Bot_Miercoles.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Miercoles.exe
    schtasks /delete /tn "Bot_Miercoles.exe" /f
)

schtasks /query /tn "Bot_Jueves.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Jueves.exe
    schtasks /delete /tn "Bot_Jueves.exe" /f
)

schtasks /query /tn "Bot_Viernes.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Viernes.exe
    schtasks /delete /tn "Bot_Viernes.exe" /f
)

schtasks /query /tn "Bot_Sitemap1.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Sitemap1.exe
    schtasks /delete /tn "Bot_Sitemap1.exe" /f
)

schtasks /query /tn "Bot_Sitemap2.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Sitemap2.exe
    schtasks /delete /tn "Bot_Sitemap2.exe" /f
)

schtasks /query /tn "Bot_Sitemap3.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Sitemap3.exe
    schtasks /delete /tn "Bot_Sitemap3.exe" /f
)

schtasks /query /tn "Bot_Sitemap4.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Sitemap4.exe
    schtasks /delete /tn "Bot_Sitemap4.exe" /f
)

schtasks /query /tn "Bot_Sitemap5.exe" >nul 2>&1
if %errorlevel% equ 0 (
    echo Eliminando tarea existente: Bot_Sitemap5.exe
    schtasks /delete /tn "Bot_Sitemap5.exe" /f
)

:: ===============================================
:: CREACION DE TAREAS PROGRAMADAS
:: Un bot por sitemap (Bot_Sitemap1 = primer sitemap de la lista)
:: ===============================================
echo Creando tareas programadas con el usuario actual: %USERNAME%...

:: MON - Bot_Sitemap1.exe (sitemap 1)
schtasks /create /sc weekly /d MON /st 00:15 /tn "Bot_Sitemap1.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Sitemap1.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: TUE - Bot_Sitemap2.exe (sitemap 2)
schtasks /create /sc weekly /d TUE /st 00:15 /tn "Bot_Sitemap2.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Sitemap2.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: WED - Bot_Sitemap3.exe (sitemap 3)
schtasks /create /sc weekly /d WED /st 00:15 /tn "Bot_Sitemap3.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Sitemap3.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: THU - Bot_Sitemap4.exe (sitemap 4)
schtasks /create /sc weekly /d THU /st 00:15 /tn "Bot_Sitemap4.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Sitemap4.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

:: FRI - Bot_Sitemap5.exe (sitemap 5)
schtasks /create /sc weekly /d FRI /st 00:15 /tn "Bot_Sitemap5.exe" /tr "\"C:\Users\Public\Documents\BotScraperWindows\Bot_Sitemap5.exe\"" /ru "%USERNAME%" /rl HIGHEST /f

echo.
echo ===============================================
echo Tareas creadas correctamente con el usuario actual: %USERNAME%
echo ===============================================
pause
