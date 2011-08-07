var assert = require('assert');
var exec = require('child_process').exec;

exports.rand = function () {
    var to = setTimeout(function () {
        assert.fail('never executed');
    }, 5000);
    
    var cmd = 'node -e \'require('
        + JSON.stringify(__dirname + '/../')
        + ').rand(1000).toString()\''
    ;
    exec(cmd, function (err1, r1) {
        exec(cmd, function (err2, r2) {
            clearTimeout(to);
            
            assert.ok(!err1);
            assert.ok(!err2);
            
            assert.ok(r1.match(/^\d+\n$/));
            assert.ok(r2.match(/^\d+\n$/));
            
            var n1 = parseInt(r1, 10);
            var n2 = parseInt(r2, 10);
            
            assert.ok(n1 >= 0, 'n1 >= 0');
            assert.ok(n2 >= 0, 'n2 >= 0');
            assert.ok(n1 < 1000, 'n1 < 1000');
            assert.ok(n2 < 1000, 'n2 < 1000');
            
            assert.ok(n1 != n2, 'n1 != n2');
        })
    });
}
