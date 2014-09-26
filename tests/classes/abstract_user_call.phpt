--TEST--
ZE2 An abstract method cannot be called indirectly
--FILE--
<?php

abstract class test_base
{
	abstract function func();
}

class test extends test_base
{
	function func()
	{
		echo __METHOD__ . "()\n";
	}
}

$o = new test;

$o->func();

call_user_func(array($o, 'test_base::func'));

?>
===DONE===
--EXPECTF--
test::func()

Fatal error: Uncaught exception 'EngineException' with message 'Cannot call abstract method test_base::func()' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
