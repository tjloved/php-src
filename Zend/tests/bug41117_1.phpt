--TEST--
Bug #41117 (Altering $this via argument)
--FILE--
<?php

class foo
{
    function __construct($this)
    {
        echo $this . "\n";
    }
}
function fn769874236()
{
    $obj = new foo("Hello world");
}
fn769874236();
--EXPECTF--
Fatal error: Cannot use $this as parameter in %s on line %d
