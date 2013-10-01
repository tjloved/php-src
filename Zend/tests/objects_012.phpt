--TEST--
implementing a class
--FILE--
<?php

class foo {
}

interface bar extends foo {
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Uncaught exception 'EngineException' with message 'bar cannot implement foo - it is not an interface' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
