var bi = new require('./build/default/bigint')
  , BigInt = bi.BigInt;

module.exports = BigInt;

BigInt.conditionArgs = function(num, base) {
    if (typeof num !== 'string') num = num.toString(base || 10);
    
    if (num.match(/e\+/)) { // positive exponent
        if (!Number(num).toString().match(/e\+/)) {
	    return {
		    num: Math.floor(Number(num)).toString(),
		    base: 10
	    };
        }
        else {
            var pow = Math.ceil(Math.log(num) / Math.log(2));
            var n = (num / Math.pow(2, pow)).toString(2)
                .replace(/^0/,'');
            var i = n.length - n.indexOf('.');
            n = n.replace(/\./,'');
            
            for (; i <= pow; i++) n += '0';
	    return {
		    num: n,
		    base: 2
	    };
        }
    }
    else if (num.match(/e\-/)) { // negative exponent
	return {
		num: Math.floor(Number(num)).toString(),
		base: base || 10
	};
    }
    else {
	return {
		num: num,
		base: base || 10
	};
    }
};

bi.setJSConditioner(BigInt.conditionArgs);

BigInt.prototype.inspect = function () {
    return '<BigInt ' + this.toString(10) + '>';
};

BigInt.prototype.toNumber = function () {
    return parseInt(this.toString(), 10);
};

[ 'add', 'sub', 'mul', 'div', 'mod' ].forEach(function (op) {
    BigInt.prototype[op] = function (num) {
        if (num instanceof BigInt) {
            return this['b'+op](num);
        }
        else if (typeof num === 'number') {
            if (num >= 0) {
                return this['u'+op](num);
            }
            else if (op === 'add') {
                return this.usub(-num);
            }
            else if (op === 'sub') {
                return this.uadd(-num);
            }
            else {
                var x = BigInt(num);
                return this['b'+op](x);
            }
        }
        else if (typeof num === 'string') {
            var x = BigInt(num);
            return this['b'+op](x);
        }
        else {
            throw new TypeError('Unspecified operation for type '
                + (typeof num) + ' for ' + op);
        }
    };
});

BigInt.prototype.abs = function () {
    return this.babs();
};

BigInt.prototype.neg = function () {
    return this.bneg();
};

BigInt.prototype.powm = function (num, mod) {
	var m, res;

	if ((typeof mod) === 'number' || (typeof mod) === 'string') {
		m = BigInt(mod);
	} else if (mod instanceof BigInt) {
		m = mod;
	}

	if ((typeof num) === 'number') {
		return this.upowm(num, m);
	} else if ((typeof num) === 'string') {
		var n = BigInt(num);
		return this.bpowm(n, m);
	} else if (num instanceof BigInt) {
		return this.bpowm(num, m);
	}
};

BigInt.prototype.pow = function (num) {
    if (typeof num === 'number') {
        if (num >= 0) {
            return this.upow(num);
        } else {
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
        return this.bcompare(num);
    }
    else if (typeof num === 'number') {
        if (num < 0) {
            return this.scompare(num);
        } else {
            return this.ucompare(num);
        }
    }
    else {
        var x = BigInt(num);
        return this.bcompare(x);
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

BigInt.prototype.ne = function (num) {
    return this.cmp(num) !== 0;
};

BigInt.prototype.lt = function (num) {
    return this.cmp(num) < 0;
};

BigInt.prototype.le = function (num) {
    return this.cmp(num) <= 0;
};

'and or xor'.split(' ').forEach(function (name) {
    BigInt.prototype[name] = function (num) {
        if (num instanceof BigInt) {
            return this['b' + name](num);
        } else {
            var x = BigInt(num);
            return this['b' + name](x);
        }
    };
});

BigInt.prototype.sqrt = function() {
	return this.bsqrt();
};

BigInt.prototype.root = function(num) {
	if (num instanceof BigInt) {
		return this.broot(num);
	} else {
		var x = BigInt(num);
		return this.broot(num);
	}
};

BigInt.prototype.rand = function (to) {
    if (to === undefined) {
        if (this.toString() === '1') {
            return BigInt(0);
        } else {
            return this.brand0();
        }
    }
    else {
        var x = to instanceof BigInt
            ? to.sub(this)
            : BigInt(to).sub(this);
        return x.brand0().add(this);
    }
};

BigInt.prototype.invertm = function (mod) {
    if (mod instanceof BigInt) {
        return this.binvertm(mod);
    } else {
        var x = BigInt(mod);
        return this.binvertm(x);
    }
};

BigInt.prototype.probPrime = function (reps) {
    var n = this.probprime(reps || 10);
    return { 2 : true, 1 : 'maybe', 0 : false }[n];
};

BigInt.prototype.nextPrime = function () {
    return this.nextprime();
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
    
    return BigInt(
        (order === 'forward' ? hex : hex.reverse()).join(''), 16
    );
};

BigInt.prototype.toBuffer = function (opts) {
    if (typeof opts === 'string') {
        if (opts !== 'mpint') return 'Unsupported Buffer representation';
        
        var abs = this.abs();
        var buf = abs.toBuffer({ size : 1, endian : 'big' });
        var len = buf.length === 1 && buf[0] === 0 ? 0 : buf.length;
        if (buf[0] & 0x80) len ++;
        
        var ret = new Buffer(4 + len);
        if (len > 0) buf.copy(ret, 4 + (buf[0] & 0x80 ? 1 : 0));
        if (buf[0] & 0x80) ret[4] = 0;
        
        ret[0] = len & (0xff << 24);
        ret[1] = len & (0xff << 16);
        ret[2] = len & (0xff << 8);
        ret[3] = len & (0xff << 0);
        
        // two's compliment for negative integers:
        var isNeg = this.lt(0);
        if (isNeg) {
            for (var i = 4; i < ret.length; i++) {
                ret[i] = 0xff - ret[i];
            }
        }
        ret[4] = (ret[4] & 0x7f) | (isNeg ? 0x80 : 0);
        if (isNeg) ret[ret.length - 1] ++;
        
        return ret;
    }
    
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
            var bigi = BigInt(num);
            return bigi[name].apply(bigi, args);
        }
    };
});
