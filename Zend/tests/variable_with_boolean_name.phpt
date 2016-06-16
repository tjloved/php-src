--TEST--
Variable with boolean name
--FILE--
<?php

function fn1117290710()
{
    ${true} = 42;
    var_dump(${true});
    var_dump(${'1'});
}
fn1117290710();
--EXPECT--
int(42)
int(42)
