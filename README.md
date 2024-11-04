# CppUTest for FreeRTOS

Build and Test status: ![Build and Tests](https://github.com/covemountainsoftware/cpputest-for-freertos/actions/workflows/cmake.yml/badge.svg)

Copyright Matthew Eshleman. 

To learn more about Matthew, see here: https://covemountainsoftware.com/services/consulting/

# Introduction

CppUTest for FreeRTOS is a host PC based unit testing library supporting
the unit testing of FreeRTOS based code.

At this time, the library is proven for firmware following an event driven 
active object pattern, i.e. threads that sit on a queue and process one event 
at a time. Other patterns may be possible too, as long as the module under test
does not mix blocking operating system calls within a function being tested. See
the provided example which provides a back-door for processing a single event,
which in-turn enables host PC based unit testing.

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

## Tasks

Non-functional FreeRTOS task related methods are provided for linking purposes only.
The library assumes that the unit under test will avoid actual threading
behavior while unit testing. i.e. test the code executed by a thread, NOT 
threading behavior itself.

## Queues

The library provides fake but functional FreeRTOS compatible queues. The queues
do not block in any manner.

## Timers

The library provides fake but functional FreeRTOS compatible software timers.
Accessor methods are provided allowing unit tests to "move time forward,"
triggering FreeRTOS timers to fire as expected.

## ASSERT

The library provides a configured "configASSERT" macro for asserts compatible
with the FreeRTOS assert method. The assert is mocked and helper methods 
are provided to enable unit testing of assert behavior.

## Delay

Basic vTaskDelay provided, which will coordinate with the fake timers when used.
Keeps APIs such xTaskGetTickCount coordinated with fake timers or alternate tick count
when timers are not being used.

## Semaphores

Available. The provided fake semaphores do not block, just like the queues.
See the example ButtonService and its unit tests for a FreeRTOS thread 
exclusively blocking on a semaphore, triggered from an ISR.

## Mutexes

Available. The provided fake mutexes do not block, just like the semaphores and queues.
Additionally, when a unit test is completed, the library confirms that all mutex 
have been either deleted OR are unlocked. i.e. ensure that a particular test does
not leave any mutexes in a locked state accidentally.

## Direct to task notifications

TODO.

## Stream buffers

TODO.

## Message buffers

TODO.

## Event groups

TODO.

# License

All code in this project found in the `cms` namespace follows a dual-license approach.
Please see LICENSE.txt for details.

All licenses for external source code and libraries relied upon by this project
remain fully owned by their respective owners. 

# References

This project was inspired by a generic example, see this blog post:
https://covemountainsoftware.com/2020/04/17/unit-testing-active-objects-and-state-machines/

Additionally, please see that post's associated GitHub repo:
https://github.com/covemountainsoftware/activeObjectUnitTestingDemo

Other references:
* Sutter, Herb. Prefer Using Active Objects Instead of Naked Threads. Dr. Dobbs, June 2010.
* Grenning, James. Test Driven Development for Embedded C.
