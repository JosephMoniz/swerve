cmake_minimum_required(VERSION 3.12)
project(swerve C)

set(CMAKE_C_STANDARD 99)

set(INCLUDE_FILES
        include/swerve/lexer/lex.h
        include/swerve/lexer/token.h
        include/swerve/lexer/token_queue.h include/swerve/lexer/token_type.h include/swerve/lexer/lexer_error.h include/swerve/lexer/lexer_result.h)

set(SOURCE_FILES
        src/main.c
        src/swerve/lexer/token.c
        src/swerve/lexer/token_queue.c src/swerve/lexer/token_type.c src/swerve/lexer/lex.c)

add_executable(swerve
        ${INCLUDE_FILES}
        ${SOURCE_FILES})

target_include_directories(swerve PUBLIC include)