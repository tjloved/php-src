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
function fn868562851()
{
    $t = new A();
    print_r($t);
}
fn868562851();
--EXPECT--
A Object
(
)
