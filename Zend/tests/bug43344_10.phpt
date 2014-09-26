--TEST--
Bug #43344.10 (Wrong error message for undefined namespace constant)
--FILE--
<?php
echo namespace\bar."\n";
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Undefined constant 'bar'' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
