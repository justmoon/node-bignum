var assert = require('assert'),
    BigNum = require('../');

exports.windowsCrash = function () {
    var num = new BigNum(1234);
    assert.equal(num.toString(), '1234')
}
