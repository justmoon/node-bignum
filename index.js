var ffi = require('node-ffi');
var bigint = new ffi.Library(__dirname + '/build/default/libbigint', {
    create : [ 'uint32', [] ],
    destroy : [ 'uint32', [ 'uint32' ] ],
    toString : [ 'string', [ 'uint32', 'uint32' ] ],
    fromString : [ 'uint32', [ 'string', 'uint32' ] ],
});

module.exports = BigInt;

function BigInt (num, base) {
    if (!(this instanceof BigInt)) return new BigInt(num, base);
    
    if (typeof num === 'number') {
        this.id = bigint.fromString(num.toString(), base);
    }
    else if (typeof num === 'string') {
        this.id = bigint.fromString(num, base || 10);
    }
    else {
        this.id = bigint.create();
    }
}

BigInt.prototype.inspect = function () {
    return '<BigInt ' + this.toString(10) + '>';
};

BigInt.prototype.toString = function (base) {
    return bigint.toString(this.id, base || 10);
};

BigInt.prototype.destroy = function (base) {
    bigint.destroy(this.id);
};
