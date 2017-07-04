--TEST--
Incrementing and decrementing a referenced property
--FILE--
<?php

function fn653836981()
{
    $obj = new stdClass();
    $obj->prop = 1;
    $ref =& $obj->prop;
    var_dump(++$obj->prop);
    var_dump($obj->prop);
    var_dump($obj->prop++);
    var_dump($obj->prop);
    var_dump(--$obj->prop);
    var_dump($obj->prop);
    var_dump($obj->prop--);
    var_dump($obj->prop);
}
fn653836981();
--EXPECT--
int(2)
int(2)
int(2)
int(3)
int(2)
int(2)
int(2)
int(1)
