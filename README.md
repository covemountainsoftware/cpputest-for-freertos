# CppUTest for FreeRTOS

Build and Test status: ![Build and Tests](https://github.com/covemountainsoftware/cpputest-for-freertos/actions/workflows/cmake.yml/badge.svg)

Copyright Matthew Eshleman. 

To learn more about Matthew, see here: https://covemountainsoftware.com/services/consulting/

# Introduction

CppUTest for FreeRTOS is a host PC based unit testing library supporting
the unit testing of well-formed FreeRTOS based code. Fundamentally it provides
various FreeRTOS functions as non-functional linking only replacements OR 'fakes'
that provide equivalent functionality (especially see timers).

What do I mean by well-formed? Well-formed in this context means a thread or 
module with only a single thread blocking mechanism (queue or semaphore, for example)
which in turn, when unblocked, activates the thread's behavior.
A well-known example would be an [active object](https://covemountainsoftware.com/2021/04/20/what-is-an-active-object/).

Frankly, a firmware's threads should be well-formed. Like many others, 
I consider the mixing of RTOS blocking and locking mechanism's to be
a recipe for a bad-cake, if not disaster.

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

This project uses GitHub Actions to build and execute all
unit tests found in this project. This is an example of one of the 
key benefits of host-based testing of embedded software.

See the configuration at: `.github/workflows/cmake.yml`

# Examples

## Button Service

The button service is effectively an active object style FreeRTOS service (thread)
which blocks exclusively on a semaphore. It is provided as an example of
how to use this library to unit test a thread that pends on a semaphore.

See `example/services/buttonService/test` for the associated unit tests.

## Hw Lock Ctrl Service

The Hw Lock Ctrl Service is a traditional active object FreeRTOS service (thread)
which blocks exclusively on a queue. It is provided as an example of 
how to use this library to unit test an active object thread.

See `example/services/hwLockCtrlService/test` for the associated unit tests.

## demoPcApp

A POSIX port FreeRTOS demonstration app interacting with the example services in a 
real FreeRTOS environment.

See `example\apps\demoPcApp`.

# FreeRTOS Support

## Tasks

Non-functional FreeRTOS task related methods are provided for linking purposes only.
The library assumes that the unit under test will avoid actual threading
behavior while unit testing. i.e. test the code executed by a thread, NOT 
threading behavior itself.

## Queues

The library provides fake but functional FreeRTOS compatible queues. The queues
do not block in any manner.

## Queue Sets

The library provides fake but functional FreeRTOS compatible queue sets. 
The queues sets do not block in any manner and operate as expected with 
queues and semaphores. NOTE: provided for projects that may have a 
real need for this FreeRTOS feature. However, FreeRTOS is not encouraging
this feature and neither would I encourage the use of this feature. Too 
many caveats and potential maintenance issues.

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
