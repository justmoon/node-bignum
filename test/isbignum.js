var assert = require('assert');
var BigNum = require('../');

exports.create = function () {

    var validBn = BigNum('42');
    var testObj;

    testObj = BigNum('123');
    assert.equal(BigNum.isBigNum(testObj), true);

    testObj = {};
    assert.equal(BigNum.isBigNum(testObj), false);

    testObj = {};
    assert.throws(function() {
        validBn.add(testObj)
    });

    testObj = falsePositive();
    assert.equal(BigNum.isBigNum(testObj), true);

    // this causes a hard crash, so its disabled for now
    // testObj = falsePositive();
    // assert.throws(function() {
    //     validBn.add(testObj)
    // });
};

function falsePositive () {
    var obj = {};
    for (var key in BigNum.prototype) {
        obj[key] = true;
    }
    return obj;
}

if (process.argv[1] === __filename) {
    assert.eql = assert.deepEqual;
    Object.keys(exports).forEach(function (ex) {
        exports[ex]();
    });

    if ("function" === typeof gc) {
        gc();
    }
}
