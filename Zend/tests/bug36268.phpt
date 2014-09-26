--TEST--
Bug #36268 (Object destructors called even after fatal errors)
--FILE--
<?php
class Foo {
	function __destruct() {
		echo "Ha!\n";
	}
}
$x = new Foo();
bar();
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Call to undefined function bar()' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
