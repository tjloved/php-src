--TEST--
ZE2 Late Static Binding class name "static"
--FILE--
<?php
class static {
}
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
