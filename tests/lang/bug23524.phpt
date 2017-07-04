--TEST--
Bug #23524 (Improper handling of constants in array indices)
--FILE--
<?php

function f($a = array(THE_CONST => THE_CONST))
{
    print_r($a);
}
function fn607412821()
{
    echo "Begin\n";
    define("THE_CONST", 123);
    f();
    f();
    f();
    echo "Done";
}
fn607412821();
--EXPECT--
Begin
Array
(
    [123] => 123
)
Array
(
    [123] => 123
)
Array
(
    [123] => 123
)
Done
