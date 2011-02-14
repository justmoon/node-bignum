bigint
======

Arbitrary precision integral arithmetic for node.js!

This library wraps around [libgmp](http://gmplib.org/)'s
[integer functions](http://gmplib.org/manual/Integer-Functions.html#Integer-Functions)
to perform infinite-precision arithmetic using
[node-ffi](https://github.com/rbranson/node-ffi).

example
=======

    var bigint = require('bigint');

    var b = bigint('782910138827292261791972728324982')
        .sub('182373273283402171237474774728373')
        .div(8)
    ;
    console.log(b);

***
    $ node simple.js
    <BigInt 75067108192986261319312244199576>

methods
=======

bigint(n, base=10)
------------------

Create a new bigint from `n` and a base. `n` can be a string, integer, or
another `bigint`.

If you pass in a string you can set the base that string is encoded in.

destroy()
---------

Destroy a bigint. This module is using node-ffi so I can't hook onto the
GC easily. You only need to call this if you're creating bigints in a loop.

toString(base=10)
-----------------

Print out the `bigint` instance in the requested base as a string.

add(n)
------

Return a new `bigint` containing the instance value plus `n`.

sub(n)
------

Return a new `bigint` containing the instance value minus `n`.

mul(n)
------

Return a new `bigint` containing the instance value multiplied by `n`.

div(n)
------

Return a new `bigint` containing the instance value integrally divided by `n`.

abs()
-----

Return a new `bigint` with the absolute value of the instance.

neg()
-----

Return a new `bigint` with the negative of the instance value.

pow(n)
------

Return a new `bigint` with the instance value raised to the `n`th power.

powm(n, m)
----------

Return a new `bigint` with the instance value raised to the `n`th power modulo
`m`.
