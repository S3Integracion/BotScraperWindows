@echo off
    echo Compilando...

    gcc Bot_Lunes.c -o Bot_Lunes.exe && gcc Bot_Martes.c -o Bot_Martes.exe &&  gcc Bot_Miercoles.c -o Bot_Miercoles.exe && gcc Bot_Jueves.c -o Bot_Jueves.exe && gcc Bot_Viernes.c -o Bot_Viernes.exe

echo Compilación completada.