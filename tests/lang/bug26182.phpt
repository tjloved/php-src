--TEST--
Bug #26182 (Object properties created redundantly)
--INI--
error_reporting=4095
--FILE--
<?php

class A
{
    function NotAConstructor()
    {
        if (isset($this->x)) {
            //just for demo
        }
    }
}
function fn693402096()
{
    $t = new A();
    print_r($t);
}
fn693402096();
--EXPECT--
A Object
(
)
