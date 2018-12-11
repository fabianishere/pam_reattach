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
(I suggest using `optional` to prevent getting locked out in case of bugs)

Modify the targeted service in `/etc/pam.d/` (such as `/etc/pam.d/sudo`) as explained:
```
auth     optional     pam_reattach.so
auth     sufficient   pam_tid.so
...
```

Make sure you have the module installed. For further information, see
`reattach_aqua(3)`, `pam_reattach(8)` and `reattach-to-session-namespace(8)`.

## Installation
The module is built using [CMake](https://cmake.org). Enter the following
commands into your command prompt in the directory in which you intend to build
the module:

```bash
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/usr/local <PATH-TO-SOURCE>
$ make
$ sudo make install
```

## Deinstallation
Make sure you **keep** the generated `install_manifest.txt` file in the build folder
after installation. Run the following command in your command prompt to remove
the installation from your system:

```bash
$ sudo xargs rm < install_manifest.txt
```

In case you lost `install_manifest.txt`, this is a list of files that are
installed:
```
/usr/local/lib/libreattach.a
/usr/local/include/reattach.h
/usr/local/share/man/man3/reattach_aqua.3
/usr/local/lib/pam/pam_reattach.so
/usr/local/share/man/man8/pam_reattach.8
/usr/local/bin/reattach-to-session-namespace
/usr/local/share/man/man8/reattach-to-session-namespace.8
```

## Enabling Touch ID authorization for sudo
To enable Touch ID authorization for sudo, please see [this](https://derflounder.wordpress.com/2017/11/17/enabling-touch-id-authorization-for-sudo-on-macos-high-sierra/)
article.

## Additional Tools
Additionally, one can build a `reattach-to-session-namespace` command line
utility by specifying the `-DENABLE_CLI=ON` option when calling CMake. 

This command allows you to reattach to the user's session namespace from the
command line. See `reattach-to-session-namespace(8)`

## License
The code is released under the MIT license. See [LICENSE.txt](/LICENSE.txt).
