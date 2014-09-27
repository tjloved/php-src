--TEST--
Argument parsing error #002
--FILE--
<?php
function foo($arg1/) {}
?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected '%s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
