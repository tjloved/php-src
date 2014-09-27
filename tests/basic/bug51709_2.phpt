--TEST--
Bug #51709 (Can't use keywords as method names)
--FILE--
<?php

class foo {
        static function goto() {
                echo "1";
        }
}

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected 'goto' (T_GOTO), expecting identifier (T_STRING)' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
