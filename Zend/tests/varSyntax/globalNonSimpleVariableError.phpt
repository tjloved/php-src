--TEST--
Global keyword only accepts simple variables
--FILE--
<?php

global $$foo->bar;

?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected '->' (T_OBJECT_OPERATOR), expecting ',' or ';'' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
