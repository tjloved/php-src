--TEST--
Accessing variable variables using referenced names
--FILE--
<?php

function fn277745223()
{
    $name = 'var';
    $ref =& $name;
    ${$name} = 42;
    var_dump(isset(${$name}));
    unset(${$name});
    var_dump(isset(${$name}));
}
fn277745223();
--EXPECT--
bool(true)
bool(false)
