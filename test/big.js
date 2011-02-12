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

exports.multiply = function () {
    assert.eql(bigint(3).mul(4).toString(), '12');
    assert.eql(bigint(3).mul('4').toString(), '12');
    assert.eql(bigint(3).mul(bigint(4)).toString(), '12');
    
    assert.eql(bigint(5).mul(-7).toString(), '-35');
    assert.eql(bigint(5).mul('-7').toString(), '-35');
    assert.eql(bigint(5).mul(bigint('-7')).toString(), '-35');
    
    assert.eql(
        bigint('10000000000000000000000000000').mul(123).toString(),
        '1230000000000000000000000000000'
    );
    
    assert.eql(
        bigint('10000000000000000000000000000').mul(-123).toString(),
        '-1230000000000000000000000000000'
    );
};

exports.add = function () {
    assert.eql(bigint(3).add(4).toString(), '7');
    assert.eql(bigint(3).add('4').toString(), '7');
    assert.eql(bigint(3).add(bigint(4)).toString(), '7');
    
    assert.eql(bigint(5).add(-7).toString(), '-2');
    assert.eql(bigint(5).add('-7').toString(), '-2');
    assert.eql(bigint(5).add(bigint('-7')).toString(), '-2');
};
