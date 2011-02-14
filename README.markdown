bigint
======

Arbitrary precision integral arithmetic for node.js!

examples
========

simple.js
---------

    var bigint = require('bigint');

    var b = bigint('782910138827292261791972728324982')
        .sub('182373273283402171237474774728373')
        .div(8)
        .toString()
    ;
    console.log(b);

***
    $ node simple.js
    75067108192986261319312244199576
