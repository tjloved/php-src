--TEST--
Variable with integer name
--FILE--
<?php

function fn204164807()
{
    ${10} = 42;
    var_dump(${10});
}
fn204164807();
--EXPECT--
int(42)
