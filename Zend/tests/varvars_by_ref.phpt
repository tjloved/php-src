--TEST--
Accessing variable variables using referenced names
--FILE--
<?php

function fn1064263141()
{
    $name = 'var';
    $ref =& $name;
    ${$name} = 42;
    var_dump(isset(${$name}));
    unset(${$name});
    var_dump(isset(${$name}));
}
fn1064263141();
--EXPECT--
bool(true)
bool(false)
