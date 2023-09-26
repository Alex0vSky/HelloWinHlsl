# _HelloWinHlsl_
[![windows-os](https://img.shields.io/badge/platform-windows-blue?logo=Windows)](
https://en.wikipedia.org/wiki/Microsoft_Windows
) [![cpp-logo](https://img.shields.io/badge/C%2B%2B-v17-blue?logo=cplusplus)](
https://en.wikipedia.org/wiki/C++
) [![directx-logo](https://img.shields.io/badge/DirectX-9,10,11,12-blue.svg?logo=data:image/svg%2bxml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZlcnNpb249IjEiIHdpZHRoPSI2MDAiIGhlaWdodD0iNjAwIj48cGF0aCBkPSJNMTI5IDExMWMtNTUgNC05MyA2Ni05MyA3OEwwIDM5OGMtMiA3MCAzNiA5MiA2OSA5MWgxYzc5IDAgODctNTcgMTMwLTEyOGgyMDFjNDMgNzEgNTAgMTI4IDEyOSAxMjhoMWMzMyAxIDcxLTIxIDY5LTkxbC0zNi0yMDljMC0xMi00MC03OC05OC03OGgtMTBjLTYzIDAtOTIgMzUtOTIgNDJIMjM2YzAtNy0yOS00Mi05Mi00MmgtMTV6IiBmaWxsPSIjZmZmIi8+PC9zdmc+)](
https://en.wikipedia.org/wiki/DirectX
)

[![Qa test and coverage](https://github.com/Alex0vSky/HelloWinHlsl/actions/workflows/qa-UnitTest-Coverage.yml/badge.svg)](
https://github.com/Alex0vSky/HelloWinHlsl/actions/workflows/qa-UnitTest-Coverage.yml
)

[![cpplint](https://gist.githubusercontent.com/Alex0vSky/f15941c4c8858c6209ad8800fa810539/raw/GoogleStyle_cpplint.svg)](
https://Alex0vSky.github.io/project-qa-report/HelloWinHlsl/cpplint.xml
) [![cppcheck](https://gist.githubusercontent.com/Alex0vSky/f15941c4c8858c6209ad8800fa810539/raw/StaticAnalysis_cppcheck.svg)](
https://Alex0vSky.github.io/project-qa-report/HelloWinHlsl/cppcheck.xml
) [![LinesOfСode](https://gist.githubusercontent.com/Alex0vSky/f15941c4c8858c6209ad8800fa810539/raw/Metrixpp-LinesOfСode.svg)](
https://Alex0vSky.github.io/project-qa-report/HelloWinHlsl/metrixpp.txt
) [![Comments](https://gist.githubusercontent.com/Alex0vSky/f15941c4c8858c6209ad8800fa810539/raw/Metrixpp-Comments.svg)](
https://Alex0vSky.github.io/project-qa-report/HelloWinHlsl/metrixpp.txt
) [![amount testsuites](https://gist.githubusercontent.com/Alex0vSky/f15941c4c8858c6209ad8800fa810539/raw/GoogleTest-testsuites-Windows-x64-Debug.svg)](
https://Alex0vSky.github.io/project-qa-report/HelloWinHlsl/GoogleTestCombinedOutput/index.html
) [![tests coverage](https://gist.githubusercontent.com/Alex0vSky/f15941c4c8858c6209ad8800fa810539/raw/TestsCoverage-Occ-Windows-x64-Debug.svg)](
https://Alex0vSky.github.io/project-qa-report/HelloWinHlsl/HtmlReportOcc/index.html
)

Windows DirectX Hlsl shader C++ library

- [x] Dx9
- [x] Dx10
- [x] Dx11
- [x] Dx12
- [x] get rid of dependence on the boost::pfr library
- [x] get rid of dependence on the DirectXTK12 library

## Features

## Requirements
WindowsSdk, DirectX

## Install

## Usage

## Tests
### CI github actions
+ qa UnitTest and coverage: _google-test_, _OpenCppCoverage/OpenCppCoverage_;
+ qa staticAnalysis and codeMetrics: _cppcheck_, py _metrixpp_, py _cpplint_;
+ auxiliary: py _junitparser_, py _burlachenkok/gtest_report_(fork).

## Build
Building the project and tests from the source code requires the presence of the __Microsoft Visual Studio 2019 Community__, and using *.sln and *.vcxproj project files.

## Contributing
Can ask questions. PRs are accepted. No requirements for contributing.

## Thanks

## License
See the [LICENSE](https://github.com/Alex0vSky/HelloWinHlsl/blob/main/LICENSE) file for license rights and limitations (MIT).
