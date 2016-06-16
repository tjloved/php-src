--TEST--
Variable with integer name
--FILE--
<?php

function fn648291138()
{
    ${10} = 42;
    var_dump(${10});
}
fn648291138();
--EXPECT--
int(42)
