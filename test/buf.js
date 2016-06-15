var bignum = require('../');
var put = require('put');
var test = require('tap').test;

test('buf_be', function (t) {
    var buf1 = new Buffer([1,2,3,4]);
    var num = bignum.fromBuffer(buf1, { size : 4 }).toNumber();
    t.deepEqual(
        num,
        1*Math.pow(256, 3)
        + 2 * Math.pow(256, 2)
        + 3 * 256
        + 4
    );
    
    var buf2 = put().word32be(num).buffer();
    t.deepEqual(buf1, buf2, 
        '[ ' + [].slice.call(buf1) + ' ] != [ ' + [].slice.call(buf2) + ' ]'
    );

    t.end();
});

test('buf_le', function (t) {
    var buf1 = new Buffer([1,2,3,4]);
    var num = bignum
        .fromBuffer(buf1, { size : 4, endian : 'little' })
        .toNumber()
    ;
    var buf2 = put().word32le(num).buffer();
    t.deepEqual(buf1, buf2,
        '[ ' +  [].join.call(buf1, ',')
            + ' ] != [ '
        + [].join.call(buf2, ',') + ' ]'
    );

    t.end();
});

test('buf_be_le', function (t) {
    var buf_be = new Buffer([1,2,3,4,5,6,7,8]);
    var buf_le = new Buffer([4,3,2,1,8,7,6,5]);
    
    var num_be = bignum
        .fromBuffer(buf_be, { size : 4, endian : 'big' })
        .toString()
    ;
    var num_le = bignum
        .fromBuffer(buf_le, { size : 4, endian : 'little' })
        .toString()
    ;
    
    t.deepEqual(num_be, num_le);

    t.end();
});

test('buf_high_bits', function (t) {
    var buf_be = new Buffer([
        201,202,203,204,
        205,206,207,208
    ]);
    var buf_le = new Buffer([
        204,203,202,201,
        208,207,206,205
    ]);
    
    var num_be = bignum
        .fromBuffer(buf_be, { size : 4, endian : 'big' })
        .toString()
    ;
    var num_le = bignum
        .fromBuffer(buf_le, { size : 4, endian : 'little' })
        .toString()
    ;
    
    t.deepEqual(num_be, num_le);

    t.end();
});

test('buf_to_from', function (t) {
    var nums = [
        0, 1, 10, 15, 3, 16,
        7238, 1337, 31337, 505050,
        '172389721984375328763297498273498732984324',
        '32848432742',
        '12988282841231897498217398217398127983721983719283721',
        '718293798217398217312387213972198321'
    ];
    
    nums.forEach(function (num) {
        var b = bignum(num);
        var u = b.toBuffer();
        
        t.ok(u);
        t.deepEqual(
            bignum.fromBuffer(u).toString(),
            b.toString()
        );
    });
    
    t.throws(function () {
        bignum(-1).toBuffer(); // can't pack negative numbers yet
    });

    t.end();
});

test('toBuf', function (t) {
    var buf = new Buffer([ 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f ]);
    var b = bignum(
        0x0a * 256*256*256*256*256
        + 0x0b * 256*256*256*256
        + 0x0c * 256*256*256
        + 0x0d * 256*256
        + 0x0e * 256
        + 0x0f
    );
    
    t.deepEqual(b.toString(16), '0a0b0c0d0e0f');
    
    t.deepEqual(
        [].slice.call(b.toBuffer({ endian : 'big', size : 2 })),
        [ 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f ]
    );
    
    t.deepEqual(
        [].slice.call(b.toBuffer({ endian : 'little', size : 2 })),
        [ 0x0b, 0x0a, 0x0d, 0x0c, 0x0f, 0x0e ]
    );
    
    t.deepEqual(
        bignum.fromBuffer(buf).toString(16),
        b.toString(16)
    );
    
    t.deepEqual(
        [].slice.call(bignum(43135012110).toBuffer({
            endian : 'little', size : 4
        })),
        [ 0x0a, 0x00, 0x00, 0x00, 0x0e, 0x0d, 0x0c, 0x0b ]
    );
    
    t.deepEqual(
        [].slice.call(bignum(43135012110).toBuffer({
            endian : 'big', size : 4
        })),
        [ 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e ]
    );

    t.end();
});

test('zeroPad', function (t) {
    var b = bignum(0x123456);
    
    t.deepEqual(
        [].slice.call(b.toBuffer({ endian : 'big', size:4 })),
        [ 0x00, 0x12, 0x34, 0x56 ]
    );
    
    t.deepEqual(
        [].slice.call(b.toBuffer({ endian : 'little', size:4 })),
        [ 0x56, 0x34, 0x12, 0x00 ]
    );

    t.end();
});

test('toMpint', function (t) {
    // test values taken directly out of
    // http://tools.ietf.org/html/rfc4251#page-10
    
    var refs = {
        '0' : new Buffer([ 0x00, 0x00, 0x00, 0x00 ]),
        '9a378f9b2e332a7' : new Buffer([
            0x00, 0x00, 0x00, 0x08,
            0x09, 0xa3, 0x78, 0xf9,
            0xb2, 0xe3, 0x32, 0xa7,
        ]),
        '80' : new Buffer([ 0x00, 0x00, 0x00, 0x02, 0x00, 0x80 ]),
        '-1234' : new Buffer([ 0x00, 0x00, 0x00, 0x02, 0xed, 0xcc ]),
        '-deadbeef' : new Buffer([
            0x00, 0x00, 0x00, 0x05, 0xff, 0x21, 0x52, 0x41, 0x11
        ]),
    };
    
    Object.keys(refs).forEach(function (key) {
        var buf0 = bignum(key, 16).toBuffer('mpint');
        var buf1 = refs[key];
        
        t.deepEqual(
            buf0, buf1,
            buf0.inspect() + ' != ' + buf1.inspect()
            + ' for bignum(' + key + ')'
        );
    });

    t.end();
});
