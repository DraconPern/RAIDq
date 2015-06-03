# README #

This README would normally document whatever steps are necessary to get your application up and running.

### What is this repository for? ###

This is proof-of-concept implementation of RAIDq.

RAIDq—a new way of extending RAID-6 to allow more checksum disks. 

For example, RAIDq can
allow up to 92 + 4 disks if we seek maximum coding speed, while
164 + 4 disks if we need to support larger disk arrays.

### How do I get set up? ###

Requirement: G++ and machine with AVX-2 instruction set.

* How to run tests

raidq-test    : tester for RAIDq code.

raid_rs-test  : tester for Reed-Solomon code.

err-decoder-test : tester for error decoder.

tests/basicop-benchmark-test: benchmarker for basic operations.

code-benchmark-test :  benchmarker for various codes.

code-benchmark2-test:  benchmarker for various codes.

err-decoder-benchmark-test: benchmarker for error decoder.

* Deployment instructions

type: make

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact
