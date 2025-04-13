# VaLib
**VaLib** *(short for Vast Library)* is a C++ library in early beta that provides a rich set of modern types and utility functions.

## Table of Contents
- [Overview](#overview)
- [Building](#building)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [Features](#features)
- [License](#license)

## Overview
**VaLib** is a modern and extensible C++ library, currently in early beta. It offers a wide variety of useful types, functions, and abstractions to streamline development and enhance productivity.

## Building
Building VaLib is simple — we provide a tool that handles everything for you!

> [!NOTE]
> VaLib currently has no external dependencies beyond `libstdc++`, so you don't need to install anything extra.

To build the library, just run:

```sh
./build.sh --target={target}
```

Replace `{target}` with the desired output type. Available options:
- **static** – builds a static library (`.a`)
- **shared** – builds a shared library (`.so`)
- **object** – builds a single object file (`.o`)
- **all** – builds all of the above: static, shared, and object

The resulting files will be placed in the `out/` directory.

To explore all available build options, run:

```sh
./build.sh --help
```

## Installation
To install VaLib, you can use the same build tool as above.
To install the library:

```sh
./build.sh --target={target} --install-libs
```

Valid `{target}` values:
- **static** – builds and installs the static library (`.a`)
- **shared** – builds and installs the shared library (`.so`)
- **object** – builds an object file (`.o`); installation not supported
- **all** – builds and installs both static and shared libraries, plus the object file

To install the header files and make VaLib available for use in your own projects, run:

```sh
./build.sh --install-headers
```

> [!WARNING]
> The `--install-libs` and `--install-headers` options require root privileges. Use `sudo` or switch to the root account to perform installation.

## Usage
To learn how to use VaLib, check out our documentation, available online at [VaLib Documentation](https://VaLibTeam.github.io) under the **Docs** section.

If you prefer having the documentation locally, you can generate it using **Doxygen**:

```sh
doxygen Doxyfile
```

The documentation will be generated in HTML format under `docs/html/index.html`.

## Contributing
We welcome contributions! If you'd like to contribute to VaLib, please read our [contributing guidelines](CONTRIBUTING.md) (coming soon!) and feel free to open issues or pull requests.

## Features
### 🚀 Modern C++ Features
VaLib leverages the power of modern C++ standards like [C++20](https://en.cppreference.com/w/cpp/20) and [C++23](https://en.cppreference.com/w/cpp/23) to provide clean, readable, and efficient code. Some of the features used include:

- Concepts
- Variadic templates
- Smart pointers
- And much more!

### ⚡ High Performance
VaLib is designed from the ground up with performance in mind. Your applications can remain lightweight and fast, even when using advanced features.

> [!TIP] 
> You can run our benchmark suite using the `test.sh` script to evaluate VaLib’s performance.

## License
VaLib is licensed under the [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html). You can read the full license [here](https://www.gnu.org/licenses/gpl-3.0.html) or in the `LICENSE` file included in this repository.