--TEST--
Bug #60613 (Segmentation fault with $cls->{expr}() syntax)
--FILE--
<?php

class Cls
{
    function __call($name, $arg)
    {
    }
}
function fn196257129()
{
    $cls = new Cls();
    $cls->{0}();
    $cls->{1.0}();
    $cls->{true}();
    $cls->{false}();
    $cls->{null}();
    echo "ok\n";
}
fn196257129();
--EXPECTF--
Fatal error: Method name must be a string in %sbug60613.php on line %d
