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

methods[0]
==========

bigint(n, base=10)
------------------

Create a new `bigint` from `n` and a base. `n` can be a string, integer, or
another `bigint`.

If you pass in a string you can set the base that string is encoded in.

.toString(base=10)
------------------

Print out the `bigint` instance in the requested base as a string.

bigint.fromBuffer(buf, opts)
----------------------

Create a new `bigint` from a `Buffer`.

The default options are:
    {
        order : 'forward', // low-to-high indexed word ordering
        endian : 'big',
        size : 1, // number of bytes in each word
    }

methods[1]
==========

For all of the instance methods below you can write either

    bigint.method(x, y, z)

or if x is a `bigint` instance``

    x.method(y, z)

.destroy()
----------

Destroy a `bigint`. This module is using node-ffi so I can't hook onto the
GC easily. You only need to call this if you're creating bigints in a loop.

.toNumber()
-----------

Turn a `bigint` into a `Number`. If the `bigint` is too big you'll lose
precision or you'll get ±`Infinity`.

.toBuffer(opts)
-------------

Return a new `Buffer` with the data from the `bigint`.

The default options are:
    {
        order : 'forward', // low-to-high indexed word ordering
        endian : 'big',
        size : 1, // number of bytes in each word
    }

.add(n)
-------

Return a new `bigint` containing the instance value plus `n`.

.sub(n)
-------

Return a new `bigint` containing the instance value minus `n`.

.mul(n)
-------

Return a new `bigint` containing the instance value multiplied by `n`.

.div(n)
-------

Return a new `bigint` containing the instance value integrally divided by `n`.

.abs()
------

Return a new `bigint` with the absolute value of the instance.

.neg()
------

Return a new `bigint` with the negative of the instance value.

.pow(n)
-------

Return a new `bigint` with the instance value raised to the `n`th power.

.powm(n, m)
-----------

Return a new `bigint` with the instance value raised to the `n`th power modulo
`m`.

.rand()
-------
.rand(upperBound)
-----------------

If `upperBound` is supplied, return a random `bigint` between the instance value
and `upperBound - 1`, inclusive.

Otherwise, return a random `bigint` between 0 and the instance value - 1,
inclusive.

.probPrime()
------------

Return whether the bigint is:

* certainly prime (true)
* probably prime ('maybe')
* certainly composite (false)

using [mpz_probab_prime](http://gmplib.org/manual/Number-Theoretic-Functions.html).

.nextPrime()
------------

Return the next prime greater than `this` using
[mpz_nextprime](http://gmplib.org/manual/Number-Theoretic-Functions.html).
