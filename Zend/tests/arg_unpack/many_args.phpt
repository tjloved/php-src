--TEST--
Argument unpacking with many arguments
--FILE--
<?php

function fn(...$args)
{
    var_dump(count($args));
}
function fn531578180()
{
    $array = array_fill(0, 10000, 42);
    fn(...$array, ...$array);
}
fn531578180();
--EXPECT--
int(20000)
