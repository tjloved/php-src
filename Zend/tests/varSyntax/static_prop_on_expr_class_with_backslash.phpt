--TEST--
Static property on constexpr class with leading backslash
--FILE--
<?php

class A
{
    public static $b = 42;
}
function fn679713896()
{
    var_dump(('\\A' . (string) '')::$b);
}
fn679713896();
--EXPECT--
int(42)
