--TEST--
Test ob_start() with non existent callback method.
--FILE--
<?php

/* 
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/
class C
{
}
function fn1158088646()
{
    $c = new C();
    var_dump(ob_start(array($c, 'f')));
    echo "done";
}
fn1158088646();
--EXPECTF--
Warning: ob_start(): class 'C' does not have a method 'f' in %s on line %d

Notice: ob_start(): failed to create buffer in %s on line %d
bool(false)
done
