var BigNum = require('../')
var test = require('tap').test

test('windowsCrash', function (t) {
  var num = new BigNum(1234)
  t.equal(num.toString(), '1234')

  t.end()
})
