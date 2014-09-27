--TEST--
Argument parsing error #001
--FILE--
<?php
function foo($arg1 string) {}
?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected 'string' (T_STRING), expecting ')'' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
