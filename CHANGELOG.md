# Changelog

## [1.6.0] - 2025-04-27
### Added
- **( scripts: build.sh )** Refactored build.sh and added new functions like building archives and improved flags support.
- **( repo )** Added CHANGELOG.md, and scripts/new.py.
- **( scripts: build.sh )** Fix `--build-all` in build.sh and `--default`, `--default-devel` flags.
### Changed
- **( doxygen: Doxyfile )** Updated Doxyfile, for displaying README.md as main page.
### Fixed
- **( scripts: build.sh )** Fixed `--build-all` in build.sh.

## [1.6.1] - 2025-05-10
### Fixed
- **[ Types: Any.hpp ]** Improved the VaAny class to no longer use the C++23-deprecated std::aligned_storage.
- **[ FuncTools: Func.hpp ]** Improved the VaFunc class to no longer use the C++23-deprecated std::aligned_storage.
### Added
- **[ FuncTools: Method.hpp ]** Added a new `VaMethod` class for storing class methods. Minor fixes in `VaFunc`.
- **[ FuncTools: TypeWrapper.hpp ]** Added VaTypeWrapper that wraps a function into a struct with `operator()`.
- **[ FuncTools: TypeWrapper.hpp ]** Added unwrapFunc.

## [1.6.2] - 2025-05-10
### Added
- **[ Types: List.hpp ]** Added UnsafeTake method into VaList class.
- **( scripts: new.py )** Added new options and formatting to new.py
- **( docs: LinkedList.hpp )** Added doxygen documentation in `VaLinkedList`.

## [1.7.1] - 2025-05-13
### Added
- **( scripts: version.py )** Added better version management with `scripts/version.py`.

## [1.7.2] - 2025-05-14
### Changed
- **( scripts: new.py )** Refactored new.py script.

## [1.7.3] - 2025-05-18
### Added
- **[ RawAccess ]** Added RawAccess module.
- **[ RawAccess: List.hpp ]** Added VaListRawView.
- **[ RawAccess: LinkedList.hpp ]** Added VaLinkedListRawView.
- **[ RawAccess: Dict.hpp ]** Added VaDictRawView.

## [1.7.4] - 2025-05-18
### Added
- **[ Types: List.hpp ]** Added `getRawView()` and `getUnsafeAccess()` methods. (`VaList::getRawView()`:[List.hpp](./Include/Types/List.hpp):`1409`, `VaList::getUnsafeAccess()`:[List.hpp](./Include/Types/List.hpp):`1427`)
- **[ Types: LinkedList.hpp ]** Added `getRawView()` and `getUnsafeAccess()` methods. (`VaLinkedList::getRawView()`:[LinkedList.hpp](./Include/Types/LinkedList.hpp):`1015`, `VaLinkedList::getUnsafeAccess()`:[LinkedList.hpp](./Include/Types/LinkedList.hpp):`1029`)
- **[ Types: Dict.hpp ]** Added `getRawView()` and `getUnsafeAccess()` methods.
### Fixes
- **[ Types: String.cpp ]** Fixed memory leak in operator+=(VaString&& other), occurring when the other was empty (`VaString::operator+=`:[String.cpp](./src/Types/String.cpp):`129`)

## [1.7.5] - 2025-05-21
### Added
- **[ Mem: UniquePtr.hpp ]** Added VaUniquePtr class.
- **[ Mem: SharedPtr.hpp ]** Added VaSharedPtr class.
- **[ Mem: WeakPtr.hpp ]** Added VaWeakPtr class.
- **[ Mem: UniquePtr.hpp ]** Added specializations for VaUniquePtr arrays (`VaUniquePtr<T[]>`:[UniquePtr.hpp](./Include/VaLib/Mem/UniquePtr.hpp):`159`)

## [1.7.6] - 2025-05-22
### Added
- **[ Mem: SharedPtr.hpp ]** Added specializations for the VaSharedPtr arrays (`VaSharedPtr<T[]>`:[SharedPtr.hpp](./Include/VaLib/Mem/SharedPtr.hpp):`225`)
- **[ Mem: WeakPtr.hpp ]** Added specializations for the VaWeakPtr arrays (`VaWeakPtr<T[]>`:[WeakPtr.hpp](./Include/VaLib/Mem/WeakPtr.hpp):`140`)
- **( testing: TestSharedPtr.cpp )** VaUniquePtr test.
- **( testing: TestWeakPtr.cpp )** VaUniquePtr test.
- **( testing: TestUniquePtr.cpp )** VaUniquePtr test.

## [1.8.0] - 2025-05-25
### Added
- **[ Types: List.hpp ]** Added appendEach, prependEach, insertEach and appendAll, prependAll, insertAll methods.
- **[ Types: LinkedList.hpp ]** Added appendEach, prependEach, insertEach and appendAll, prependAll, insertAll methods.
- **[ Types: LinkedChunkedList.hpp ]** Added VaLinkedChunkedList class.
- **( testing: TestLinkedChunkedList.cpp )** Added TestLinkedChunkedList.
- **[ Types: Dict.hpp ]** Added some new utils for VaDict class.
- **( testing: TestDict.cpp )** Updated test for VaDict.
### Fixed
- **( scripts: )** Fixed reading C++ compiler flags.
### Deprecated
- **[ Types: Dict.hpp ]** VaDict::remove is now deprecated. Use VaDict::del instanted.
