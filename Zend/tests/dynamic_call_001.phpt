--TEST--
Testing dynamic call to constructor (old-style)
--FILE--
<?php 

class foo { 
	public function foo() {
	}	
}

$a = 'foo';

$a::$a();

?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Non-static method foo::foo() cannot be called statically' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
