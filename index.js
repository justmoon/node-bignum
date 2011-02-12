var ffi = require('node-ffi');
var bigint = new ffi.Library(__dirname + '/build/default/libbigint', {
    create : [ 'uint32', [] ],
    destroy : [ 'uint32', [ 'uint32' ] ],
    toString : [ 'string', [ 'uint32', 'uint32' ] ],
    fromString : [ 'uint32', [ 'string', 'uint32' ] ],
});

module.exports = function (num) {
    if (typeof num === number) {
    }
};

var x = bigint.create();
console.log('x.toString(10) = ' + bigint.toString(x, 10));

var y = bigint.fromString('555', 10);
console.log('y = ' + y);
console.log('y.toString(10) = ' + bigint.toString(y, 10));

bigint.destroy(x);
bigint.destroy(y);
