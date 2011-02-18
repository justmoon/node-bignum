var assert = require('assert');
var bigint = require('bigint');
var put = require('put');

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
            assert.eql(bigint.add(i, j).toString(), ks);
        }
    }
    
    assert.eql(
        bigint(
            '201781752444966478956292456789265633588628356858680927185287861892'
            + '9889675589272409635031813235465496971529430565627918846694860512'
            + '1492948268400884893722767401972695174353441'
        ).add(
            '939769862972759638577945343130228368606420083646071622223953046277'
            + '3784500359975110887672142614667937014937371109558223563373329424'
            + '0624814097369771481147215472578762824607080'
        ).toString(),
        '1141551615417726117534237799919494002195048440504752549409240908170367'
        + '41759492475205227039558501334339864668016751861424100681899362117762'
        + '365770656374869982874551457998960521'
    );
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
            assert.eql(bigint.sub(i, j).toString(), ks);
        }
    }
    
    assert.eql(
        bigint(
            '635849762218952604062459342660379446997761295162166888134051068531'
            + '9813941775949841573516110003093332652267534768664621969514455380'
            + '8051168706779408804756208386011014197185296'
        ).sub(
            '757617343536280696839135295661092954931163607913400460585109207644'
            + '7966483882748233585856350085641718822741649072106343655764769889'
            + '6399869016678013515043471880323279258685478'
        ).toString(),
        '-121767581317328092776675953000713507933402312751233572451058139112815'
        + '25421067983920123402400825483861704741143034417216862503145088348700'
        + '309898604710287263494312265061500182'
    );
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
            assert.eql(bigint.mul(i, j).toString(), ks);
        }
    }
    
    assert.eql(
        bigint(
            '433593290010590489671135819286259593426549306666324008679782084292'
            + '2446494189019075159822930571858728009485237489829138626896756141'
            + '8738958337632249177044975686477011571044266'
        ).mul(
            '127790264841901718791915669264129510947625523373763053776083279450'
            + '3886212911067061184379695097643279217271150419129022856601771338'
            + '794256383410400076210073482253089544155377'
        ).toString(),
        '5540900136412485758752141142221047463857522755277604708501015732755989'
        + '17659432099233635577634197309727815375309484297883528869192732141328'
        + '99346769031695550850320602049507618052164677667378189154076988316301'
        + '23719953859959804490669091769150047414629675184805332001182298088891'
        + '58079529848220802017396422115936618644438110463469902675126288489182'
        + '82'
    );
    
    assert.eql(
        bigint('10000000000000000000000000000').mul(-123).toString(),
        '-1230000000000000000000000000000'
    );
};

exports.div = function () {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var ks = Math.floor(i / j).toString();
            if (ks.match(/^-?\d+$/)) { // ignore exceptions
                assert.eql(bigint(i).div(j).toString(), ks);
                assert.eql(bigint(i).div(js).toString(), ks);
                assert.eql(bigint(i).div(bigint(j)).toString(), ks);
                assert.eql(bigint.div(i, j).toString(), ks);
            }
        }
    }
    
    assert.eql(
        bigint(
            '433593290010590489671135819286259593426549306666324008679782084292'
            + '2446494189019075159822930571858728009485237489829138626896756141'
            + '8738958337632249177044975686477011571044266'
        ).div(
            '127790264841901718791915669264129510947625523373763053776083279450'
            + '3886212911067061184379695097643279217271150419129022856601771338'
            + '794256383410400076210073482253089544155377'
        ).toString(),
        '33'
    );
};

exports.abs = function () {
    assert.eql(
        bigint(
            '433593290010590489671135819286259593426549306666324008679782084292'
            + '2446494189019075159822930571858728009485237489829138626896756141'
            + '8738958337632249177044975686477011571044266'
        ).abs().toString(),
        '4335932900105904896711358192862595934265493066663240086797820842922446'
        + '49418901907515982293057185872800948523748982913862689675614187389583'
        + '37632249177044975686477011571044266'
    );
    
    assert.eql(
        bigint(
            '-43359329001059048967113581928625959342654930666632400867978208429'
            + '2244649418901907515982293057185872800948523748982913862689675614'
            + '18738958337632249177044975686477011571044266'
        ).abs().toString(),
        '4335932900105904896711358192862595934265493066663240086797820842922446'
        + '49418901907515982293057185872800948523748982913862689675614187389583'
        + '37632249177044975686477011571044266'
    );
};

exports.neg = function () {
    assert.eql(
        bigint(
            '433593290010590489671135819286259593426549306666324008679782084292'
            + '2446494189019075159822930571858728009485237489829138626896756141'
            + '8738958337632249177044975686477011571044266'
        ).neg().toString(),
        '-433593290010590489671135819286259593426549306666324008679782084292244'
        + '64941890190751598229305718587280094852374898291386268967561418738958'
        + '337632249177044975686477011571044266'
    );
    
    assert.eql(
        bigint(
            '-43359329001059048967113581928625959342654930666632400867978208429'
            + '2244649418901907515982293057185872800948523748982913862689675614'
            + '18738958337632249177044975686477011571044266'
        ).neg().toString(),
        '4335932900105904896711358192862595934265493066663240086797820842922446'
        + '49418901907515982293057185872800948523748982913862689675614187389583'
        + '37632249177044975686477011571044266'
    );
};

exports.mod = function () {
    for (var i = 0; i < 10; i++) {
        for (var j = 0; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            if (!isNaN(i % j)) {
                var ks = (i % j).toString();
                assert.eql(bigint(i).mod(j).toString(), ks);
                assert.eql(bigint(i).mod(js).toString(), ks);
                assert.eql(bigint(i).mod(bigint(j)).toString(), ks);
                assert.eql(bigint.mod(i, j).toString(), ks);
            }
        }
    }
    
    assert.eql(
        bigint('486541542410442549118519277483401413')
            .mod('1802185856709793916115771381388554')
            .toString()
        ,
        '1753546955507985683376775889880387'
    );
};

exports.powm = function () {
    var twos = [ 2, '2', bigint(2), bigint('2') ]
    var tens = [ 100000, '100000', bigint(100000), bigint(100000) ];
    twos.forEach(function (two) {
        tens.forEach(function (t) {
            assert.eql(
                bigint('111111111').powm(two, t).toString(),
                '54321'
            );
        });
    });
    
    assert.eql(
        bigint('624387628734576238746587435')
            .powm(2732, '457676874367586')
            .toString()
        ,
        '335581885073251'
    );
};

exports.pow = function () {
    [ 2, '2', bigint(2), bigint('2') ].forEach(function (two) {
        assert.eql(
            bigint('111111111').pow(two).toString(),
            '12345678987654321'
        );
    });
    
    assert.eql(
        bigint('3487438743234789234879').pow(22).toString(),
        '861281136448465709000943928980299119292959327175552412961995332536782980636409994680542395362634321718164701236369695670918217801815161694902810780084448291245512671429670376051205638247649202527956041058237646154753587769450973231275642223337064356190945030999709422512682440247294915605076918925272414789710234097768366414400280590151549041536921814066973515842848197905763447515344747881160891303219471850554054186959791307149715821010152303317328860351766337716947079041'
    );
};

exports.rand = function () {
    for (var i = 1; i < 1000; i++) {
        var x = bigint(i).rand().toNumber();
        assert.ok(0 <= x  && x < i);
        
        var y = bigint(i).rand(i + 10).toNumber();
        assert.ok(i <= y && y < i + 10);
        
        var z = bigint.rand(i, i + 10).toNumber();
        assert.ok(i <= z && z < i + 10);
    }
};

exports.pack_be = function () {
    var buf1 = new Buffer([1,2,3,4]);
    var num = bigint.pack(buf1, { size : 4 }).toNumber();
    var buf2 = put().word32be(num).buffer();
    assert.eql(buf1, buf2, 
        [].slice.call(buf1) + ' != ' + [].slice.call(buf2)
    );
};

exports.pack_le = function () {
    var buf1 = new Buffer([1,2,3,4]);
    var num = bigint.pack(buf1, { size : 4, endian : 'little' }).toNumber();
    var buf2 = put().word32le(num).buffer();
    assert.eql(buf1, buf2);
};

exports.pack_le_rev = function () {
    var buf = new Buffer([1,2,3,4]);
    var bufRev = new Buffer([3,4,1,2]);
    
    var num = bigint.pack(buf, {
        size : 2,
        endian : 'little',
    }).toNumber();
    
    var numRev = bigint.pack(bufRev, {
        size : 2,
        endian : 'little',
        order : 'backward',
    }).toNumber();
    
    assert.eql(num, numRev);
    
    var bufPut = put()
        .word16le(Math.floor(num / 256 / 256))
        .word16le(num % (256 * 256))
        .buffer()
    ;
    assert.eql(
        buf, bufPut,
        [].slice.call(buf) + ' != ' + [].slice.call(bufPut)
    );
};

exports.pack_be_le = function () {
    var buf_be = new Buffer([1,2,3,4,5,6,7,8]);
    var buf_le = new Buffer([4,3,2,1,8,7,6,5]);
    
    var num_be = bigint
        .pack(buf_be, { size : 4, endian : 'big' })
        .toString()
    ;
    var num_le = bigint
        .pack(buf_le, { size : 4, endian : 'little' })
        .toString()
    ;
    
    assert.eql(num_be, num_le);
};

exports.pack_high_bits = function () {
    var buf_be = new Buffer([
        201,202,203,204,
        205,206,207,208
    ]);
    var buf_le = new Buffer([
        204,203,202,201,
        208,207,206,205
    ]);
    
    var num_be = bigint
        .pack(buf_be, { size : 4, endian : 'big' })
        .toString()
    ;
    var num_le = bigint
        .pack(buf_le, { size : 4, endian : 'little' })
        .toString()
    ;
    
    assert.eql(num_be, num_le);
};

exports.unpack_pack = function () {
    var nums = [
        0, 1, 10, 15, 3, 16,
        7238, 1337, 31337, 505050,
        '172389721984375328763297498273498732984324',
        '32848432742',
        '12988282841231897498217398217398127983721983719283721',
        '718293798217398217312387213972198321'
    ];
    
    nums.forEach(function (num) {
        var b = bigint(num);
        var u = b.unpack();
        
        assert.ok(u);
        assert.eql(
            bigint.pack(u).toString(),
            b.toString()
        );
    });
    
    assert.throws(function () {
        bigint(-1).unpack(); // can't pack negative numbers yet
    });
};

if (process.argv[1] === __filename) {
    assert.eql = assert.deepEqual;
    Object.keys(exports).forEach(function (ex) {
        exports[ex]();
    });
}
