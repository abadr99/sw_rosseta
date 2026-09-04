# Vendored LIEF SDK

Official prebuilt LIEF 1.0.0 SDKs, checked in so CI and local builds do not compile LIEF from source.

| File | Platform | Source |
| --- | --- | --- |
| `LIEF-1.0.0-Linux-x86_64.tar.gz` | Linux x86_64 (glibc) | [LIEF 1.0.0 release](https://github.com/lief-project/LIEF/releases/tag/1.0.0) |
| `LIEF-1.0.0-win64.zip` | Windows x64 (MSVC, `/MT`) | same |

CMake unpacks the matching archive into the build directory and uses `find_package(LIEF)`.
