--TEST--
SPL: ArrayIterator and values assigned by reference
--FILE--
<?php
	
$tmp = 1;

$a = new ArrayIterator();
$a[] = $tmp;
$a[] = &$tmp;

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Uncaught exception 'EngineException' with message 'Cannot assign by reference to overloaded object' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
