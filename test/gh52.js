var BigNum = require('../')
var test = require('tap').test

test('github52', function (t) {
  // verify that numbers aren't truncated to 32 bits when running on
  // 32-bit platforms (where the OpenSSL sizeof(BN_ULONG) is 32 bits)
  var num = new BigNum(0x100000000)
  t.equal(num.toString(), '4294967296')
  t.equal(num.add(0x100000000).toString(), '8589934592')
  t.equal(num.sub(0x100000001).toString(), '-1')
  t.equal(num.mul(0x100000000).toString().toString(), '18446744073709551616')
  t.equal(num.div(0x100000002).toString(), '0')
  t.equal(num.mod(0x100000002).toString(), '4294967296')
  num = new BigNum(2)
  t.equal(num.powm(0x100000001, 4).toString(), '0')
  num = new BigNum(-0x100000000)
  t.ok(num.cmp(-0x100000000) === 0)
  num = new BigNum(0x100000000)
  t.ok(num.cmp(0x100000000) === 0)

  t.end()
})
