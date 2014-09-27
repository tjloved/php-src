--TEST--
ZE2 Late Static Binding ensuring implementing 'static' is not allowed
--FILE--
<?php

class Foo implements static {
}

?>
==DONE==
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
