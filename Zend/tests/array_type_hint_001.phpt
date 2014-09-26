--TEST--
Array type hint
--FILE--
<?php
function foo(array $a) {
	echo count($a)."\n";
}

foo(array(1,2,3));
foo(123);
?>
--EXPECTF--
3

Fatal error: Uncaught exception 'EngineException' with message 'Argument 1 passed to foo() must be of the type array, integer given, called in %s on line %d and defined' in %s:%d
Stack trace:
#0 %s(%d): foo(123)
#1 {main}
  thrown in %s on line %d
