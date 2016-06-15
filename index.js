var bin = require('bindings')('bignum.node')
var BigNum = bin.BigNum

module.exports = BigNum

BigNum.conditionArgs = function (num, base) {
  if (typeof num !== 'string') num = num.toString(base || 10)

  if (num.match(/e\+/)) { // positive exponent
    if (!Number(num).toString().match(/e+/)) {
      return {
        num: Math.floor(Number(num)).toString(),
        base: 10
      }
    } else {
      var pow = Math.ceil(Math.log(num) / Math.log(2))
      var n = (num / Math.pow(2, pow)).toString(2)
        .replace(/^0/, '')
      var i = n.length - n.indexOf('.')
      n = n.replace(/\./, '')

      for (; i <= pow; i++) n += '0'
      return {
        num: n,
        base: 2
      }
    }
  } else if (num.match(/e\-/)) { // negative exponent
    return {
      num: Math.floor(Number(num)).toString(),
      base: base || 10
    }
  } else {
    return {
      num: num,
      base: base || 10
    }
  }
}

bin.setJSConditioner(BigNum.conditionArgs)

BigNum.isBigNum = function (num) {
  if (!num) {
    return false
  }
  for (var key in BigNum.prototype) {
    if (!num[key]) {
      return false
    }
  }
  return true
}

BigNum.prototype.inspect = function () {
  return '<BigNum ' + this.toString(10) + '>'
}

BigNum.prototype.toString = function (base) {
  var value
  if (base) {
    value = this.tostring(base)
  } else {
    value = this.tostring()
  }
  if (base > 10 && typeof value === 'string') {
    value = value.toLowerCase()
  }
  return value
}

BigNum.prototype.toNumber = function () {
  return parseInt(this.toString(), 10)
}

;[ 'add', 'sub', 'mul', 'div', 'mod' ].forEach(function (op) {
  BigNum.prototype[op] = function (num) {
    var x
    if (BigNum.isBigNum(num)) {
      return this['b' + op](num)
    } else if (typeof num === 'number') {
      if (num >= 0) {
        return this['u' + op](num)
      } else if (op === 'add') {
        return this.usub(-num)
      } else if (op === 'sub') {
        return this.uadd(-num)
      } else {
        x = BigNum(num)
        return this['b' + op](x)
      }
    } else if (typeof num === 'string') {
      x = BigNum(num)
      return this['b' + op](x)
    } else {
      throw new TypeError('Unspecified operation for type ' +
        (typeof num) + ' for ' + op)
    }
  }
})

BigNum.prototype.abs = function () {
  return this.babs()
}

BigNum.prototype.neg = function () {
  return this.bneg()
}

BigNum.prototype.powm = function (num, mod) {
  var m

  if ((typeof mod) === 'number' || (typeof mod) === 'string') {
    m = BigNum(mod)
  } else if (BigNum.isBigNum(mod)) {
    m = mod
  }

  if ((typeof num) === 'number') {
    return this.upowm(num, m)
  } else if ((typeof num) === 'string') {
    var n = BigNum(num)
    return this.bpowm(n, m)
  } else if (BigNum.isBigNum(num)) {
    return this.bpowm(num, m)
  }
}

BigNum.prototype.mod = function (num, mod) {
  var m

  if ((typeof mod) === 'number' || (typeof mod) === 'string') {
    m = BigNum(mod)
  } else if (BigNum.isBigNum(mod)) {
    m = mod
  }

  if ((typeof num) === 'number') {
    return this.umod(num, m)
  } else if ((typeof num) === 'string') {
    var n = BigNum(num)
    return this.bmod(n, m)
  } else if (BigNum.isBigNum(num)) {
    return this.bmod(num, m)
  }
}

BigNum.prototype.pow = function (num) {
  if (typeof num === 'number') {
    if (num >= 0) {
      return this.upow(num)
    } else {
      return BigNum.prototype.powm.call(this, num, this)
    }
  } else {
    var x = parseInt(num.toString(), 10)
    return BigNum.prototype.pow.call(this, x)
  }
}

BigNum.prototype.shiftLeft = function (num) {
  if (typeof num === 'number') {
    if (num >= 0) {
      return this.umul2exp(num)
    } else {
      return this.shiftRight(-num)
    }
  } else {
    var x = parseInt(num.toString(), 10)
    return BigNum.prototype.shiftLeft.call(this, x)
  }
}

BigNum.prototype.shiftRight = function (num) {
  if (typeof num === 'number') {
    if (num >= 0) {
      return this.udiv2exp(num)
    } else {
      return this.shiftLeft(-num)
    }
  } else {
    var x = parseInt(num.toString(), 10)
    return BigNum.prototype.shiftRight.call(this, x)
  }
}

BigNum.prototype.cmp = function (num) {
  if (BigNum.isBigNum(num)) {
    return this.bcompare(num)
  } else if (typeof num === 'number') {
    if (num < 0) {
      return this.scompare(num)
    } else {
      return this.ucompare(num)
    }
  } else {
    var x = BigNum(num)
    return this.bcompare(x)
  }
}

BigNum.prototype.gt = function (num) {
  return this.cmp(num) > 0
}

BigNum.prototype.ge = function (num) {
  return this.cmp(num) >= 0
}

BigNum.prototype.eq = function (num) {
  return this.cmp(num) === 0
}

BigNum.prototype.ne = function (num) {
  return this.cmp(num) !== 0
}

BigNum.prototype.lt = function (num) {
  return this.cmp(num) < 0
}

BigNum.prototype.le = function (num) {
  return this.cmp(num) <= 0
}

'and or xor'.split(' ').forEach(function (name) {
  BigNum.prototype[name] = function (num) {
    if (BigNum.isBigNum(num)) {
      return this['b' + name](num)
    } else {
      var x = BigNum(num)
      return this['b' + name](x)
    }
  }
})

BigNum.prototype.sqrt = function () {
  return this.bsqrt()
}

BigNum.prototype.root = function (num) {
  if (BigNum.isBigNum(num)) {
    return this.broot(num)
  } else {
    return this.broot(num)
  }
}

BigNum.prototype.rand = function (to) {
  if (to === undefined) {
    if (this.toString() === '1') {
      return BigNum(0)
    } else {
      return this.brand0()
    }
  } else {
    var x = BigNum.isBigNum(to)
      ? to.sub(this)
     : BigNum(to).sub(this)
    return x.brand0().add(this)
  }
}

BigNum.prototype.invertm = function (mod) {
  if (BigNum.isBigNum(mod)) {
    return this.binvertm(mod)
  } else {
    var x = BigNum(mod)
    return this.binvertm(x)
  }
}

BigNum.prime = function (bits, safe) {
  if (typeof safe === 'undefined') {
    safe = true
  }

  // Force uint32
  bits >>>= 0

  return BigNum.uprime0(bits, !!safe)
}

BigNum.prototype.probPrime = function (reps) {
  var n = this.probprime(reps || 10)
  return { 1: true, 0: false }[n]
}

BigNum.prototype.nextPrime = function () {
  var num = this
  do {
    num = num.add(1)
  } while (!num.probPrime())
  return num
}

BigNum.prototype.isBitSet = function (n) {
  return this.isbitset(n) === 1
}

BigNum.fromBuffer = function (buf, opts) {
  if (!opts) opts = {}

  var endian = { 1: 'big', '-1': 'little' }[opts.endian] ||
    opts.endian || 'big'

  var size = opts.size === 'auto' ? Math.ceil(buf.length) : (opts.size || 1)

  if (buf.length % size !== 0) {
    throw new RangeError('Buffer length (' + buf.length + ')' +
      ' must be a multiple of size (' + size + ')'
    )
  }

  var hex = []
  for (var i = 0; i < buf.length; i += size) {
    var chunk = []
    for (var j = 0; j < size; j++) {
      chunk.push(buf[i + (endian === 'big' ? j : (size - j - 1))])
    }

    hex.push(chunk
      .map(function (c) {
        return (c < 16 ? '0' : '') + c.toString(16)
      })
      .join('')
    )
  }

  return BigNum(hex.join(''), 16)
}

BigNum.prototype.toBuffer = function (opts) {
  if (typeof opts === 'string') {
    if (opts !== 'mpint') return 'Unsupported Buffer representation'

    var abs = this.abs()
    var buf = abs.toBuffer({ size: 1, endian: 'big' })
    var len = buf.length === 1 && buf[0] === 0 ? 0 : buf.length
    if (buf[0] & 0x80) len++

    var ret = new Buffer(4 + len)
    if (len > 0) buf.copy(ret, 4 + (buf[0] & 0x80 ? 1 : 0))
    if (buf[0] & 0x80) ret[4] = 0

    ret[0] = len & (0xff << 24)
    ret[1] = len & (0xff << 16)
    ret[2] = len & (0xff << 8)
    ret[3] = len & (0xff << 0)

    // two's compliment for negative integers:
    var isNeg = this.lt(0)
    if (isNeg) {
      for (var i = 4; i < ret.length; i++) {
        ret[i] = 0xff - ret[i]
      }
    }
    ret[4] = (ret[4] & 0x7f) | (isNeg ? 0x80 : 0)
    if (isNeg) ret[ret.length - 1]++

    return ret
  }

  if (!opts) opts = {}

  var endian = { 1: 'big', '-1': 'little' }[opts.endian] ||
    opts.endian || 'big'

  var hex = this.toString(16)
  if (hex.charAt(0) === '-') {
    throw new Error('converting negative numbers to Buffers not supported yet')
  }

  var size = opts.size === 'auto' ? Math.ceil(hex.length / 2) : (opts.size || 1)

  len = Math.ceil(hex.length / (2 * size)) * size
  buf = new Buffer(len)

  // zero-pad the hex string so the chunks are all `size` long
  while (hex.length < 2 * len) hex = '0' + hex

  var hx = hex
    .split(new RegExp('(.{' + (2 * size) + '})'))
    .filter(function (s) { return s.length > 0 })

  hx.forEach(function (chunk, i) {
    for (var j = 0; j < size; j++) {
      var ix = i * size + (endian === 'big' ? j : size - j - 1)
      buf[ix] = parseInt(chunk.slice(j * 2, j * 2 + 2), 16)
    }
  })

  return buf
}

Object.keys(BigNum.prototype).forEach(function (name) {
  if (name === 'inspect' || name === 'toString') return

  BigNum[name] = function (num) {
    var args = [].slice.call(arguments, 1)

    if (BigNum.isBigNum(num)) {
      return num[name].apply(num, args)
    } else {
      var bigi = BigNum(num)
      return bigi[name].apply(bigi, args)
    }
  }
})
