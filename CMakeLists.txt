cmake_policy(SET CMP0048 NEW)

project(
  apollo
  LANGUAGES C CXX
  VERSION 0.0.1)

cmake_minimum_required(VERSION 3.16)

add_subdirectory(src/apollo)
