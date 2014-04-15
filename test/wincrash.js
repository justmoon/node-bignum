var assert = require('assert'),
    BigNum = require('../');

exports.windowsCrash = function () { 
    var num = new BigNum(0);
    num = num.add(1);
    assert.eql(num.toString(), '1')
}
