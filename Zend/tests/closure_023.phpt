--TEST--
Closure 023: Closure declared in statically called method
--FILE--
<?php

class foo
{
    public static function bar()
    {
        $func = function () {
            echo "Done";
        };
        $func();
    }
}
function fn629674312()
{
    foo::bar();
}
fn629674312();
--EXPECT--
Done

