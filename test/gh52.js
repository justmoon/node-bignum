var assert = require('assert'),
    BigNum = require('../');

exports.github52 = function () {
    // verify that numbers aren't truncated to 32 bits when running on
    // 32-bit platforms (where the OpenSSL sizeof(BN_ULONG) is 32 bits)
    var num = new BigNum(0x100000000);
    assert.equal(num, '4294967296');
    assert.equal(num.add(0x100000000), '8589934592');
    assert.equal(num.sub(0x100000001), '-1');
    assert.equal(num.mul(0x100000000), '18446744073709551616');
    assert.equal(num.div(0x100000002), '0');
    assert.equal(num.mod(0x100000002), '4294967296');
    num = new BigNum(2);
    assert.equal(num.powm(0x100000001, 4), '0');
    num = new BigNum(-0x100000000);
    assert(num.cmp(-0x100000000) === 0);
    num = new BigNum(0x100000000);
    assert(num.cmp(0x100000000) === 0);
};
