# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/vicente/esp/esp-idf/components/bootloader/subproject"
  "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader"
  "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader-prefix"
  "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader-prefix/tmp"
  "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader-prefix/src"
  "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/vicente/Desktop/IoT/CC5326-T3/codigo_esp/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
