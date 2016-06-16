--TEST--
Bug #27468 (foreach in __destruct() causes segfault)
--FILE--
<?php

class foo
{
    function __destruct()
    {
        foreach ($this->x as $x) {
        }
    }
}
function fn1819083869()
{
    new foo();
    echo 'OK';
}
fn1819083869();
--EXPECTF--
Notice: Undefined property: foo::$x in %sbug27468.php on line %d

Warning: Invalid argument supplied for foreach() in %sbug27468.php on line %d
OK
