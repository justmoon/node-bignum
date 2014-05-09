var BigNum = require('../'),
	assert = require('assert');

describe('Private Changes', function() {
	
	it('setCompact', function() {
		var num = new BigNum('0');
		num.setCompact(436278071);
		assert.strictEqual(num.toString(16), '0113370000000000000000000000000000000000000000000000')
	})
})

