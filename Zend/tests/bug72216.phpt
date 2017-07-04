--TEST--
Bug #72216 (Return by reference with finally is not memory safe)
--FILE--
<?php

function &test()
{
    $a = ["ok"];
    try {
        return $a[0];
    } finally {
        $a[""] = 42;
    }
}
function fn1328876836()
{
    var_dump(test());
}
fn1328876836();
--EXPECT--
string(2) "ok"
