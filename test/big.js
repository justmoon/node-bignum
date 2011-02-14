var assert = require('assert');
var bigint = require('bigint');

exports.create = function () {
    assert.eql(bigint(1337).toString(), '1337');
    assert.eql(bigint('1337').toString(), '1337');
    assert.eql(new bigint('100').toString(), '100');
    assert.eql(
        new bigint('55555555555555555555555555').toString(),
        '55555555555555555555555555'
    );
    
    assert.eql(Number(bigint('1e+100').toString()), 1e+100);
    assert.eql(Number(bigint('1.23e+45').toString()), 1.23e+45);
    for (var i = 0; i < 10; i++) {
        assert.eql(
            bigint('1.23456e+' + i).toString(),
            Math.floor(1.23456 * Math.pow(10,i))
        );
    }
    
    assert.eql(bigint('1.23e-45').toString(), '0');
};

exports.add = function () {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i + j).toString();
            assert.eql(bigint(i).add(j).toString(), ks);
            assert.eql(bigint(i).add(js).toString(), ks);
            assert.eql(bigint(i).add(bigint(j)).toString(), ks);
        }
    }
};

exports.sub = function () {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i - j).toString();
            assert.eql(bigint(i).sub(j).toString(), ks);
            assert.eql(bigint(i).sub(js).toString(), ks);
            assert.eql(bigint(i).sub(bigint(j)).toString(), ks);
        }
    }
};

exports.mul = function () {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i * j).toString();
            assert.eql(bigint(i).mul(j).toString(), ks);
            assert.eql(bigint(i).mul(js).toString(), ks);
            assert.eql(bigint(i).mul(bigint(j)).toString(), ks);
        }
    }
    
    assert.eql(
        bigint('10000000000000000000000000000').mul(123).toString(),
        '1230000000000000000000000000000'
    );
    
    assert.eql(
        bigint('10000000000000000000000000000').mul(-123).toString(),
        '-1230000000000000000000000000000'
    );
};

if (process.argv[1] === __filename) {
    assert.eql = assert.deepEqual;
    Object.keys(exports).forEach(function (ex) {
        exports[ex]();
    });
}
