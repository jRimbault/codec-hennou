cmake_minimum_required(VERSION 2.8)

project(codec)

add_executable(
	codec
	../src/main.c
)

target_link_libraries(
	codec
	pthread
	m
)
