# SimpleGet repository contains:

1. SimpleGet Eclipse CDT IDE project, which is an extension of CURL simple HTTP GET example to obtain the median of some times to get a file from a hardcoded URL. This project was done as a previous step to implementing the library and example program requested at the SamKnows Technical Test (C/C++) at https://github.com/SamKnows/tests-and-metrics-test
The easiest way to try the SimpleGet program is to git clone the repository using Eclipse EGit, then Build and Run. By default, the test is executed to collect 100 samples at one sample per second, but it support the command-line arguments requested at the mentioned SamKnows challenge.

2. httpgetmedian Eclipse CDT IDE project, which is a wrapper static library, currently using CURL, but that could be exchanged by another, as requested in the SamKnows challenge.

3. testhttpgetmedian Eclipse CDT IDE project, which is a sample test program of the httpgetmedian library, implementing the same functionalities as SimpleGet project, but using the httpgetmedian library, instead.

In case you don't want to use the Eclipse IDE, you can git clone the repository and execute the make command under the Debug and Release project directories to build the library and the programs, which can be executed on these directories.

The software in this repository has been developed and tested on an x86_64 laptop running Ubuntu 16.04 LTS with a DSL natted connection to the Internet (without proxies). The Wireshark packet capture software has been used to check the software developed functionality works as intended.
