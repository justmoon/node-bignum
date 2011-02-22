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
    
    bcompare : [ 'int32', [ 'uint32', 'uint32' ] ],
    scompare : [ 'int32', [ 'uint32', 'int32' ] ],
    ucompare : [ 'int32', [ 'uint32', 'uint64' ] ],
    
    brand0 : [ 'uint32', [ 'uint32' ] ],
    probprime : [ 'char', [ 'uint32', 'uint32' ] ],
    nextprime : [ 'uint32', [ 'uint32' ] ],
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
    if (this.destroyed) throw new Error('BigInt already destroyed');
    bigint.destroy(this.id);
    this.destroyed = true;
};

BigInt.prototype.inspect = function () {
    return '<BigInt ' + this.toString(10) + '>';
};

BigInt.prototype.toString = function (base) {
    if (this.destroyed) throw new Error('BigInt already destroyed');
    return bigint.toString(this.id, base || 10);
};

BigInt.prototype.toNumber = function () {
    return parseInt(this.toString(), 10);
};

[ 'add', 'sub', 'mul', 'div', 'mod' ].forEach(function (op) {
    BigInt.prototype[op] = function (num) {
        if (this.destroyed) throw new Error('BigInt already destroyed');
        
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
    if (this.destroyed) throw new Error('BigInt already destroyed');
    return BigInt.fromId(bigint.babs(this.id));
};

BigInt.prototype.neg = function () {
    if (this.destroyed) throw new Error('BigInt already destroyed');
    return BigInt.fromId(bigint.bneg(this.id));
};

BigInt.prototype.powm = function (num, mod) {
    if (this.destroyed) throw new Error('BigInt already destroyed');
    
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
    if (this.destroyed) throw new Error('BigInt already destroyed');
    
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

BigInt.prototype.cmp = function (num) {
    if (num instanceof BigInt) {
        return bigint.bcompare(this.id, num.id);
    }
    else if (typeof num === 'number') {
        if (num < 0) {
            return bigint.scompare(this.id, num);
        }
        else {
            return bigint.ucompare(this.id, num);
        }
    }
    else {
        var x = bigint(num);
        var res = bigint.bcompare(this.id, x.id);
        x.destroy();
        return res;
    }
};

BigInt.prototype.gt = function (num) {
    return this.cmp(num) > 0;
};

BigInt.prototype.ge = function (num) {
    return this.cmp(num) >= 0;
};

BigInt.prototype.eq = function (num) {
    return this.cmp(num) === 0;
};

BigInt.prototype.lt = function (num) {
    return this.cmp(num) < 0;
};

BigInt.prototype.le = function (num) {
    return this.cmp(num) <= 0;
};

BigInt.prototype.rand = function (to) {
    if (this.destroyed) throw new Error('BigInt already destroyed');
    
    if (to === undefined) {
        if (this.toString() === '1') {
            return new BigInt(0);
        }
        else {
            return BigInt.fromId(bigint.brand0(this.id));
        }
    }
    else {
        var x = to instanceof BigInt
            ? to.sub(this)
            : BigInt.sub(to, this)
        ;
        var y = BigInt.fromId(bigint.brand0(x.id));
        var res = y.add(this);
        x.destroy();
        y.destroy();
        return res;
    }
};

BigInt.prototype.probPrime = function (reps) {
    var n = bigint.probprime(this.id, reps || 10);
    return { 2 : true, 1 : 'maybe', 0 : false }[n];
};

BigInt.prototype.nextPrime = function () {
    return BigInt.fromId(bigint.nextprime(this.id));
};

BigInt.fromBuffer = function (buf, opts) {
    if (!opts) opts = {};
    var order = { 1 : 'forward', '-1' : 'backward' }[opts.order]
        || opts.order || 'forward'
    ;
    
    var endian = { 1 : 'big', '-1' : 'little' }[opts.endian]
        || opts.endian || 'big'
    ;
    
    var size = opts.size || 1;
    
    if (buf.length % size !== 0) {
        throw new RangeError('Buffer length (' + buf.length + ')'
            + ' must be a multiple of size (' + size + ')'
        );
    }
    
    var hex = [];
    for (var i = 0; i < buf.length; i += size) {
        var chunk = [];
        for (var j = 0; j < size; j++) {
            chunk.push(buf[
                i + (endian === 'big' ? j : (size - j - 1))
            ]);
        }
        
        hex.push(chunk
            .map(function (c) {
                return (c < 16 ? '0' : '') + c.toString(16);
            })
            .join('')
        );
    }
    
    return new BigInt(
        (order === 'forward' ? hex : hex.reverse()).join(''), 16
    );
};

BigInt.prototype.toBuffer = function (opts) {
    if (!opts) opts = {};
    var order = { 1 : 'forward', '-1' : 'backward' }[opts.order]
        || opts.order || 'forward'
    ;
    
    var endian = { 1 : 'big', '-1' : 'little' }[opts.endian]
        || opts.endian || 'big'
    ;
    var size = opts.size || 1;
    
    var hex = this.toString(16);
    if (hex.charAt(0) === '-') throw new Error(
        'converting negative numbers to Buffers not supported yet'
    );
    
    var len = Math.ceil(hex.length / (2 * size)) * size;
    var buf = new Buffer(len);
    
    // zero-pad the hex string so the chunks are all `size` long
    while (hex.length < 2 * len) hex = '0' + hex;
    
    var hx = hex
        .split(new RegExp('(.{' + (2 * size) + '})'))
        .filter(function (s) { return s.length > 0 })
    ;
    
    (order === 'forward' ? hx : hx.reverse())
    .forEach(function (chunk, i) {
        for (var j = 0; j < size; j++) {
            var ix = i * size + (endian === 'big' ? j : size - j - 1);
            buf[ix] = parseInt(chunk.slice(j*2,j*2+2), 16);
        }
    });
    
    return buf;
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
