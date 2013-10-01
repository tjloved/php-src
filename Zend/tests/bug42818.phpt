--TEST--
Bug #42818 ($foo = clone(array()); leaks memory)
--FILE--
<?php
$foo = clone(array());
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message '__clone method called on non-object' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
