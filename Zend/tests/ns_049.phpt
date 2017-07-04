--TEST--
049: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php

const FOO = 0;
function fn570803563()
{
    var_dump(constant(__NAMESPACE__ . "\\FOO"));
}
fn570803563();
--EXPECT--
int(0)
