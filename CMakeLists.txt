cmake_minimum_required(VERSION 4.2)
project(BotScraperWindows C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(BotScraperWindows
        Bot_Jueves.c
        Bot_Lunes.c
        Bot_Martes.c
        Bot_Miercoles.c
        Bot_Viernes.c)
