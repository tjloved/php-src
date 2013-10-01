--TEST--
Trying to inherit a class in an interface
--FILE--
<?php

interface a extends Exception { }

?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'a cannot implement Exception - it is not an interface' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
