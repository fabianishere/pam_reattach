pam\_reattach
[![Build Status](https://travis-ci.org/fabianishere/pam_reattach.svg?branch=master)](https://travis-ci.org/fabianishere/pam_reattach)
=============
This is a PAM module for reattaching to the authenticating user's per-session
bootstrap namespace on macOS (See [TN2083](https://developer.apple.com/library/archive/technotes/tn2083/_index.html) about bootstrap namespace). 
This allows users to make use of the `pam_tid` module (Touch ID) from within tmux.

## Usage
This module should be invoked before the module that you want to put in the
authenticating user's per-session bootstrap namespace. The module runs in the
authentication phase and should be marked as either `optional` or `required`
(I suggest using `optional` to prevent getting locked out due to possible bugs)

```
auth     optional     pam_reattach.so
auth     sufficient   pam_tid.so
...
```

Make sure you have the module installed.

## Installation
The module is built using [CMake](https://cmake.org). Enter the following
commands into your command prompt in the directory in which you intend to build
the module:

```sh
$ cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr <PATH-TO-SOURCE>
$ make
$ sudo make install
```


## License
The code is released under the MIT license. See the `LICENSE.txt` file.
