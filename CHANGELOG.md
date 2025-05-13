# Changelog

## [1.8.0] - 2025-04-27
### Added
- **( scripts: build.sh )** Refactored build.sh and added new functions like building archives and improved flags support.
- **( repo )** Added CHANGELOG.md, and scripts/new.py.
- **( scripts: build.sh )** Fix `--build-all` in build.sh and `--default`, `--default-devel` flags.
### Changed
- **( doxygen: Doxyfile )** Updated Doxyfile, for displaying README.md as main page.
### Fixed
- **( scripts: build.sh )** Fixed `--build-all` in build.sh.

## [1.8.0] - 2025-05-10
### Fixed
- **[ Types: Any.hpp ]** Improved the VaAny class to no longer use the C++23-deprecated std::aligned_storage.
- **[ FuncTools: Func.hpp ]** Improved the VaFunc class to no longer use the C++23-deprecated std::aligned_storage.
### Added
- **[ FuncTools: Method.hpp ]** Added a new `VaMethod` class for storing class methods. Minor fixes in `VaFunc`.
- **[ FuncTools: TypeWrapper.hpp ]** Added VaTypeWrapper that wraps a function into a struct with `operator()`.
- **[ FuncTools: TypeWrapper.hpp ]** Added unwrapFunc.

## [1.9.0] - 2025-05-10
### Added
- **[ Types: List.hpp ]** Added UnsafeTake method into VaList class.
- **( scripts: new.py )** Added new options and formatting to new.py
- **( Types: LinkedList.hpp )** Added doxygen documentation in `VaLinkedList`.
## [1.9.0] - 2025-05-13
### Added
- **( scripts: version.py )** Added better version management with `scripts/version.py`.
