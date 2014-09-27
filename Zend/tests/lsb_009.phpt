--TEST--
ZE2 Late Static Binding interface name "static"
--FILE--
<?php
interface static {
}
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
