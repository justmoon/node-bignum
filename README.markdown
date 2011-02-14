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
    ;
    console.log(b);
    console.log(b.toString());

***
    $ node simple.js
    <BigInt 75067108192986261319312244199576>
    75067108192986261319312244199576

methods
=======

bigint(integer or string, base=10)
----------------------------------

Create a new bigint from an integer or string.
If you pass in a string you can set the base that string is encoded in.

destroy()
---------

Destroy a bigint. This module is using node-ffi so I can't hook onto the
GC easily. You only need to call this if you're creating bigints in a loop.

toString(base=10)
-----------------

Print out the `bigint` instance in the requested base as a string.

add(integer or string or bigint)
--------------------------------

Return a new `bigint` containing the instance value plus the supplied value.

sub(integer or string or bigint)
--------------------------------

Return a new `bigint` containing the instance value minus the supplied value.

mul(integer or string or bigint)
--------------------------------

Return a new `bigint` containing the instance value multiplied by the supplied
value.

div(integer or string or bigint)
--------------------------------

Return a new `bigint` containing the instance value divided by the supplied
value.

abs()
-----

Return a new `bigint` with the absolute value of the instance.

neg()
-----

Return a new `bigint` with the negative of the instance value.
