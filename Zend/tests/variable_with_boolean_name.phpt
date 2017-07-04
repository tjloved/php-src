--TEST--
Variable with boolean name
--FILE--
<?php

function fn347725522()
{
    ${true} = 42;
    var_dump(${true});
    var_dump(${'1'});
}
fn347725522();
--EXPECT--
int(42)
int(42)
