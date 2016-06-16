--TEST--
Ensure that ArrayObject acts like an array
--FILE--
<?php

function fn2068156262()
{
    $a = new ArrayObject();
    $a['foo'] = 'bar';
    echo reset($a);
    echo count($a);
    echo current($a);
}
fn2068156262();
--EXPECT--
bar1bar
