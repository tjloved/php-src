--TEST--
string offset 002
--FILE--
<?php
$a = "aaa";
$x = array(&$a[1]);
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Cannot create references to%s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
