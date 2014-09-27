--TEST--
Bug #51709 (Can't use keywords as method names)
--FILE--
<?php

class foo {
        static function for() {
                echo "1";
        }
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected 'for' (T_FOR), expecting identifier (T_STRING)' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
