[![Build Status](https://travis-ci.org/nar-org/libnar.png?branch=master)](https://travis-ci.org/nar-org/libnar)
[![BSD](http://b.repl.ca/v1/license-BSD-blue.png)](http://en.wikipedia.org/wiki/BSD\_licenses)

# LIBNAR: C++14

This C++ library implements the [NAR](http://github.com/nar-org/nar-specs) specification version 1.

## Installation

### Prerequisites

* GCC-5.x or clang-3.5
* cmake 3.0
* libgtest (for the tests only)
* libpthread (for the tests only)
* libboost-filesystem (for the command line only)
* libboost-program-options (for the command line only)

### Build

```Bash
mkdir Build
cd Build
cmake ..
make
```
