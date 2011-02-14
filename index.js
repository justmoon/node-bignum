var ffi = require('node-ffi');
var bigint = new ffi.Library(__dirname + '/build/default/libbigint', {
    create : [ 'uint32', [] ],
    destroy : [ 'uint32', [ 'uint32' ] ],
    toString : [ 'string', [ 'uint32', 'uint32' ] ],
    fromString : [ 'uint32', [ 'string', 'uint32' ] ],
    
    badd : [ 'uint32', [ 'uint32', 'uint32' ] ],
    bsub : [ 'uint32', [ 'uint32', 'uint32' ] ],
    bmul : [ 'uint32', [ 'uint32', 'uint32' ] ],
    bdiv : [ 'uint32', [ 'uint32', 'uint32' ] ],
    
    uadd : [ 'uint32', [ 'uint32', 'uint64' ] ],
    usub : [ 'uint32', [ 'uint32', 'uint64' ] ],
    umul : [ 'uint32', [ 'uint32', 'uint64' ] ],
    udiv : [ 'uint32', [ 'uint32', 'uint64' ] ],
    
    babs : [ 'uint32', [ 'uint32' ] ],
    bneg : [ 'uint32', [ 'uint32' ] ],
    
    bmod : [ 'uint32', [ 'uint32', 'uint32' ] ],
    umod : [ 'uint32', [ 'uint32', 'uint64' ] ],
    bpowm : [ 'uint32', [ 'uint32', 'uint32', 'uint32' ] ],
    upowm : [ 'uint32', [ 'uint32', 'uint32', 'uint32' ] ],
    upow : [ 'uint32', [ 'uint32', 'uint64' ] ],
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

[ 'add', 'sub', 'mul', 'div', 'mod' ].forEach(function (op) {
    BigInt.prototype[op] = function (num) {
        if (num instanceof BigInt) {
            return BigInt.fromId(bigint['b'+op](this.id, num.id));
        }
        else if (typeof num === 'number') {
            if (num >= 0) {
                return BigInt.fromId(bigint['u'+op](this.id, num));
            }
            else if (op === 'add') {
                return BigInt.fromId(bigint.usub(this.id, -num));
            }
            else if (op === 'sub') {
                return BigInt.fromId(bigint.uadd(this.id, -num));
            }
            else {
                var x = new BigInt(num);
                var res = BigInt.fromId(bigint['b'+op](this.id, x.id));
                x.destroy();
                return res;
            }
        }
        else if (typeof num === 'string') {
            var x = new BigInt(num);
            var res = BigInt.fromId(bigint['b'+op](this.id, x.id));
            x.destroy();
            return res;
        }
        else {
            throw new TypeError('Unspecified operation for type '
                + (typeof num) + ' for ' + op);
        }
    };
});

BigInt.prototype.abs = function () {
    return BigInt.fromId(bigint.babs(this.id));
};

BigInt.prototype.neg = function () {
    return BigInt.fromId(bigint.bneg(this.id));
};

BigInt.prototype.powm = function (num, mod) {
    if (num instanceof BigInt && mod instanceof BigInt) {
        return BigInt.fromId(bigint.bpowm(this.id, num.id, mod.id));
    }
    else if (num >= 0 && mod instanceof BigInt) {
        var res = BigInt.fromId(bigint.upowm(this.id, num, mod.id));
        return res;
    }
    else if (num instanceof BigInt && mod >= 0) {
        var x = new BigInt(mod);
        var res = BigInt.fromId(bigint.bpowm(this.id, num.id, x.id));
        x.destroy();
        return res;
    }
    else {
        var x = new BigInt(num);
        var y = new BigInt(mod);
        var res = BigInt.fromId(bigint.bpowm(this.id, x.id, y.id));
        x.destroy();
        y.destroy();
        return res;
    }
};

BigInt.prototype.pow = function (num) {
    if (typeof num === 'number') {
        if (num >= 0) {
            return BigInt.fromId(bigint.upow(this.id, num));
        }
        else {
            return BigInt.prototype.powm.call(this, num, this);
        }
    }
    else {
        var x = parseInt(num.toString(), 10);
        return BigInt.prototype.pow.call(this, x);
    }
};

Object.keys(BigInt.prototype).forEach(function (name) {
    if (name === 'inspect' || name === 'toString') return;
    
    BigInt[name] = function (num) {
        var args = [].slice.call(arguments, 1);
        
        if (num instanceof BigInt) {
            return num[name].apply(num, args);
        }
        else {
            var bigi = new BigInt(num);
            return bigi[name].apply(bigi, args);
        }
    };
});
