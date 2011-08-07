bigint
======

Arbitrary precision integral arithmetic for node.js!

This library wraps around [libgmp](http://gmplib.org/)'s
[integer functions](http://gmplib.org/manual/Integer-Functions.html#Integer-Functions)
to perform infinite-precision arithmetic.

example
=======

simple.js
---------

    var bigint = require('bigint');
    
    var b = bigint('782910138827292261791972728324982')
        .sub('182373273283402171237474774728373')
        .div(8)
    ;
    console.log(b);

***
    $ node simple.js
    <BigInt 75067108192986261319312244199576>

perfect.js
----------

Generate the perfect numbers:

    // If 2**n-1 is prime, then (2**n-1) * 2**(n-1) is perfect.
    var bigint = require('bigint');

    for (var n = 0; n < 100; n++) {
        var p = bigint.pow(2, n).sub(1);
        if (p.probPrime(50)) {
            var perfect = p.mul(bigint.pow(2, n - 1));
            console.log(perfect.toString());
        }
    }

***

    6
    28
    496
    8128
    33550336
    8589869056
    137438691328
    2305843008139952128
    2658455991569831744654692615953842176
    191561942608236107294793378084303638130997321548169216

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

Note that endian doesn't matter when size = 1.

methods[1]
==========

For all of the instance methods below you can write either

    bigint.method(x, y, z)

or if x is a `bigint` instance``

    x.method(y, z)

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

Note that endian doesn't matter when size = 1.

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

.cmp(n)
-------

Compare the instance value to `n`. Return a positive integer if `> n`, a
negative integer if `< n`, and 0 if `== n`.

.gt(n)
------

Return a boolean: whether the instance value is greater than n (`> n`).

.ge(n)
------

Return a boolean: whether the instance value is greater than or equal to n
(`>= n`).

.eq(n)
------

Return a boolean: whether the instance value is equal to n (`== n`).

.lt(n)
------

Return a boolean: whether the instance value is less than n (`< n`).

.le(n)
------

Return a boolean: whether the instance value is less than or equal to n
(`<= n`).

.and(n)
-------

Return a new `bigint` with the instance value bitwise AND (&)-ed with `n`.

.or(n)
------

Return a new `bigint` with the instance value bitwise inclusive-OR (|)-ed with
`n`.

.xor(n)
-------

Return a new `bigint` with the instance value bitwise exclusive-OR (^)-ed with
`n`.

.pow(n)
-------

Return a new `bigint` with the instance value raised to the `n`th power.

.powm(n, m)
-----------

Return a new `bigint` with the instance value raised to the `n`th power modulo
`m`.

.invertm(m)
-----------

Compute the multiplicative inverse modulo `m`.

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

.sqrt()
-------

Return a new `bigint` that is the square root.  This truncates.

.root(n)
-------

Return a new `bigint` that is the `nth` root.  This truncates.

.shiftLeft(n)
-------

Return a new `bigint` that is the `2^n` multiple. Equivalent of the <<
operator.

.shiftRight(n)
-------

Return a new `bigint` of the value integer divided by
`2^n`. Equivalent of the >> operator.

install
=======

You'll need the libgmp source to compile this package. Under Debian-based systems,

    sudo aptitude install libgmp3-dev

On a Mac with [Homebrew](https://github.com/mxcl/homebrew/),

    brew install gmp

And then install with [npm](http://npmjs.org):

    npm install bigint
