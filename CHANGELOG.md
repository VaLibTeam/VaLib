# Changelog

## [1.8.0] - 2025-04-27
### Added
- Refactored build.sh and added new functions like building archives and improved flags support.
- Added CHANGELOG.md, and scripts/new.py.
- Fix '--build-all' in build.sh and --default, --default-devel flags.
### Changed
- Updated Doxyfile, for displaying README.md as main page.
### Fixed
- Fixed --build-all in build.sh.
## [1.8.0] - 2025-05-10
### Fixed
- Improved the VaAny and VaFunc classes to no longer use the C++23-deprecated std::aligned_storage.
### Added
- Added a new `VaMethod` class for storing class methods. Minor fixes in `VaFunc`.
- Added VaTypeWrapper that wraps a function into a struct with operator().
- Added unwrapFunc.