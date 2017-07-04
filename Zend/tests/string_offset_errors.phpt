--TEST--
Some string offset errors
--FILE--
<?php

function &test() : string
{
    $str = "foo";
    return $str[0];
}
function fn1549033960()
{
    try {
        test();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        $str = "foo";
        $str[0] =& $str[1];
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
fn1549033960();
--EXPECT--
Cannot return string offsets by reference
Cannot create references to/from string offsets
