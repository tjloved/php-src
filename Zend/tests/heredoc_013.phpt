--TEST--
Heredoc with double quotes and wrong prefix
--FILE--
<?php
$test = "foo";
$var = prefix<<<"MYLABEL"
test: $test
MYLABEL;
echo $var;
?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
