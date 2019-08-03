# simplecgen

[![Build
Status](https://travis-ci.org/theimpossibleastronaut/simplecgen.svg?branch=master)](https://travis-ci.org/theimpossibleastronaut/simplecgen)

simplecgen uses the
[simplectemplate](https://github.com/dafky2000/simplectemplate) library
to generate a web site. After you clone this repo, to get the library,
just use:

    git submodule update --init --recursive

To build simplecgen:

    mkdir build
    cd build
    cmake ../src
    make

simplecgen will look for the config file (simplecgen.conf) in the
directory from which you are running simplecgen.

To generate the sample site:

    From the top level directory

        ./build/simplecgen (it will read ./templates and ./infiles for the data)

You can then preview the site by opening index.html in your web browser.

Some of the web site design was done by [Elliot
Alexander](https://github.com/escottalexander)

LICENSE: GNU GPL3
