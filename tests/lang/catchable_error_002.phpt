--TEST--
Catchable fatal error [2]
--FILE--
<?php

function func(Foo $a) { }

try {
	func(42);
} catch (EngineException $e) {
    echo $e->getMessage(), "\n";
}

echo "ALIVE!\n";

?>
--EXPECTF--
Argument 1 passed to func() must be an instance of Foo, integer given, called in %s on line %d and defined
ALIVE!
