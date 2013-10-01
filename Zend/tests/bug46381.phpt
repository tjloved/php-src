--TEST--
Bug #46381 (wrong $this passed to internal methods causes segfault)
--SKIPIF--
<?php if (!extension_loaded("spl")) die("skip SPL is no available"); ?>
--FILE--
<?php

class test {
	public function test() {
		return ArrayIterator::current();
	}
}
$test = new test();
$test->test();

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Uncaught exception 'EngineException' with message 'Non-static method ArrayIterator::current() cannot be called statically, assuming $this from incompatible context' in %s:%d
Stack trace:
#0 %s(%d): test->test()
#1 {main}
  thrown in %s on line %d
