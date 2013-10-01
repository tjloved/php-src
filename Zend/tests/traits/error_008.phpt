--TEST--
Trying to implement a trait
--FILE--
<?php

trait abc { }

class foo implements abc { }

?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'foo cannot implement abc - it is not an interface' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
