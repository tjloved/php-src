--TEST--
Simple While Loop Test
--FILE--
<?php

function fn1479680605()
{
    $a = 1;
    while ($a < 10) {
        echo $a;
        $a++;
    }
}
fn1479680605();
--EXPECT--
123456789
