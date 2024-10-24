# CppUTest for FreeRTOS

Build and Test status: ![Build and Tests](https://github.com/covemountainsoftware/cpputest-for-freertos/actions/workflows/cmake.yml/badge.svg)

Copyright Matthew Eshleman

# Introduction

TODO

# Environment

This project was developed and proven in Ubuntu 22.04. In theory any
build or host operating system environment supported by CppUTest 4.0 will
be compatible with this code.

## Prerequisites

* CMake and associated build tools were used to develop
  and prove out this project.
* CppUTest (version 4.0)
* This project requires support for C++14 and C11.

## Continuous Integration

This project has configured GitHub Actions to build and execute all
unit tests found in this project. This is an example
of one of the key benefits of host-based testing of embedded software.

See the configuration at: `.github/workflows/cmake.yml`


# Testing Support

TODO


# License

All code in this project found in the `cms` namespace follows a dual-license approach.
Please see LICENSE.txt for details.

All licenses for external source code and libraries relied upon by this project
remain fully owned by their respective owners. 


# References

This project was also inspired by a generic example, see this blog post:
https://covemountainsoftware.com/2020/04/17/unit-testing-active-objects-and-state-machines/

Additionally, please see that post's associated github repo:
https://github.com/covemountainsoftware/activeObjectUnitTestingDemo

Other references:
* Sutter, Herb. Prefer Using Active Objects Instead of Naked Threads. Dr. Dobbs, June 2010.
* Grenning, James. Test Driven Development for Embedded C.
