var ffi = require('node-ffi');
var bigint = new ffi.Library(__dirname + '/build/default/libbigint', {
    create : [ 'uint32', [] ],
    destroy : [ 'uint32', [ 'uint32' ] ],
    toString : [ 'string', [ 'uint32', 'uint32' ] ],
    fromString : [ 'uint32', [ 'string', 'uint32' ] ],
    mul : [ 'uint32', [ 'uint32', 'uint32' ] ],
});

module.exports = BigInt;

function BigInt (num, base) {
    if (!(this instanceof BigInt)) return new BigInt(num, base);
    if (typeof num !== 'string') num = num.toString(base || 10);
    
    if (num.match(/e\+/)) { // positive exponent
        if (!Number(num).toString().match(/e\+/)) {
            this.id = bigint.fromString(
                Math.floor(Number(num)).toString(), 10
            );
        }
        else {
            var pow = Math.ceil(Math.log(num) / Math.log(2));
            var n = (num / Math.pow(2, pow)).toString(2)
                .replace(/^0/,'');
            var i = n.length - n.indexOf('.');
            n = n.replace(/\./,'');
            
            for (; i <= pow; i++) n += '0';
            this.id = bigint.fromString(n, 2);
        }
    }
    else if (num.match(/e\-/)) { // negative exponent
        this.id = bigint.fromString(
            Math.floor(Number(num)).toString(),
            base || 10
        );
    }
    else {
        this.id = bigint.fromString(num, base || 10);
    }
}

BigInt.fromId = function (id) {
    var bigi = Object.create(BigInt.prototype);
    bigi.id = id;
    return bigi;
};

BigInt.prototype.destroy = function () {
    bigint.destroy(this.id);
    return this;
};

BigInt.prototype.inspect = function () {
    return '<BigInt ' + this.toString(10) + '>';
};

BigInt.prototype.toString = function (base) {
    return bigint.toString(this.id, base || 10);
};

BigInt.prototype.mul = function (num) {
    if (num instanceof BigInt) {
        return BigInt.fromId(bigint.mul(this.id, num.id));
    }
    else {
        var x = BigInt(num);
        var id = bigint.mul(this.id, x.id);
        var res = BigInt.fromId(id);
        x.destroy();
        return res;
    }
};
