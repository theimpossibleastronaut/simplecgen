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

The sample site is published at http://theimpossibleastronaut.com/simplecgen/

To generate the sample site (html files will be generated to a
'_web_root' folder):

    From the top level directory

        ./build/simplecgen (it will read ./templates and ./infiles for the data)

Copy the assets folder into the newly created '_web_root' directory. You
can then preview the site by opening index.html in your web browser.

Some of the web site design was done by [Elliot
Alexander](https://github.com/escottalexander)

LICENSE: GNU GPL3
