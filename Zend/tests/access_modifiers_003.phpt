--TEST--
using multiple access modifiers (classes)
--FILE--
<?php

final final class test {
	function foo() {}
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected 'final' (T_FINAL), expecting class (T_CLASS)' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
