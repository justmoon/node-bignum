var ffi = require('node-ffi');
var bignum = new ffi.Library(__dirname + '/build/default/libbignum', {
    create : [ 'uint32', [] ],
    destroy : [ 'uint32', [ 'uint32' ] ],
    toString : [ 'string', [ 'uint32', 'uint32' ] ],
    fromString : [ 'uint32', [ 'string', 'uint32' ] ],
});

module.exports = function (num) {
    if (typeof num === number) {
    }
};

var x = bignum.create();
console.log('x.toString(10) = ' + bignum.toString(x, 10));

var y = bignum.fromString('555', 10);
console.log('y = ' + y);
console.log('y.toString(10) = ' + bignum.toString(y, 10));

bignum.destroy(x);
bignum.destroy(y);
