--TEST--
Bug #64966 (segfault in zend_do_fcall_common_helper_SPEC)
--FILE--
<?php
error_reporting(E_ALL);

function test($func) {
	$a = $func("");
	return true;
}
class A {
	public function b() {
		test("strlen");
		test("iterator_apply");
	}
}

$a = new A();
$a->b();
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Argument 1 passed to iterator_apply() must implement interface Traversable, string given' in %s:%d
Stack trace:
#0 %s(%d): iterator_apply('')
#1 %s(%d): test('iterator_apply')
#2 %s(%d): A->b()
#3 {main}
  thrown in %s on line %d
