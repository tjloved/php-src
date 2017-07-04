--TEST--
Accessing a static property on a statically evaluable class expression
--FILE--
<?php

class A
{
    public static $b = 42;
}
function fn635946548()
{
    var_dump(('A' . (string) '')::$b);
}
fn635946548();
--EXPECT--
int(42)
