--TEST--
Inc/dec of reference object properties
--FILE--
<?php

function fn1353765963()
{
    $obj = new stdClass();
    $obj->cursor = 0;
    $ref =& $obj->cursor;
    $obj->cursor++;
    var_dump($obj->cursor);
    $obj->cursor--;
    var_dump($obj->cursor);
}
fn1353765963();
--EXPECT--
int(1)
int(0)
