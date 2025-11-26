# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/pedro/esp/esp-idf/components/bootloader/subproject"
  "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader"
  "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader-prefix"
  "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader-prefix/tmp"
  "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader-prefix/src/bootloader-stamp"
  "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader-prefix/src"
  "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/pedro/Documents/Security-Chat/secure-chat-server/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
