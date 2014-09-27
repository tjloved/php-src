--TEST--
Heredoc with double quotes syntax but missing second quote
--FILE--
<?php
$test = "foo";
$var = <<<"MYLABEL
test: $test
MYLABEL;
echo $var;
?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
