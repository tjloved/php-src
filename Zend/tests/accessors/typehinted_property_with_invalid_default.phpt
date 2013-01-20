--TEST--
Only null is allowed as default value for properties with accessors
--FILE--
<?php

class Test {
    public $foo = 42 { }
}

?>
--EXPECTF--
Fatal error: Only null is allowed as a default value for properties with accessors in %s on line %d
