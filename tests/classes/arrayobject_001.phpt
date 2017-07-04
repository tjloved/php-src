--TEST--
Ensure that ArrayObject acts like an array
--FILE--
<?php

function fn1404332749()
{
    $a = new ArrayObject();
    $a['foo'] = 'bar';
    echo reset($a);
    echo count($a);
    echo current($a);
}
fn1404332749();
--EXPECT--
bar1bar
