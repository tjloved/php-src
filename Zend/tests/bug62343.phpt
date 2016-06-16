--TEST--
Bug #62343 (Show class_alias In get_declared_classes())
--FILE--
<?php

class a
{
}
function fn1105702036()
{
    class_alias("a", "b");
    $c = get_declared_classes();
    var_dump(end($c));
    var_dump(prev($c));
}
fn1105702036();
--EXPECT--
string(1) "b"
string(1) "a"
