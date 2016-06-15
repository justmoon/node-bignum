var BigNum = require('../')
var test = require('tap').test

test('create', function (t) {
  var validBn = BigNum('42')
  var testObj

  testObj = BigNum('123')
  t.equal(BigNum.isBigNum(testObj), true)

  testObj = {}
  t.equal(BigNum.isBigNum(testObj), false)

  testObj = {}
  t.throws(function () {
    validBn.add(testObj)
  })

  testObj = falsePositive()
  t.equal(BigNum.isBigNum(testObj), true)

  // this causes a hard crash, so its disabled for now
  // testObj = falsePositive()
  // t.throws(function() {
  //     validBn.add(testObj)
  // })

  t.end()
})

function falsePositive () {
  var obj = {}
  for (var key in BigNum.prototype) {
    obj[key] = true
  }
  return obj
}
