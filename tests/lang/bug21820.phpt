--TEST--
Bug #21820 ("$arr['foo']" generates bogus E_NOTICE, should be E_PARSE)
--FILE--
<?php

error_reporting(E_ALL);

$arr = array('foo' => 'bar');
echo "$arr['foo']";

?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected %s, expecting identifier (T_STRING) or variable (T_VARIABLE) or number (T_NUM_STRING)' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
