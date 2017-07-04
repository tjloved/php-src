--TEST--
Call function with correct number of arguments
--FILE--
<?php

function foo()
{
}
function bar($foo, $bar)
{
}
function bat(int $foo, string $bar)
{
}
function fn1402355030()
{
    foo();
    bar(1, 2);
    bat(123, "foo");
    bat("123", "foo");
    $fp = fopen(__FILE__, "r");
    fclose($fp);
    echo "done";
}
fn1402355030();
--EXPECT--
done