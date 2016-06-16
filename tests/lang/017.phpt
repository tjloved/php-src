--TEST--
Testing user-defined function falling out of an If into another
--FILE--
<?php

function Test($a)
{
    if ($a < 3) {
        return 3;
    }
}
function fn1302281948()
{
    $a = 1;
    if ($a < Test($a)) {
        echo "{$a}\n";
        $a++;
    }
}
fn1302281948();
--EXPECT--
1
