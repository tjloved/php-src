--TEST--
Static property on constexpr class with leading backslash
--FILE--
<?php

class A
{
    public static $b = 42;
}
function fn30583010()
{
    var_dump(('\\A' . (string) '')::$b);
}
fn30583010();
--EXPECT--
int(42)
