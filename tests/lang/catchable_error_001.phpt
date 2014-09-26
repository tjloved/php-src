--TEST--
Catchable fatal error [1]
--FILE--
<?php
	class Foo {
	}

	function blah (Foo $a)
	{
	}

	function error()
	{
		$a = func_get_args();
		var_dump($a);
	}

	blah (new StdClass);
	echo "ALIVE!\n";
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Argument 1 passed to blah() must be an instance of Foo, instance of stdClass given, called in %s on line %d and defined' in %s:%d
Stack trace:
#0 %s(%d): blah(Object(stdClass))
#1 {main}
  thrown in %s on line %d
