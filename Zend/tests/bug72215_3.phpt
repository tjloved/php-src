--TEST--
Bug #72215.3 (Wrong return value if var modified in finally)
--FILE--
<?php

function &test()
{
    try {
        return $a;
    } finally {
        $a = 2;
    }
}
function fn1937758296()
{
    var_dump(test());
}
fn1937758296();
--EXPECT--
int(2)
