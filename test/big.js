var bignum = require('../');
var test = require('tap').test;

test('create', function (t) {
    t.deepEqual(bignum(1337).toString(), '1337');
    t.deepEqual(bignum('1337').toString(), '1337');
    t.deepEqual(new bignum('100').toString(), '100');
    t.deepEqual(
        new bignum('55555555555555555555555555').toString(),
        '55555555555555555555555555'
    );
    
    t.deepEqual(Number(bignum('1e+100').toString()), 1e+100);
    t.deepEqual(bignum('1e+100').bitLength(), 333);
    t.deepEqual(Number(bignum('1.23e+45').toString()), 1.23e+45);
    for (var i = 0; i < 10; i++) {
        t.deepEqual(
            bignum('1.23456e+' + i).toString(),
            Math.floor(1.23456 * Math.pow(10,i))
        );
    }
    
    t.deepEqual(bignum('1.23e-45').toString(), '0');

    t.throws(function() { bignum(undefined); });
    t.throws(function() { bignum(null); });

    t.end();
})

test('add', function (t) {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i + j).toString();
            t.deepEqual(bignum(i).add(j).toString(), ks);
            t.deepEqual(bignum(i).add(js).toString(), ks);
            t.deepEqual(bignum(i).add(bignum(j)).toString(), ks);
            t.deepEqual(bignum.add(i, j).toString(), ks);
        }
    }
    
    t.deepEqual(
        bignum(
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

    t.end();
})

test('sub', function (t) {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i - j).toString();
            t.deepEqual(bignum(i).sub(j).toString(), ks);
            t.deepEqual(bignum(i).sub(js).toString(), ks);
            t.deepEqual(bignum(i).sub(bignum(j)).toString(), ks);
            t.deepEqual(bignum.sub(i, j).toString(), ks);
        }
    }
    
    t.deepEqual(
        bignum(
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

    t.end();
})

test('mul', function (t) {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i * j).toString();
            t.deepEqual(bignum(i).mul(j).toString(), ks);
            t.deepEqual(bignum(i).mul(js).toString(), ks);
            t.deepEqual(bignum(i).mul(bignum(j)).toString(), ks);
            t.deepEqual(bignum.mul(i, j).toString(), ks);
        }
    }
    
    t.deepEqual(
        bignum(
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
    
    t.deepEqual(
        bignum('10000000000000000000000000000').mul(-123).toString(),
        '-1230000000000000000000000000000'
    );

    t.end();
})

test('div', function (t) {
    for (var i = -10; i < 10; i++) {
        for (var j = -10; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            var round = ((i/j) < 0) ? Math.ceil : Math.floor;
            var ks = round(i / j).toString();
            if (ks.match(/^-?\d+$/)) { // ignore exceptions
                t.deepEqual(bignum(i).div(j).toString(), ks);
                t.deepEqual(bignum(i).div(js).toString(), ks);
                t.deepEqual(bignum(i).div(bignum(j)).toString(), ks);
                t.deepEqual(bignum.div(i, j).toString(), ks);
            }
        }
    }
    
    t.deepEqual(
        bignum(
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

    t.end();
})

test('abs', function (t) {
    t.deepEqual(
        bignum(
            '433593290010590489671135819286259593426549306666324008679782084292'
            + '2446494189019075159822930571858728009485237489829138626896756141'
            + '8738958337632249177044975686477011571044266'
        ).abs().toString(),
        '4335932900105904896711358192862595934265493066663240086797820842922446'
        + '49418901907515982293057185872800948523748982913862689675614187389583'
        + '37632249177044975686477011571044266'
    );
    
    t.deepEqual(
        bignum(
            '-43359329001059048967113581928625959342654930666632400867978208429'
            + '2244649418901907515982293057185872800948523748982913862689675614'
            + '18738958337632249177044975686477011571044266'
        ).abs().toString(),
        '4335932900105904896711358192862595934265493066663240086797820842922446'
        + '49418901907515982293057185872800948523748982913862689675614187389583'
        + '37632249177044975686477011571044266'
    );

    t.end();
})

test('neg', function (t) {
    t.deepEqual(
        bignum(
            '433593290010590489671135819286259593426549306666324008679782084292'
            + '2446494189019075159822930571858728009485237489829138626896756141'
            + '8738958337632249177044975686477011571044266'
        ).neg().toString(),
        '-433593290010590489671135819286259593426549306666324008679782084292244'
        + '64941890190751598229305718587280094852374898291386268967561418738958'
        + '337632249177044975686477011571044266'
    );
    
    t.deepEqual(
        bignum(
            '-43359329001059048967113581928625959342654930666632400867978208429'
            + '2244649418901907515982293057185872800948523748982913862689675614'
            + '18738958337632249177044975686477011571044266'
        ).neg().toString(),
        '4335932900105904896711358192862595934265493066663240086797820842922446'
        + '49418901907515982293057185872800948523748982913862689675614187389583'
        + '37632249177044975686477011571044266'
    );

    t.end();
})

test('mod', function (t) {
    for (var i = 0; i < 10; i++) {
        for (var j = 0; j < 10; j++) {
            var is = i.toString();
            var js = j.toString();
            if (!isNaN(i % j)) {
                var ks = (i % j).toString();
                t.deepEqual(bignum(i).mod(j).toString(), ks);
                t.deepEqual(bignum(i).mod(js).toString(), ks);
                t.deepEqual(bignum(i).mod(bignum(j)).toString(), ks);
                t.deepEqual(bignum.mod(i, j).toString(), ks);
            }
        }
    }
    
    t.deepEqual(
        bignum('486541542410442549118519277483401413')
            .mod('1802185856709793916115771381388554')
            .toString()
        ,
        '1753546955507985683376775889880387'
    );

    t.end();
})

test('cmp', function (t) {
    for (var i = -10; i <= 10; i++) {
        var bi = bignum(i);
        
        for (var j = -10; j <= 10; j++) {
            [ j, bignum(j) ].forEach(function (jj) {
                t.deepEqual(bi.lt(jj), i < j);
                t.deepEqual(bi.le(jj), i <= j);
                t.deepEqual(bi.eq(jj), i === j);
                t.deepEqual(bi.ne(jj), i !== j);
                t.deepEqual(bi.gt(jj), i > j);
                t.deepEqual(bi.ge(jj), i >= j);
            });
        }
    }

    t.end();
})

test('powm', function (t) {
    var twos = [ 2, '2', bignum(2), bignum('2') ]
    var tens = [ 100000, '100000', bignum(100000), bignum(100000) ];
    twos.forEach(function (two) {
        tens.forEach(function (ten) {
            t.deepEqual(
                bignum('111111111').powm(two, ten).toString(),
                '54321'
            );
        });
    });
    
    t.deepEqual(
        bignum('624387628734576238746587435')
            .powm(2732, '457676874367586')
            .toString()
        ,
        '335581885073251'
    );

    t.end();
})

test('pow', function (t) {
    [ 2, '2', bignum(2), bignum('2') ].forEach(function (two) {
        t.deepEqual(
            bignum('111111111').pow(two).toString(),
            '12345678987654321'
        );
    });
    
    t.deepEqual(
        bignum('3487438743234789234879').pow(22).toString(),
        '861281136448465709000943928980299119292959327175552412961995332536782980636409994680542395362634321718164701236369695670918217801815161694902810780084448291245512671429670376051205638247649202527956041058237646154753587769450973231275642223337064356190945030999709422512682440247294915605076918925272414789710234097768366414400280590151549041536921814066973515842848197905763447515344747881160891303219471850554054186959791307149715821010152303317328860351766337716947079041'
    );

    t.end();
})

test('and', function (t) {
    for (var i = 0; i < 256; i += 7) {
        for (var j = 0; j < 256; j += 7) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i & j).toString();
            t.deepEqual(bignum(i).and(j).toString(), ks);
            t.deepEqual(bignum(i).and(js).toString(), ks);
            t.deepEqual(bignum(i).and(bignum(j)).toString(), ks);
            t.deepEqual(bignum.and(i, j).toString(), ks);

            t.deepEqual(bignum(-1 * i).and(j).toString(), ((-1 * i) & j).toString());
            t.deepEqual(bignum(i).and(-1 * j).toString(), (i & (-1 * j)).toString());
            t.deepEqual(bignum(-1 * i).and(-1 * j).toString(), ((-1 * i) & (-1 * j)).toString());
        }
    }
    t.deepEqual(bignum.and(bignum('111111', 16), bignum('111111', 16)).toString(16), '111111');
    t.deepEqual(bignum.and(bignum('111110', 16), bignum('111111', 16)).toString(16), '111110');
    t.deepEqual(bignum.and(bignum('111112', 16), bignum('111111', 16)).toString(16), '111110');
    t.deepEqual(bignum.and(bignum('111121', 16), bignum('111111', 16)).toString(16), '111101');
    t.deepEqual(bignum.and(bignum('111131', 16), bignum('111111', 16)).toString(16), '111111');

    t.deepEqual(bignum.and(bignum('-111111', 16), bignum('111111', 16)).toString(16), '01');
    t.deepEqual(bignum.and(bignum('111111', 16), bignum('-111111', 16)).toString(16), '01');
    t.deepEqual(bignum.and(bignum('-111111', 16), bignum('-111111', 16)).toString(16), '-111111');

    t.end();
})

test('or', function (t) {
    for (var i = 0; i < 256; i += 7) {
        for (var j = 0; j < 256; j += 7) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i | j).toString();
            t.deepEqual(bignum(i).or(j).toString(), ks);
            t.deepEqual(bignum(i).or(js).toString(), ks);
            t.deepEqual(bignum(i).or(bignum(j)).toString(), ks);
            t.deepEqual(bignum.or(i, j).toString(), ks);

            t.deepEqual(bignum(-1 * i).or(j).toString(), ((-1 * i) | j).toString());
            t.deepEqual(bignum(i).or(-1 * j).toString(), (i | (-1 * j)).toString());
            t.deepEqual(bignum(-1 * i).or(-1 * j).toString(), ((-1 * i) | (-1 * j)).toString());
        }
    }
    t.deepEqual(bignum.or(bignum('111111', 16), bignum('111111', 16)).toString(16), '111111');
    t.deepEqual(bignum.or(bignum('111110', 16), bignum('111111', 16)).toString(16), '111111');
    t.deepEqual(bignum.or(bignum('111112', 16), bignum('111111', 16)).toString(16), '111113');
    t.deepEqual(bignum.or(bignum('111121', 16), bignum('111111', 16)).toString(16), '111131');

    t.deepEqual(bignum.or(bignum('-111111', 16), bignum('111111', 16)).toString(16), '-01');
    t.deepEqual(bignum.or(bignum('111111', 16), bignum('-111111', 16)).toString(16), '-01');
    t.deepEqual(bignum.or(bignum('-111111', 16), bignum('-111111', 16)).toString(16), '-111111');

    t.end();
})

test('xor', function (t) {
    for (var i = 0; i < 256; i += 7) {
        for (var j = 0; j < 256; j += 7) {
            var is = i.toString();
            var js = j.toString();
            var ks = (i ^ j).toString();
            t.deepEqual(bignum(i).xor(j).toString(), ks);
            t.deepEqual(bignum(i).xor(js).toString(), ks);
            t.deepEqual(bignum(i).xor(bignum(j)).toString(), ks);
            t.deepEqual(bignum.xor(i, j).toString(), ks);

            t.deepEqual(bignum(-1 * i).xor(j).toString(), ((-1 * i) ^ j).toString());
            t.deepEqual(bignum(i).xor(-1 * j).toString(), (i ^ (-1 * j)).toString());
            t.deepEqual(bignum(-1 * i).xor(-1 * j).toString(), ((-1 * i) ^ (-1 * j)).toString());
        }
    }
    t.deepEqual(bignum.xor(bignum('111111', 16), bignum('111111', 16)).toString(), 0);
    t.deepEqual(bignum.xor(bignum('111110', 16), bignum('111111', 16)).toString(), 1);
    t.deepEqual(bignum.xor(bignum('111112', 16), bignum('111111', 16)).toString(), 3);
    t.deepEqual(bignum.xor(bignum('111121', 16), bignum('111111', 16)).toString(), 0x30);

    t.deepEqual(bignum.xor(bignum('-111111', 16), bignum('111111', 16)).toString(), -2);
    t.deepEqual(bignum.xor(bignum('111111', 16), bignum('-111111', 16)).toString(), -2);
    t.deepEqual(bignum.xor(bignum('-111111', 16), bignum('-111111', 16)).toString(), 0);

    t.end();
})

test('rand', function (t) {
    for (var i = 1; i < 1000; i++) {
        var x = bignum(i).rand().toNumber();
        t.ok(0 <= x  && x < i);
        
        var y = bignum(i).rand(i + 10).toNumber();
        t.ok(i <= y && y < i + 10);
        
        var z = bignum.rand(i, i + 10).toNumber();
        t.ok(i <= z && z < i + 10);
    }

    t.end();
})

test('primes', function (t) {
    var ps = { 2 : true, 3 : true, 5 : true, 7 : true };
    for (var i = 0; i <= 10; i++) {
        t.deepEqual(bignum(i).probPrime(), ps[i] ? true : false);
    }
    
    var ns = {
        2 : 3,
        3 : 5,
        15313 : 15319,
        222919 : 222931,
        611939 : 611951,
        334214459 : '334214467',
        961748927 : '961748941',
        9987704933 : '9987704953',
    };
    
    Object.keys(ns).forEach(function (n) {
        t.deepEqual(
            bignum(n).nextPrime().toString(),
            ns[n].toString()
        );
    });
    
    var uniques = [
        '3', '11', '37', '101', '9091', '9901', '333667', '909091', '99990001',
        '999999000001', '9999999900000001', '909090909090909091',
        '1111111111111111111', '11111111111111111111111',
        '900900900900990990990991',
    ];
    
    var wagstaff = [
        '3', '11', '43', '683', '2731', '43691', '174763', '2796203',
        '715827883', '2932031007403', '768614336404564651',
        '201487636602438195784363', '845100400152152934331135470251',
        '56713727820156410577229101238628035243',
    ];
    
    var big = [
        '4669523849932130508876392554713407521319117239637943224980015676156491',
        '54875133386847519273109693154204970395475080920935355580245252923343305939004903',
        '204005728266090048777253207241416669051476369216501266754813821619984472224780876488344279',
        '2074722246773485207821695222107608587480996474721117292752992589912196684750549658310084416732550077',
        '5628290459057877291809182450381238927697314822133923421169378062922140081498734424133112032854812293',
    ];
    
    [ uniques, wagstaff, big ].forEach(function (xs) {
        xs.forEach(function (x) {
            var p = bignum(x).probPrime();
            t.ok(p === true || p === 'maybe');
        });
    });

    t.end();
})

test('isbitset', function (t) {
    function mkbin(bn) {
        var bin = '';

        for (var i = 0; i < bn.bitLength(); ++i) {
            bin += bn.isBitSet(i) ? '1' : '0';
        }

        return bin;
    }

    t.deepEqual(mkbin(bignum( 127)), '1111111');
    t.deepEqual(mkbin(bignum(-127)), '1111111');

    t.deepEqual(mkbin(bignum( 128)), '00000001');
    t.deepEqual(mkbin(bignum(-128)), '00000001');

    t.deepEqual(mkbin(bignum( 129)), '10000001');
    t.deepEqual(mkbin(bignum(-129)), '10000001');

    t.end();
})

test('invertm', function (t) {
    // numbers from http://www.itl.nist.gov/fipspubs/fip186.htm appendix 5
    var q = bignum('b20db0b101df0c6624fc1392ba55f77d577481e5', 16);
    var k = bignum('79577ddcaafddc038b865b19f8eb1ada8a2838c6', 16);
    var kinv = k.invertm(q);
    t.deepEqual(kinv.toString(16), '2784e3d672d972a74e22c67f4f4f726ecc751efa');

    t.end();
})

test('shift', function (t) {
    t.deepEqual(bignum(37).shiftLeft(2).toString(), (37 << 2).toString()); // 148
    t.deepEqual(bignum(37).shiftRight(2).toString(), (37 >> 2).toString()); // 9
    
    t.equal(
        bignum(2).pow(Math.pow(2,10)).shiftRight(4).toString(),
        bignum(2).pow(Math.pow(2,10)).div(16).toString()
    );

    t.end();
})

test('mod', function (t) {
    t.deepEqual(bignum(55555).mod(2).toString(), '1');
    t.deepEqual(
        bignum('1234567').mod(
            bignum('4321')
        ).toNumber(), 
        1234567 % 4321
    );

    t.end();
})

test('endian', function (t) {
    var a = bignum(0x0102030405);
    t.deepEqual(a.toBuffer({ endian: 'big', size: 2 }).toString('hex'), '000102030405');
    t.deepEqual(a.toBuffer({ endian: 'little', size: 2 }).toString('hex'), '010003020504');
    
    var b = bignum(0x0102030405);
    t.deepEqual(a.toBuffer({ endian: 'big', size: 'auto' }).toString('hex'), '0102030405');
    t.deepEqual(a.toBuffer({ endian: 'little', size: 'auto' }).toString('hex'), '0504030201');
    
    var c = new Buffer("000102030405", 'hex');
    t.deepEqual(bignum.fromBuffer(c, { endian: 'big', size: 'auto'}).toString(16), "0102030405");
    t.deepEqual(bignum.fromBuffer(c, { endian: 'little', size: 'auto'}).toString(16), "050403020100");

    t.end();
})

test('bitlength', function (t) {
    var bl = bignum(
    '433593290010590489671135819286259593426549306666324008679782084292'
      + '2446494189019075159822930571858728009485237489829138626896756141'
      + '873895833763224917704497568647701157104426'
    ).bitLength();
    
    t.equal(bl > 0, true);

    t.end();
})

test('gcd', function (t) {
    var b1 = bignum('234897235923342343242');
    var b2 = bignum('234790237101762305340234');
    var expected = bignum('6');
    t.equal(b1.gcd(b2).toString(), expected.toString());

    t.end();
})

test('jacobi', function (t) {
    // test case from p. 134 of D. R. Stinson
    var b1 = bignum('7411');
    var b2 = bignum('9283');
    t.equal(b1.jacobi(b2), -1);

    // test case from p. 132 of D. R. Stinson
    b1 = bignum('6278');
    b2 = bignum('9975');
    t.equal(b1.jacobi(b2), -1);

    // test case from p. 74 of Men. Oorsh. Vans.
    b1 = bignum('158');
    b2 = bignum('235');
    t.equal(b1.jacobi(b2), -1);

    // test case from p. 216 of Kumanduri Romero
    b1 = bignum('4');
    b2 = bignum('7');
    t.equal(b1.jacobi(b2), 1);

    // test case from p. 363 of K. R. Rosen
    b1 = bignum('68');
    b2 = bignum('111');
    t.equal(b1.jacobi(b2), 1);

    t.end();
})
