# Install script for directory: /home/luke/Documents/Projects/test-networks

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/luke/Documents/Projects/scspkg/packages/chi_nettest")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench"
         RPATH "/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/bzip2-1.0.8-hoy47ejhkn54yrjve52itdwhav72rgaw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lzo-2.10-rmgck6agsxmbn2opauuc5mvn6n2ajcjw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zstd-1.5.6-gyzu7edy7b2vruukmtxbxlcmiakinsnb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lz4-1.9.4-piqur6dwee3f2zw3p6hsbnbp4xhadib3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zlib-1.3.1-4tmckgxn4ftjzv4wifl4oo5pteaxrxdc/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/xz-5.4.6-wa7kwighypm4yjxxz5jpddbr6paglyni/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/brotli-1.1.0-ofxoh2pqbljpy4tvav7ygabvkjvl3br3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/snappy-1.1.10-xt7rxz23xsxdyzphg7p3mbvs5bi4z4s4/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/c-blosc2-2.11.1-hexh27bwf5u4sihpuoibnnrkazdqwuxb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/yaml-cpp-0.7.0-yap357umhi3lved7hwsfjchc4nrbb7ib/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/openmpi-5.0.3-c2uqih6ry5zotjxkgo3tgzxwude6qiqf/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mochi-margo-0.15.0-mnnircguh3r36uwckal76c6gitdxnrzt/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mercury-2.3.1-hk4lz55uw6due7x2fygusqi737oyean2/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/argobots-1.1-xl2ttdn3pmnkmszaytszwihzaq7u3xj3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/json-c-0.16-uyb5xmlt2jjkx3eztc5uy3b3hs4r4awe/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin" TYPE EXECUTABLE FILES "/home/luke/Documents/Projects/test-networks/build/bin/chi_net_bench")
  if(EXISTS "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench"
         OLD_RPATH "/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/bzip2-1.0.8-hoy47ejhkn54yrjve52itdwhav72rgaw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lzo-2.10-rmgck6agsxmbn2opauuc5mvn6n2ajcjw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zstd-1.5.6-gyzu7edy7b2vruukmtxbxlcmiakinsnb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lz4-1.9.4-piqur6dwee3f2zw3p6hsbnbp4xhadib3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zlib-1.3.1-4tmckgxn4ftjzv4wifl4oo5pteaxrxdc/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/xz-5.4.6-wa7kwighypm4yjxxz5jpddbr6paglyni/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/brotli-1.1.0-ofxoh2pqbljpy4tvav7ygabvkjvl3br3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/snappy-1.1.10-xt7rxz23xsxdyzphg7p3mbvs5bi4z4s4/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/c-blosc2-2.11.1-hexh27bwf5u4sihpuoibnnrkazdqwuxb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/yaml-cpp-0.7.0-yap357umhi3lved7hwsfjchc4nrbb7ib/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/openmpi-5.0.3-c2uqih6ry5zotjxkgo3tgzxwude6qiqf/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mochi-margo-0.15.0-mnnircguh3r36uwckal76c6gitdxnrzt/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mercury-2.3.1-hk4lz55uw6due7x2fygusqi737oyean2/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/argobots-1.1-xl2ttdn3pmnkmszaytszwihzaq7u3xj3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/json-c-0.16-uyb5xmlt2jjkx3eztc5uy3b3hs4r4awe/lib:"
         NEW_RPATH "/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/bzip2-1.0.8-hoy47ejhkn54yrjve52itdwhav72rgaw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lzo-2.10-rmgck6agsxmbn2opauuc5mvn6n2ajcjw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zstd-1.5.6-gyzu7edy7b2vruukmtxbxlcmiakinsnb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lz4-1.9.4-piqur6dwee3f2zw3p6hsbnbp4xhadib3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zlib-1.3.1-4tmckgxn4ftjzv4wifl4oo5pteaxrxdc/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/xz-5.4.6-wa7kwighypm4yjxxz5jpddbr6paglyni/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/brotli-1.1.0-ofxoh2pqbljpy4tvav7ygabvkjvl3br3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/snappy-1.1.10-xt7rxz23xsxdyzphg7p3mbvs5bi4z4s4/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/c-blosc2-2.11.1-hexh27bwf5u4sihpuoibnnrkazdqwuxb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/yaml-cpp-0.7.0-yap357umhi3lved7hwsfjchc4nrbb7ib/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/openmpi-5.0.3-c2uqih6ry5zotjxkgo3tgzxwude6qiqf/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mochi-margo-0.15.0-mnnircguh3r36uwckal76c6gitdxnrzt/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mercury-2.3.1-hk4lz55uw6due7x2fygusqi737oyean2/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/argobots-1.1-xl2ttdn3pmnkmszaytszwihzaq7u3xj3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/json-c-0.16-uyb5xmlt2jjkx3eztc5uy3b3hs4r4awe/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_bench")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/luke/Documents/Projects/test-networks/build/CMakeFiles/chi_net_bench.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping"
         RPATH "/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/bzip2-1.0.8-hoy47ejhkn54yrjve52itdwhav72rgaw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lzo-2.10-rmgck6agsxmbn2opauuc5mvn6n2ajcjw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zstd-1.5.6-gyzu7edy7b2vruukmtxbxlcmiakinsnb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lz4-1.9.4-piqur6dwee3f2zw3p6hsbnbp4xhadib3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zlib-1.3.1-4tmckgxn4ftjzv4wifl4oo5pteaxrxdc/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/xz-5.4.6-wa7kwighypm4yjxxz5jpddbr6paglyni/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/brotli-1.1.0-ofxoh2pqbljpy4tvav7ygabvkjvl3br3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/snappy-1.1.10-xt7rxz23xsxdyzphg7p3mbvs5bi4z4s4/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/c-blosc2-2.11.1-hexh27bwf5u4sihpuoibnnrkazdqwuxb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/yaml-cpp-0.7.0-yap357umhi3lved7hwsfjchc4nrbb7ib/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/openmpi-5.0.3-c2uqih6ry5zotjxkgo3tgzxwude6qiqf/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mochi-margo-0.15.0-mnnircguh3r36uwckal76c6gitdxnrzt/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mercury-2.3.1-hk4lz55uw6due7x2fygusqi737oyean2/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/argobots-1.1-xl2ttdn3pmnkmszaytszwihzaq7u3xj3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/json-c-0.16-uyb5xmlt2jjkx3eztc5uy3b3hs4r4awe/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin" TYPE EXECUTABLE FILES "/home/luke/Documents/Projects/test-networks/build/bin/chi_net_ping")
  if(EXISTS "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping"
         OLD_RPATH "/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/bzip2-1.0.8-hoy47ejhkn54yrjve52itdwhav72rgaw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lzo-2.10-rmgck6agsxmbn2opauuc5mvn6n2ajcjw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zstd-1.5.6-gyzu7edy7b2vruukmtxbxlcmiakinsnb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lz4-1.9.4-piqur6dwee3f2zw3p6hsbnbp4xhadib3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zlib-1.3.1-4tmckgxn4ftjzv4wifl4oo5pteaxrxdc/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/xz-5.4.6-wa7kwighypm4yjxxz5jpddbr6paglyni/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/brotli-1.1.0-ofxoh2pqbljpy4tvav7ygabvkjvl3br3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/snappy-1.1.10-xt7rxz23xsxdyzphg7p3mbvs5bi4z4s4/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/c-blosc2-2.11.1-hexh27bwf5u4sihpuoibnnrkazdqwuxb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/yaml-cpp-0.7.0-yap357umhi3lved7hwsfjchc4nrbb7ib/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/openmpi-5.0.3-c2uqih6ry5zotjxkgo3tgzxwude6qiqf/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mochi-margo-0.15.0-mnnircguh3r36uwckal76c6gitdxnrzt/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mercury-2.3.1-hk4lz55uw6due7x2fygusqi737oyean2/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/argobots-1.1-xl2ttdn3pmnkmszaytszwihzaq7u3xj3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/json-c-0.16-uyb5xmlt2jjkx3eztc5uy3b3hs4r4awe/lib:"
         NEW_RPATH "/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/bzip2-1.0.8-hoy47ejhkn54yrjve52itdwhav72rgaw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lzo-2.10-rmgck6agsxmbn2opauuc5mvn6n2ajcjw/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zstd-1.5.6-gyzu7edy7b2vruukmtxbxlcmiakinsnb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/lz4-1.9.4-piqur6dwee3f2zw3p6hsbnbp4xhadib3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/zlib-1.3.1-4tmckgxn4ftjzv4wifl4oo5pteaxrxdc/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/xz-5.4.6-wa7kwighypm4yjxxz5jpddbr6paglyni/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/brotli-1.1.0-ofxoh2pqbljpy4tvav7ygabvkjvl3br3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/snappy-1.1.10-xt7rxz23xsxdyzphg7p3mbvs5bi4z4s4/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/c-blosc2-2.11.1-hexh27bwf5u4sihpuoibnnrkazdqwuxb/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/yaml-cpp-0.7.0-yap357umhi3lved7hwsfjchc4nrbb7ib/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/openmpi-5.0.3-c2uqih6ry5zotjxkgo3tgzxwude6qiqf/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mochi-margo-0.15.0-mnnircguh3r36uwckal76c6gitdxnrzt/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/mercury-2.3.1-hk4lz55uw6due7x2fygusqi737oyean2/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/argobots-1.1-xl2ttdn3pmnkmszaytszwihzaq7u3xj3/lib:/home/luke/Documents/Projects/spack/opt/spack/linux-linuxmint22-zen2/gcc-13.2.0/json-c-0.16-uyb5xmlt2jjkx3eztc5uy3b3hs4r4awe/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/luke/Documents/Projects/scspkg/packages/chi_nettest/bin/chi_net_ping")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/luke/Documents/Projects/test-networks/build/CMakeFiles/chi_net_ping.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/luke/Documents/Projects/test-networks/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
