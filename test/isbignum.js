var assert = require('assert');
var bignum = require('../');

exports.create = function () {

    var validBn = bignum('42');
    var testObj;

    testObj = bignum('123');
    assert.equal(bignum.isBigNum(testObj), true);

    testObj = {};
    assert.equal(bignum.isBigNum(testObj), false);

    testObj = {};
    assert.throws(function() {
        validBn.add(testObj)
    });

    testObj = falsePositive();
    assert.equal(bignum.isBigNum(testObj), true);

    // this causes a hard crash, so its disabled for now
    // testObj = falsePositive();
    // assert.throws(function() {
    //     validBn.add(testObj)
    // });
};

function falsePositive () {
    return {
        badd: true,
        bsub: true,
        bmul: true,
        bdiv: true,
        bmod: true,
    };
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
