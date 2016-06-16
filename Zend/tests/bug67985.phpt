--TEST--
Bug #67985 - Last used array index not copied to new array at assignment
--FILE--
<?php

function fn2102239410()
{
    $a = ['zero', 'one', 'two'];
    unset($a[2]);
    $b = $a;
    $a[] = 'three';
    $b[] = 'three';
    var_dump($a === $b);
}
fn2102239410();
--EXPECT--
bool(true)
