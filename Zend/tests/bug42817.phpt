--TEST--
Bug #42817 (clone() on a non-object does not result in a fatal error)
--FILE--
<?php
$a = clone(null);
array_push($a->b, $c);
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message '__clone method called on non-object' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
