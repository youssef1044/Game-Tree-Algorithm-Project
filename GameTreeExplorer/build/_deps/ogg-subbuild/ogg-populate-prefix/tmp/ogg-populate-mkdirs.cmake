# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-src"
  "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-build"
  "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-subbuild/ogg-populate-prefix"
  "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-subbuild/ogg-populate-prefix/tmp"
  "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp"
  "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-subbuild/ogg-populate-prefix/src"
  "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/studies/year 2/Semester 2/Analysis of algorithms/Project/Project/Game-Tree-Algorithm-Project/GameTreeExplorer/build/_deps/ogg-subbuild/ogg-populate-prefix/src/ogg-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
