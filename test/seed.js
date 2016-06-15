// skip on Windows for now, this doesn't work
if (process.platform === 'win32') {
  process.exit(0)
}

var path = require('path')
var execFile = require('child_process').execFile
var test = require('tap').test

test('rand', function (t) {
  var to = setTimeout(function () {
    t.fail('never executed')
  }, 5000)

  var args = [
    '-p',
    'require("' + path.join(__dirname, '..') + '").rand(1000).toString()'
  ]

  execFile(process.execPath, args, function (err1, r1) {
    execFile(process.execPath, args, function (err2, r2) {
      clearTimeout(to)

      t.ifError(err1)
      t.ifError(err2)

      t.ok(
        r1.match(/^\d+[\r\n]+/),
        JSON.stringify(r1) + ' is not an integer'
      )
      t.ok(
        r2.match(/^\d+[\r\n]+/),
        JSON.stringify(r2) + ' is not an integer'
      )

      var n1 = parseInt(r1.split(/[\r\n]+/)[0], 10)
      var n2 = parseInt(r2.split(/[\r\n]+/)[0], 10)

      t.ok(n1 >= 0, 'n1 >= 0')
      t.ok(n2 >= 0, 'n2 >= 0')
      t.ok(n1 < 1000, 'n1 < 1000')
      t.ok(n2 < 1000, 'n2 < 1000')

      t.ok(n1 !== n2, 'n1 != n2')

      t.end()
    })
  })
})
