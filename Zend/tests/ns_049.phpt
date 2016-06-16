--TEST--
049: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php

const FOO = 0;
function fn558220684()
{
    var_dump(constant(__NAMESPACE__ . "\\FOO"));
}
fn558220684();
--EXPECT--
int(0)
