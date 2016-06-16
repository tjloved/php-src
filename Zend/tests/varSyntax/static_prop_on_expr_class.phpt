--TEST--
Accessing a static property on a statically evaluable class expression
--FILE--
<?php

class A
{
    public static $b = 42;
}
function fn1891792905()
{
    var_dump(('A' . (string) '')::$b);
}
fn1891792905();
--EXPECT--
int(42)
