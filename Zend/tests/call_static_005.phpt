--TEST--
Invalid method name in dynamic static call
--FILE--
<?php

class foo {
	static function __callstatic($a, $b) {
		var_dump($a);
	}
}

$a = 'foo::';
$a();

?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Call to undefined function foo::()' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
