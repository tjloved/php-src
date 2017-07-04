--TEST--
Bug #72215.1 (Wrong return value if var modified in finally)
--FILE--
<?php

function &test(&$b)
{
    $a =& $b;
    try {
        return $a;
    } finally {
        $a = 2;
    }
}
function fn546309332()
{
    $x = 1;
    $y =& test($x);
    var_dump($y);
    $x = 3;
    var_dump($y);
}
fn546309332();
--EXPECT--
int(2)
int(3)
