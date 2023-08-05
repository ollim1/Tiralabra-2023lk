# Testing document

## Tested items
- functionality of algorithm components, e.g. PriorityQueue
- effectiveness of algorithms (

## Unit testing
Unit tests are implemented using the Check framework. They cover expected basic functionality like adding to a queue or buffer and struct initialization.

Run unit tests with the command `make check`. For this, the package `check` should be installed from your distribution's package manager.

## Comparison of algorithms
Comparison testing is implemented using random input and output and averaged over multiple iterations. This can be accessed with the argument `-t`

During testing the primary metric of comparison is compression ratio of input.
