--TEST--
Bug #24445 (get_parent_class() returns the current class when passed an object)
--FILE--
<?php

class Test
{
}
function fn1597393281()
{
    var_dump(get_parent_class('Test'));
    $t = new Test();
    var_dump(get_parent_class($t));
}
fn1597393281();
--EXPECT--
bool(false)
bool(false)
