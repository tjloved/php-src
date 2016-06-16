--TEST--
Calling parse_str through argument unpacking
--FILE--
<?php

function test()
{
    $i = 0;
    parse_str(...["i=41"]);
    var_dump($i + 1);
}
function fn1781854825()
{
    test();
}
fn1781854825();
--EXPECT--
int(42)
