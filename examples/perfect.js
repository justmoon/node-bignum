// If 2**n-1 is prime, then (2**n-1) * 2**(n-1) is perfect.
var bigint = require('../');

for (var n = 0; n < 100; n++) {
    var p = bigint.pow(2, n).sub(1);
    if (p.probPrime(50)) {
        var perfect = p.mul(bigint.pow(2, n - 1));
        console.log(perfect.toString());
    }
}
