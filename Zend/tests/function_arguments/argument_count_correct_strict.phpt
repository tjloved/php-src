--TEST--
Call function with correct number of arguments with strict types
--FILE--
<?php

declare (strict_types=1);
function foo()
{
}
function bar($foo, $bar)
{
}
function bat(int $foo, string $bar)
{
}
function fn223481889()
{
    foo();
    bar(1, 2);
    bat(123, "foo");
    $fp = fopen(__FILE__, "r");
    fclose($fp);
    echo "done";
}
fn223481889();
--EXPECT--
done