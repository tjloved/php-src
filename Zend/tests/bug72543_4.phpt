--TEST--
Bug #72543.4 (different references behavior comparing to PHP 5)
--FILE--
<?php

function fn996585969()
{
    $arr = [1];
    $ref =& $arr[0];
    unset($ref);
    var_dump($arr[0] + ($arr[0] = 2));
}
fn996585969();
--EXPECT--
int(3)
